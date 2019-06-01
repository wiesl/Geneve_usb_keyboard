/*
 * Geneve Usb Keyboard
 *
 * This program reads a (wireless) USB keyboard and converts the
 * user input to a XT keyboard output to be used for the Geneve
 * home computer.
 *
 * This program is designed around a Teensy 3.2 with a Sparkfun
 * Teensy 3.1 Arduino Shield Adapter and a Sparkfun USB host
 * shield.
 *
 *
 */

#include <hidboot.h>
#include <usbhub.h>

// Satisfy the IDE, which needs to see the include statment in the ino too.
#ifdef dobogusinclude
#include <spi4teensy3.h>
#include <SPI.h>
#endif

/*
 * DEBUG OPTIONS
 */
//#define DEBUG_SERIAL			//Serial debugger over USB

#if defined DEBUG_SERIAL

//#define DEBUG_MISC			//Miscelaneous debug
//#define DEBUG_USB_KB_READ		//USB KeayBoard Read
//#define DEBUG_USB_TO_XT		//USB to XT conversion
//#define DEBUG_USB_CAPSLCK		//Capslock on/ off
//#define DEBUG_USB_NUMLCK		//Numlock on /off
//#define DEBUG_USB_SP_KM		//SPecial Keyboard Mode

#endif


/*
 * Geneve Keyboard includes
 */
#include "Geneve_usb_keyboard.h"
#include "usb_conv_table.h"
#include "xt_conv_table.h"
#include "basic_table.h"
#include "mdos_table.h"


#ifndef _delay_us
#define _delay_us(n) delayMicroseconds(n)
#endif

#ifndef _delay_ms
#define _delay_ms(n) delay(n)
#endif

static XTKEYCONV *GetXtKeyConv(uchar chr);
static USBKEYCONV *GetUsbKeyConv(byte key);
static void Xt_Write_String(const char *string);
static void Xt_Write_String_Bsps(const char *string, byte bsps);
static void Xt_Write_Byte(byte data);
static void ConvUsbToXt(byte key_stat, byte usb_key);

static void UsbKbSpecialMode(byte ascii);
static void UsbKbEsc(const CMDLIST *CmdList);
static void UsbKbEscMode(void);
static void UsbKbTab(const CMDLIST *CmdList);
static void UsbKbTabMode(void);
static const KBSMLIST *NextCmd(const KBSMLIST *ThisCmd);
static const KBSMLIST *PrevCmd(const KBSMLIST *ThisCmd);
static const KBSMLIST *NextAltCmd(const KBSMLIST *ThisCmd);
static const KBSMLIST *PrevAltCmd(const KBSMLIST *ThisCmd);
static void UsbKbMode(byte ascii, const KBSMLIST *NewCmd);
static void UsbKbModeMdos(byte ascii);
static void UsbKbModeBasic(byte ascii);

//Pin assignment
#define XT_CLCK   0
#define XT_DATA   1
#define XT_RESET  2

//Some bit manipulation Macro's
#define SBO(p)    digitalWrite(p, HIGH)     	//Set Bit One
#define SBZ(p)    digitalWrite(p, LOW)      	//Set Bit Zero
#define TB(p)     (digitalRead(p) ? HIGH : LOW)	//Test Bit

//Xt Keyboard status
static byte XtReset = FALSE;
static bool XtCapsl = FALSE;
static bool XtNuml  = FALSE;
static bool XtShift = FALSE;
static bool XtCntrl = FALSE;
static bool XtAlt   = FALSE;

static byte UsbModSum   = 0;		// Modifier sum
static bool UsbLftShift = FALSE;	// Left  Shift down
static bool UsbRgtShift = FALSE;	// Right Shift Down
static bool UsbLftCtrl  = FALSE;	// Left  Control down
static bool UsbRgtCtrl  = FALSE;	// Right Control Down
static bool UsbLftAlt   = FALSE;	// Left  Alt down
static bool UsbRgtAlt   = FALSE;	// Right Alt Down
static bool UsbLftGui   = FALSE;	// Left  GUI down
static bool UsbRgtGui   = FALSE;	// Right GUI Down
static bool UsbWfkUp	= FALSE;	// Wait For Key Up
static byte KeybMode    = KBSM_UNDEF;	// Keyboard Special Mode

static byte      AsciiCmdList = 0;	// Previous Ascii code command list
static const KBSMLIST *OldCmd = NULL;	// Old (Previous) Command

static byte  CmdBufferCnt = 0;		// Command to find with <TAB> key
static byte  CmdBufferIdx = 0;		// Last command send with <TAB> key
static char  CmdBuffer[MAXCMDBUFSIZE+1];

static char  AutoRptXtCode = 0;		// Auto repeat XT scan code
static ulong AutoRptDelay  = 0;		// Auto repeat delay
static ulong AutoRptSpeed  = 0;		// Auto repeat speed

#if defined DEBUG_USB_TO_XT
/* PrintPressedKey - Print pressed key value
 *
 * Input : 
 * Return: 
 * Error : 
 */
static void PrintPressedKey(const char *what, const byte ascii)
{
	Serial.print(what);
	if ((ascii >= ' ') && (ascii <= '~'))
	{
		Serial.print((char)ascii);
	}
	else
	{
		Serial.print("?");
	}
	Serial.print(" (0x");
	Serial.print(ascii, HEX); 
	Serial.print(")");
}

/* PrintUsbXtKey - Print Usb To Xt conversion Key
 *
 * Input : 
 * Return: 
 * Error : 
 */
static void PrintUsbXtKey(byte key_stat, byte usb_key, byte usb_asc, byte xt_key, byte xt_asc)
{
	Serial.print(key_stat == KBKEY_DN ? "DOWN" : "UP  ");
	Serial.print(" UsbKey=0x");
	Serial.print(usb_key, HEX); 
	Serial.print(" XtKey=0x");
	Serial.print(xt_key, HEX); 
	PrintPressedKey(" UsbAscii=", usb_asc);
	PrintPressedKey(" XtAscii=",  xt_asc);
	Serial.println("");
}
#endif

/*
 * NextCmd - Find next command
 */
static const KBSMLIST *NextCmd(const KBSMLIST *ThisCmd)
{
	byte  old_cmdnmb = ThisCmd->cmdnmb;
	
	if (ThisCmd != NULL)
	{
		while (ThisCmd->cmdnmb == old_cmdnmb)
		{
			ThisCmd += 1;
		}
		if (ThisCmd->cmdnmb == 0)	//END: Goto first command and first alternative
		{
			while (ThisCmd->cmdnmb != 1) ThisCmd -= 1;
			if (ThisCmd->altnmb != 0)
			{
				while (ThisCmd->altnmb != 1) ThisCmd -= 1;
			}
		}
	}
	return ThisCmd;
}

/*
 * PrevCmd - Find previous command
 */
static const KBSMLIST *PrevCmd(const KBSMLIST *ThisCmd)
{
	byte  old_cmdnmb = ThisCmd->cmdnmb;
	
	if (ThisCmd != NULL)
	{
		if (ThisCmd->cmdnmb == 1)	//BEGIN: Goto last and first alternative
		{
			while (ThisCmd->cmdnmb != 0) ThisCmd += 1;
			ThisCmd -= 1;
		}
		else
		{
			while (ThisCmd->cmdnmb == old_cmdnmb) ThisCmd -= 1;
		}
		if (ThisCmd->altnmb != 0)	// Goto first alternative
		{
			while (ThisCmd->altnmb != 1) ThisCmd -= 1;
		}			
	}
	return ThisCmd;
}

/*
 * NextAltCmd - Find next alternative command (if any)
 */
static const KBSMLIST *NextAltCmd(const KBSMLIST *ThisCmd)
{
	if (ThisCmd != NULL)
	{
		if (ThisCmd->altnmb == 0) return NextCmd(ThisCmd); 

		if (ThisCmd->altnmb == 255)	// Goto first alternative
		{
			while (ThisCmd->altnmb != 1) ThisCmd -= 1;
		}
		else
		{
			ThisCmd += 1;		// Goto next alternative
		}
	}
	return ThisCmd;
}

/*
 * PrevAltCmd - Find previous command (if any)
 */
static const KBSMLIST *PrevAltCmd(const KBSMLIST *ThisCmd)
{
	if (ThisCmd != NULL)
	{
		if (ThisCmd->altnmb == 0) return PrevCmd(ThisCmd);

		if (ThisCmd->altnmb == 1)	// Goto last alternative
		{
			while (ThisCmd->altnmb != 255) ThisCmd += 1;
		}
		else
		{
			ThisCmd -= 1;		// Goto previous alternative
		}
	}
	return ThisCmd;
}

/* UsbKbMode - Handle Special Keyboard Mode
 *
 * Input :  ascii code off function
 *          a-z : Sends command
 *          up  : Sends previous command
 *          dn  : Sends next command
 *          lft : Sends previous alternative
 *          rgt : Sends next alternative
 * Return: 
 * Error : 
 */
static void UsbKbMode(byte ascii, const KBSMLIST *NewCmd)
{
	const char *s = NULL;
	byte  bsps = 0;
	byte  size;

	if (NewCmd != NULL)	// New command list
	{
		AsciiCmdList = ascii;
	}
	else	// Same command list, maybe next/prev [alt] command
	{
		switch (ascii)
		{
		case EK_LINEDN:	NewCmd = NextCmd   (OldCmd); break;
		case EK_LINEUP:	NewCmd = PrevCmd   (OldCmd); break;
		case EK_CHARRT:	NewCmd = NextAltCmd(OldCmd); break;
		case EK_CHARLT:	NewCmd = PrevAltCmd(OldCmd); break;
		default: break;
		}
	}
	
	if ((NewCmd != NULL) && (NewCmd != OldCmd))
	{
		if (OldCmd != NULL)	// Remove previous command
		{
			size = strlen(OldCmd->cmd);
			bsps = OldCmd->bsps;
			if (bsps < size) Xt_Write_Chars(EK_CHARLT, size-bsps);
			Xt_Write_Chars(EK_DELETE, size);
		}
		OldCmd = NewCmd;

		s = NewCmd->cmd;
		bsps = NewCmd->bsps;
#if defined DEBUG_USB_SP_KM
		Serial.print("*** FUNC: ");
		Serial.println(s);
#endif
		if (bsps == 0)
		{
			Xt_Write_String(s);
		}
		else
		{
			Xt_Write_String_Bsps(s, bsps);
		}
	}
}

/* UsbKbModeMdos - Handle Special Keyboard Mode MDOS
 *
 * Input :  ascii code off function
 *          a-z : Sends command
 *          up  : Sends previous command
 *          dn  : Sends next command
 *          lft : Sends previous alternative
 *          rgt : Sends next alternative
 * Return: 
 * Error : 
 */
static void UsbKbModeMdos(byte ascii)
{
	const KBSMLIST *NewCmd = NULL;
	
	if (ascii == AsciiCmdList)
	{
		ascii = EK_LINEDN;
	}
	switch (ascii)
	{
	case 'a': NewCmd = Mdos_Cmds_A; break;
	case 'b': NewCmd = Mdos_Cmds_B; break;
	case 'c': NewCmd = Mdos_Cmds_C; break;
	case 'd': NewCmd = Mdos_Cmds_D; break;
	case 'e': NewCmd = Mdos_Cmds_E; break;
	case 'f': NewCmd = Mdos_Cmds_F; break;
	case 'g': NewCmd = Mdos_Cmds_G; break;
	case 'h': NewCmd = Mdos_Cmds_H; break;
	case 'i': NewCmd = Mdos_Cmds_I; break;
	case 'l': NewCmd = Mdos_Cmds_L; break;
	case 'm': NewCmd = Mdos_Cmds_M; break;
	case 'p': NewCmd = Mdos_Cmds_P; break;
	case 'r': NewCmd = Mdos_Cmds_R; break;
	case 's': NewCmd = Mdos_Cmds_S; break;
	case 't': NewCmd = Mdos_Cmds_T; break;
	case 'v': NewCmd = Mdos_Cmds_V; break;
	default : break;
	}

	UsbKbMode(ascii, NewCmd);
}


/* UsbKbModeBasic - Handle Special Keyboard Mode BASIC
 *
 * Input :  ascii code off function
 *          a-z : Sends command
 *          up  : Sends previous command
 *          dn  : Sends next command
 *          lft : Sends previous alternative
 *          rgt : Sends next alternative
 * Return: 
 * Error : 
 */
static void UsbKbModeBasic(byte ascii)
{
	const KBSMLIST *NewCmd = NULL;
	
	if (ascii == AsciiCmdList)
	{
		ascii = EK_LINEDN;
	}
	switch (ascii)
	{
	case 'a': NewCmd = Basic_Cmds_A; break;
	case 'b': NewCmd = Basic_Cmds_B; break;
	case 'c': NewCmd = Basic_Cmds_C; break;
	case 'd': NewCmd = Basic_Cmds_D; break;
	case 'e': NewCmd = Basic_Cmds_E; break;
	case 'f': NewCmd = Basic_Cmds_F; break;
	case 'g': NewCmd = Basic_Cmds_G; break;
	case 'h': NewCmd = Basic_Cmds_H; break;
	case 'i': NewCmd = Basic_Cmds_I; break;
	case 'j': NewCmd = Basic_Cmds_J; break;
	case 'k': NewCmd = Basic_Cmds_K; break;
	case 'l': NewCmd = Basic_Cmds_L; break;
	case 'm': NewCmd = Basic_Cmds_M; break;
	case 'n': NewCmd = Basic_Cmds_N; break;
	case 'o': NewCmd = Basic_Cmds_O; break;
	case 'p': NewCmd = Basic_Cmds_P; break;
	case 'r': NewCmd = Basic_Cmds_R; break;
	case 's': NewCmd = Basic_Cmds_S; break;
	case 't': NewCmd = Basic_Cmds_T; break;
	case 'u': NewCmd = Basic_Cmds_U; break;
	case 'v': NewCmd = Basic_Cmds_V; break;
	default : break;
	}
	
	UsbKbMode(ascii, NewCmd);
}

/* UsbKbSpecialMode - Handle Special Keyboard modes
 *
 * Input :  ascii code for mode of function
 * Return: 
 * Error : 
 */
static void UsbKbSpecialMode(byte ascii)
{
	//Define new special mode
	if (UsbLftGui && UsbRgtGui)
	{
		KeybMode = KBSM_UNDEF;
		CmdBufferCnt = 0;
		CmdBufferIdx = 0;
#if defined DEBUG_USB_SP_KM
		Serial.println("*** MODE: OFF");
#endif
		switch(ascii)
		{
		case 'b':		
#if defined DEBUG_USB_SP_KM
			Serial.println("*** MODE: BASIC");
#endif
			KeybMode = KBSM_BASIC;
			break;
		case 'm':
#if defined DEBUG_USB_SP_KM
			Serial.println("*** MODE: MDOS");
#endif
			KeybMode = KBSM_MDOS;
			break;
		}
		return;
	}	

	switch(KeybMode)
	{
	case KBSM_MDOS : UsbKbModeMdos(ascii); break;
	case KBSM_BASIC: UsbKbModeBasic(ascii); break;
//	case KBSM_ASSM : UsbKbModeAssm(ascii); break;
//	case KBSM_C99  : UsbKbModeC99(ascii); break;
	case KBSM_UNDEF:
	default        : break;
	}
}

/* UsbKbEsc - Remove command suggestion
 *
 * Input :
 * Return: 
 * Error : 
 */
static void UsbKbEsc(const CMDLIST *CmdList)
{
	const CMDLIST *pCmd;
	byte  size;
	byte  bsps;
	
	pCmd = &CmdList[CmdBufferIdx-1];
	size = strlen(pCmd->cmd);
	if (size > CmdBufferCnt)
	{
		bsps = size - CmdBufferCnt;
		Xt_Write_Chars(EK_CHARLT, bsps);
		Xt_Write_Chars(EK_DELETE, bsps);
	}
}

/* UsbKbEscMode - Remove command suggestion
 *
 * Input :
 * Return: 
 * Error : 
 */
static void UsbKbEscMode(void)
{
	if (CmdBufferCnt == 0) return;	//Nothing in buffer
	if (CmdBufferIdx == 0) return;	//No previous suggestion

	switch(KeybMode)
	{
	case KBSM_MDOS : UsbKbEsc(MdosCmds); break;
	case KBSM_BASIC: UsbKbEsc(BasicCmds); break;
	case KBSM_UNDEF:
	default        : break;
	}
	CmdBufferIdx = 0;
}

/* UsbKbTab - Find (next) command that looks like command in CmdBuffer
 *
 * Input :
 * Return: 
 * Error : 
 */
static void UsbKbTab(const CMDLIST *CmdList)
{
	const CMDLIST *pCmd;
	byte  startIdx;
//	byte  size;
//	byte  bsps;
	
	if (CmdBufferCnt == 0) return;

	if (CmdBufferIdx == 0)		//Start at beginning of list
	{
		CmdBufferIdx = 1;		
	}
	else				//Remove previous send command suggestion
	{
		UsbKbEsc(CmdList);
#if 0
		pCmd = &CmdList[CmdBufferIdx-1];
		size = strlen(pCmd->cmd);
		if (size > CmdBufferCnt)
		{
			bsps = size - CmdBufferCnt;
			Xt_Write_Chars(EK_CHARLT, bsps);
			Xt_Write_Chars(EK_DELETE, bsps);
		}
#endif
		CmdBufferIdx += 1;	//Next suggestion;
	}
	
	startIdx = CmdBufferIdx;
	
	do
	{
		pCmd = &CmdList[CmdBufferIdx-1];
		if (pCmd->cmd == NULL)
		{
			CmdBufferIdx = 1;
			continue;
		}

		if (strncmp(CmdBuffer, pCmd->cmd, CmdBufferCnt) == 0)
		{
#if defined DEBUG_USB_SP_KM
			Serial.print("Found: ");
			Serial.println(pCmd->cmd);
#endif		
			Xt_Write_String(&pCmd->cmd[CmdBufferCnt]);
			return;
		}
		CmdBufferIdx += 1;
	}
	while (startIdx != CmdBufferIdx);
	CmdBufferIdx = 0;
#if defined DEBUG_USB_SP_KM
	Serial.println("Found: No Match");
#endif		
}

/* UsbKbTabMode - Find command that looks like command in CmdBuffer
 *
 * Input :
 * Return: 
 * Error : 
 */
static void UsbKbTabMode(void)
{
	switch(KeybMode)
	{
	case KBSM_MDOS : UsbKbTab(MdosCmds); break;
	case KBSM_BASIC: UsbKbTab(BasicCmds); break;
	case KBSM_UNDEF:
	default        : break;
	}
}


/* ConvUsbToXt - Convert Usb Key to Xt Key
 *
 * Input : 
 * Return: 
 * Error : 
 */
static void ConvUsbToXt(byte key_stat, byte usb_key)
{
	USBKEYCONV *pUsbKeyConv;
	XTKEYCONV  *pXtKeyConv;
	byte       xt_code;
	byte       xt_asc;
	byte       usb_asc;
	byte       cmd_asc;

	//Get USB key
	pUsbKeyConv = GetUsbKeyConv(usb_key);
	if (pUsbKeyConv == NULL)
	{
#if defined DEBUG_USB_TO_XT
		PrintUsbXtKey(key_stat, usb_key, 0x00, 0x00, 0x00);
		Serial.println("GetUsbKeyConv returned <NULL>");
#endif
		return;
	}
	usb_asc = pUsbKeyConv->Asc_Code;
	
	//Convert TAB to F7 (== Alt+7 == Fctn+7)
	if (usb_asc == SK_TAB)
	{
		if (KeybMode == KBSM_UNDEF)
		{
			pUsbKeyConv = GetUsbKeyConv(0x40);	//USB FK_F07
		}
		else
		{
			if (key_stat == KBKEY_DN) UsbKbTabMode();
			return;
		}
	}
	
	//Remove TAB command suggestion
	if ((usb_asc == SK_ESCAPE) && (KeybMode != KBSM_UNDEF))
	{
		if (key_stat == KBKEY_DN) UsbKbEscMode();
		return;
	}

	//Switch to special keyboard mode	
	if (UsbLftGui || UsbRgtGui)
	{
		if (key_stat == KBKEY_DN)
		{
			UsbWfkUp = TRUE;
			UsbKbSpecialMode(usb_asc);
			return;
		}
	}
	if (UsbWfkUp && (key_stat == KBKEY_UP))
	{
		UsbWfkUp = FALSE;
		return;
	}
	
	//Save char to find command with <TAB> key
	if ((KeybMode != KBSM_UNDEF) && (key_stat == KBKEY_DN))
	{
		do
		{
			cmd_asc = 0xFF;
			
			if ((usb_asc == MK_LSHIFT) || (usb_asc == MK_RSHIFT)
			||  (usb_asc == MK_LCTRL ) || (usb_asc == MK_RCTRL )
			||  (usb_asc == MK_LALT  ) || (usb_asc == MK_RALT  ))
			{
				break;
			}

			if (UsbLftCtrl || UsbRgtCtrl || UsbLftAlt || UsbRgtAlt)
			{
				cmd_asc = 0x00; break;
			}

			if ((usb_asc >= 'a') && (usb_asc <= 'z'))
			{
				cmd_asc = usb_asc; break;
			}
			if (isdigit(usb_asc) && !UsbLftShift && !UsbRgtShift)
			{
				cmd_asc = usb_asc; break;
			}
			cmd_asc = 0x00;
		}
		while (0);
		
		switch(cmd_asc)
		{
		case 0xFF: break;
		case 0x00: CmdBufferCnt = 0; break;
		default  : 
			if (CmdBufferCnt < MAXCMDBUFSIZE)
			{
				CmdBuffer[CmdBufferCnt++] = toupper(cmd_asc);
			}
			break;
		}
		CmdBuffer[CmdBufferCnt]   = '\0';
		
#if defined DEBUG_USB_SP_KM
		Serial.print(" CmdBuffer: [");
		Serial.print(CmdBuffer);
		Serial.println("]");
#endif
	}
	
	//Get XT key
	pXtKeyConv = GetXtKeyConv(pUsbKeyConv->Asc_Code);
	if (pXtKeyConv == NULL)
	{
#if defined DEBUG_USB_TO_XT
		PrintUsbXtKey(key_stat, usb_key, 0x00, 0x00, 0x00);
		Serial.println("GetXtKeyConv returned <NULL>");
#endif
		return;
	}

	//Reset Command List
	AsciiCmdList = 0;
	OldCmd       = NULL;
	CmdBufferIdx = 0;

	xt_code = pXtKeyConv->Xt_Code;
	if (xt_code == 0x00)
	{
#if defined DEBUG_USB_TO_XT
		PrintUsbXtKey(key_stat, usb_key, 0x00, xt_code, 0x00);
		Serial.println("*** Not an XT key!");
#endif
		return;				//Nothing to send
	}
	
	if (key_stat == KBKEY_UP)
	{
		switch(pXtKeyConv->Xt_modifier & UP_BITMSK)
		{
		case UP_NOUPCD: return;
		case UP_STNDRD: xt_code |= 0x80; break;
		case UP_AACODE: xt_code = 0xAA;  break;
		default: break;
		}
	}
//	usb_asc = pUsbKeyConv->Asc_Code;
	xt_asc  = pXtKeyConv->Asc_Code;
	AutoRptXtCode = 0;
	AutoRptDelay  = 0;
	AutoRptSpeed  = 0;
	
	if (key_stat == KBKEY_DN)
	{
		switch(xt_asc)
		{
		case SK_CPSLCK:
			XtCapsl = !XtCapsl;
#if defined DEBUG_USB_CAPSLCK
			Serial.print("XtCapsLock=");
			Serial.println(XtCapsl ? "ON" : "OFF");
#endif
			break;

		case NK_NUMLCK:
			XtNuml = !XtNuml;
#if defined DEBUG_USB_NUMLCK
			Serial.print("XtNumLock=");
			Serial.println(XtNuml ? "ON" : "OFF");
#endif
			break;

		default: break;
		}
		
		//Set autorepeat scan code for keys with Auto repeat option
		if (pXtKeyConv->Xt_modifier & OP_ARPON)
		{
			AutoRptXtCode = xt_code;
		}
	}

#if defined DEBUG_USB_TO_XT
	PrintUsbXtKey(key_stat, usb_key, usb_asc, xt_code, xt_asc);
#endif
	
	//Handle keys with escaped scancode
	if (XtNuml && (pXtKeyConv->Xt_modifier & OP_ESCSCD))
	{
		if (key_stat == KBKEY_DN)
		{
			Xt_Write_Byte(0xE0);	//Escaped Shift DN
			Xt_Write_Byte(0x2A);
		}
		Xt_Write_Byte(0xE0);		//Escaped ...
		Xt_Write_Byte(xt_code);

		if (key_stat == KBKEY_UP)
		{
			Xt_Write_Byte(0xE0);	//Escaped Shift UP
			Xt_Write_Byte(0xAA);
		}
	}
	else
	{
		//Send the XT keyboard code
		Xt_Write_Byte(xt_code);
	}
}

/* GetXtKeyConv - Get XT Key Conversion struct
 *
 * Input : (pseudo) ascii code
 * Return: XtKeyConv
 * Error : NULL
 */
static XTKEYCONV *GetXtKeyConv(uchar chr)
{
  if ((chr >= XT_FIRST_CODE) && (chr <= XT_LAST_CODE))
  {
	return &XtKeyConv[chr-' '];
  }
  return NULL;
}

/* GetUsbKeyConv - Get USB Key Conversion Struct
 *
 * Input : USB key code
 * Return: UsbKeyConv
 * Error : NULL
 */
static USBKEYCONV *GetUsbKeyConv(byte key)
{
  if ((key >= USB_FIRST_CODE) && (key <= USB_LAST_CODE))
  {
	return &UsbKeyConv[key-0x04];
  }
  return NULL;
}

static bool Xt_Handle_Modifier(byte mf, byte mftype, bool mfstatus, byte mfkey)
{
	if (mf & mftype)
	{
		if (!mfstatus)
		{
			mfstatus = TRUE;
			Xt_Write_Byte(mfkey);   //modifier down
			_delay_ms(20);
		}
	}
	else
	{
		if (mfstatus)
		{
			mfstatus = FALSE;
			Xt_Write_Byte(mfkey|0x80);  //modifier up
			_delay_ms(20);
		}
	}
	return mfstatus;
}

/* Xt_Write_Chars - Write XT keyboard N Characters
 *
 */
static void Xt_Write_Chars(byte chr, byte cnt)
{
	byte sc;
	byte ix;
	XTKEYCONV *pXtKeyConv;
	
	pXtKeyConv = GetXtKeyConv(chr);
	sc = pXtKeyConv->Xt_Code;

	for (ix=0; ix<cnt; ix+=1)
	{
		Xt_Write_Byte(sc);
		_delay_ms(10);	//was 40
		Xt_Write_Byte(sc|0x80);
		_delay_ms(10);	//was 40
	}
}

/* Xt_Write_String_Bsps - Write an XT keyboard string and N backspaces
 *
 */
static void Xt_Write_String_Bsps(const char *string, byte bsps)
{
	Xt_Write_String(string);
	Xt_Write_Chars(EK_CHARLT, bsps);
}

/* Xt_Write_String - Write an XT keyboard string
 *
 */
static void Xt_Write_String(const char *string)
{
	byte sc;
	byte mf;
	char chr;
	XTKEYCONV *pXtKeyConv;

	while(*string != '\0')
	{
		mf = 0;
		chr = *string++;
		
		//Preserve case of predefined texts
		if (XtCapsl)
		{
			chr = islower(chr) ? toupper(chr) : tolower(chr);
		}
		
		if (chr == '^')		//Add Control key
		{
			if (*string != '\0')
			{
				chr = *string++;
				if (chr != '^')
				{
					mf |= SM_CTRL;
				}
			}
		}

		pXtKeyConv = GetXtKeyConv(chr);
		mf |= (pXtKeyConv->Xt_modifier & SM_BITMSK);
		
		XtCntrl = Xt_Handle_Modifier(mf, SM_CTRL,  XtCntrl, 0x1D);
		XtAlt   = Xt_Handle_Modifier(mf, SM_ALT,   XtAlt,   0x38);
		XtShift = Xt_Handle_Modifier(mf, SM_SHIFT, XtShift, 0x2A);
		
		if (pXtKeyConv != NULL)
		{
			sc = pXtKeyConv->Xt_Code;
			Xt_Write_Byte(sc);
			_delay_ms(10);		//was 40
			Xt_Write_Byte(sc|0x80);
			_delay_ms(10);		//was 40
		}
	}

	XtShift = Xt_Handle_Modifier(0, SM_SHIFT, XtShift, 0x2A);
	XtAlt   = Xt_Handle_Modifier(0, SM_ALT,   XtAlt,   0x38);
	XtCntrl = Xt_Handle_Modifier(0, SM_CTRL,  XtCntrl, 0x1D);
}

/* Xt_Write_Byte - Write an XT keyboard byte
 *
 */
static void Xt_Write_Byte(byte data)
{
/* PRO-LINE XT-Keyboard

      :      <-------------------------270us------------------>
 uS   :     | 15 | 15  | 15 | 15 |                             | 1ms ->
      : ____      _10_5      ____      ____         _____       ________
CLOCK :     |____|     |____|    |____|    |.......|     |_____|
      : _____________ _________ _________          ____ ________________
DATA  :              X___B0____X___B1____XXXXXXXXXX_B7_X________/

            1    2   3 4    5                                  6 7
*/
// while (digitalRead(xt_clk) != HIGH) ;

	byte mask = 0x01;

	SBZ(XT_CLCK);			//1
	SBO(XT_DATA);
	_delay_us(15);

	while (mask != 0)
	{
		SBO(XT_CLCK);		//2&5
		_delay_us(10);
		if (data & mask) {	//3
			SBO(XT_DATA);
		} else {
			SBZ(XT_DATA);
		}
		_delay_us(5);
		SBZ(XT_CLCK);		//4
		_delay_us(15);
		mask <<= 1;
	}
	SBO(XT_CLCK);			//6
	_delay_us(10);
	SBO(XT_DATA);			//7
	_delay_ms(1);
}


/* USB Keyboard stuff */
/***************************************************************************/
//#include "Usb_Keyboard.h"

class KbdRptParser : public KeyboardReportParser
{
    void PrintKey(uint8_t mod, uint8_t key);

  protected:
    void OnControlKeysChanged(uint8_t before, uint8_t after);

    void OnKeyDown   (uint8_t mod, uint8_t key);
    void OnKeyUp     (uint8_t mod, uint8_t key);
    void OnKeyPressed(uint8_t key);
};

#if defined DEBUG_USB_KB_READ
void KbdRptParser::PrintKey(uint8_t m, uint8_t key)
{
	MODIFIERKEYS mod;
	*((uint8_t*)&mod) = m;
	Serial.print((mod.bmLeftCtrl   == 1) ? "C" : " ");
	Serial.print((mod.bmLeftShift  == 1) ? "S" : " ");
	Serial.print((mod.bmLeftAlt    == 1) ? "A" : " ");
	Serial.print((mod.bmLeftGUI    == 1) ? "G" : " ");
	
	Serial.print(" >");
	Serial.print(key, HEX); 
	Serial.print("< ");
	
	Serial.print((mod.bmRightCtrl   == 1) ? "C" : " ");
	Serial.print((mod.bmRightShift  == 1) ? "S" : " ");
	Serial.print((mod.bmRightAlt    == 1) ? "A" : " ");
	Serial.println((mod.bmRightGUI    == 1) ? "G" : " ");
};
#endif

void KbdRptParser::OnKeyDown(uint8_t mod, uint8_t key)
{
#if defined DEBUG_USB_KB_READ
	Serial.print("DN ");
	PrintKey(mod, key);
	uint8_t c = OemToAscii(mod, key);

	if (c) OnKeyPressed(c);
#endif
	ConvUsbToXt(KBKEY_DN, key);
}

void KbdRptParser::OnKeyUp(uint8_t mod, uint8_t key)
{
#if defined DEBUG_USB_KB_READ
	Serial.print("UP ");
	PrintKey(mod, key);
#endif
	ConvUsbToXt(KBKEY_UP, key);
}

#if defined DEBUG_USB_KB_READ
void KbdRptParser::OnKeyPressed(uint8_t key)
{
	Serial.print("ASCII: ");
	Serial.println((char)key);
};
#endif

#if defined DEBUG_USB_KB_READ
void printUpDn(const char *what, uint8_t state)
{
	Serial.print(what);
	Serial.println(state ? " DN" : " UP");
}
#endif

void KbdRptParser::OnControlKeysChanged(uint8_t before, uint8_t after)
{

	MODIFIERKEYS beforeMod;
	*((uint8_t*)&beforeMod) = before;

	MODIFIERKEYS afterMod;
	*((uint8_t*)&afterMod) = after;
	
	UsbModSum = (byte) after;

/*
 *          Left  Right
 * Control  0x01  0x10
 * Shift    0x02  0x20
 * Alt      0x04  0x40
 * Gui      0x08  0x80
 */
#if defined DEBUG_USB_KB_READ
	Serial.print("B=");
	Serial.print(before, HEX); 
	Serial.print(" A=");
	Serial.print(after, HEX); 
	Serial.print(" ");
#endif

	if (beforeMod.bmLeftCtrl != afterMod.bmLeftCtrl) {
#if defined DEBUG_USB_KB_READ
		printUpDn("LeftCtrl", afterMod.bmLeftCtrl);
#endif
		UsbLftCtrl = afterMod.bmLeftCtrl;
		ConvUsbToXt(afterMod.bmLeftCtrl ? KBKEY_DN : KBKEY_UP, USB_LCTRL);
	}
	if (beforeMod.bmLeftShift != afterMod.bmLeftShift) {
#if defined DEBUG_USB_KB_READ
		printUpDn("LeftShift", afterMod.bmLeftShift);
#endif
		UsbLftShift = afterMod.bmLeftShift;
		ConvUsbToXt(afterMod.bmLeftShift ? KBKEY_DN : KBKEY_UP, USB_LSHIFT);
	}
	if (beforeMod.bmLeftAlt != afterMod.bmLeftAlt) {
#if defined DEBUG_USB_KB_READ
		printUpDn("LeftAlt", afterMod.bmLeftAlt);
#endif
		UsbLftAlt = afterMod.bmLeftAlt;
		ConvUsbToXt(afterMod.bmLeftAlt ? KBKEY_DN : KBKEY_UP, USB_LALT);
	}
	if (beforeMod.bmLeftGUI != afterMod.bmLeftGUI) {
#if defined DEBUG_USB_KB_READ
		printUpDn("LeftGUI", afterMod.bmLeftGUI);
#endif
		UsbLftGui = afterMod.bmLeftGUI;
		ConvUsbToXt(afterMod.bmLeftGUI ? KBKEY_DN : KBKEY_UP, USB_LGUI);
	}

	if (beforeMod.bmRightCtrl != afterMod.bmRightCtrl) {
#if defined DEBUG_USB_KB_READ
		printUpDn("RightCtrl", afterMod.bmRightCtrl);
#endif
		UsbRgtCtrl = afterMod.bmRightCtrl;
		ConvUsbToXt(afterMod.bmRightCtrl ? KBKEY_DN : KBKEY_UP, USB_RCTRL);
	}
	if (beforeMod.bmRightShift != afterMod.bmRightShift) {
#if defined DEBUG_USB_KB_READ
		printUpDn("RightShift", afterMod.bmRightShift);
#endif
		UsbRgtShift = afterMod.bmRightShift;
		ConvUsbToXt(afterMod.bmRightShift ? KBKEY_DN : KBKEY_UP, USB_RSHIFT);
	}
	if (beforeMod.bmRightAlt != afterMod.bmRightAlt) {
#if defined DEBUG_USB_KB_READ
		printUpDn("RightAlt", afterMod.bmRightAlt);
#endif
		UsbRgtAlt = afterMod.bmRightAlt;
		ConvUsbToXt(afterMod.bmRightAlt ? KBKEY_DN : KBKEY_UP, USB_RALT);
	}
	if (beforeMod.bmRightGUI != afterMod.bmRightGUI) {
#if defined DEBUG_USB_KB_READ
		printUpDn("RightGUI", afterMod.bmRightGUI);
#endif
		UsbRgtGui = afterMod.bmRightGUI;
		ConvUsbToXt(afterMod.bmRightGUI ? KBKEY_DN : KBKEY_UP, USB_RGUI);
	}
}


/*
 * Xt_AutoRepeat - Do an XT Keyboard Autorepeat simulation
 */
static void Xt_AutoRepeat(void)
{
	static ulong PrevMillis = 0;
	ulong CurMillis = millis();
	ulong PastMillis;
	
	if (PrevMillis == 0)
	{
		PastMillis = 0;
	}
	else
	{
		PastMillis = CurMillis - PrevMillis;
	}
	PrevMillis = CurMillis;
	
	if (AutoRptXtCode != 0)
	{
		if (AutoRptDelay >= AUTOREPEATDELAY)
		{
			AutoRptSpeed += PastMillis;
			if (AutoRptSpeed >= AUTOREPEATSPEED)
			{
				AutoRptSpeed = 0;
				Xt_Write_Byte(AutoRptXtCode);
			}
		}
		else
		{
			AutoRptDelay += PastMillis;
			AutoRptSpeed  = 0;
		}
	}
	else
	{
		AutoRptDelay = 0;
		AutoRptSpeed = 0;
	}
}


/*
 * MAIN PROGRAM
 */
USB     Usb;
//USBHub     Hub(&Usb);
HIDBoot<HID_PROTOCOL_KEYBOARD>    HidKeyboard(&Usb);

//uint32_t next_time;

KbdRptParser Prs;

//For Teensy3 with Sparkfun hub
#define MAX_RESET 7 //MAX3421E pin 12
#define MAX_GPX   8 //MAX3421E pin 17


void setup()
{
#if defined DEBUG_SERIAL
 	Serial.begin( 115200 );
#if !defined(__MIPSEL__)
  	while (!Serial); // Wait for serial port to connect - used on Leonardo, Teensy and other boards with built-in USB CDC serial connection
#endif
	Serial.println("Start");
#endif
	
	pinMode(MAX_GPX, INPUT);
	pinMode(MAX_RESET, OUTPUT);
	
	pinMode(XT_CLCK,  OUTPUT);
	pinMode(XT_DATA,  OUTPUT);
	pinMode(XT_RESET, INPUT_PULLUP);
	
	SBO(XT_CLCK);
	SBO(XT_DATA);
	
	digitalWrite(MAX_RESET, LOW);
	_delay_ms(20);
	digitalWrite(MAX_RESET, HIGH);
	_delay_ms(20);

	Xt_Write_Byte(0xAA);	//Keyboard powered on with no errors
	_delay_ms(500);

	if (Usb.Init() == -1)
	{
#if defined DEBUG_MISC
		Serial.println("OSC did not start.");
#endif
	}
	_delay_ms(200);

//	next_time = millis() + 5000;

	HidKeyboard.SetReportParser(0, (HIDReportParser*)&Prs);
}

void loop()
{
	
	Usb.Task();
	Xt_AutoRepeat();
	
	if ((TB(XT_RESET) == LOW) && XtReset)
	{
		XtCapsl = FALSE;
		XtNuml  = FALSE;
		XtShift = FALSE;
		XtCntrl = FALSE;
		XtAlt   = FALSE;
		
		UsbModSum   = 0;
		UsbLftShift = FALSE;
		UsbRgtShift = FALSE;
		UsbLftCtrl  = FALSE;
		UsbRgtCtrl  = FALSE;
		UsbLftAlt   = FALSE;
		UsbRgtAlt   = FALSE;
		UsbLftGui   = FALSE;
		UsbRgtGui   = FALSE;
		UsbWfkUp    = FALSE;
		KeybMode    = KBSM_UNDEF;

		AsciiCmdList = 0;
		OldCmd = NULL;

		CmdBufferCnt = 0;
		CmdBufferIdx = 0;

		XtReset = FALSE;
	}
	else
	{
		XtReset = TRUE;
	}
}
