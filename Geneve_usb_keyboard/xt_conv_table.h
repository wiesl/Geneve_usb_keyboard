/*
 * XT Keyboard ASCII to SCANCODE conversion table
 */

//XT Keyboard table definition
typedef struct _xtkeyconv_
{
	uchar Asc_Code;		//Ascii code
	uchar Xt_Code;		//Xt Scan code
	uchar Xt_modifier;	//Xt Modifier (Control, Shift, Alt)
} XTKEYCONV;

//In order of (pseudo) ASCII code
static XTKEYCONV XtKeyConv[] =
{ //     Kb          Xt    Modifier				Usb
	{' ',        0x39, OP_ARPON + UP_STNDRD},                       //0x2C,
	{'!',        0x02, OP_NONE  + SM_SHIFT},                	//0x1E,
	{'\"',       0x28, OP_NONE  + SM_SHIFT},                	//0x34,
	{'#',        0x04, OP_NONE  + SM_SHIFT},                	//0x20,
	{'$',        0x05, OP_NONE  + SM_SHIFT},                	//0x21,
	{'%',        0x06, OP_NONE  + SM_SHIFT},                	//0x22,
	{'&',        0x08, OP_NONE  + SM_SHIFT},                	//0x24,
	{'\'',       0x28, OP_ARPON + UP_STNDRD},	// ['] ["]	//0x34,
	{'(',        0x0A, OP_NONE  + SM_SHIFT},                	//0x26,
	{')',        0x0B, OP_NONE  + SM_SHIFT},                	//0x27,
	{'*',        0x09, OP_NONE  + SM_SHIFT},                	//0x25,
	{'+',        0x0D, OP_NONE  + SM_SHIFT},                	//0x2E,
	{',',        0x33, OP_ARPON + UP_STNDRD},	// [,] [<]	//0x36,
	{'-',        0x0C, OP_NONE  + UP_STNDRD},                  	//0x2D,
	{'.',        0x34, OP_ARPON + UP_STNDRD},	// [.] [>]	//0x37,
	{'/',        0x35, OP_ARPON + UP_STNDRD},	// [/] [?]	//0x38,
	{'0',        0x0B, OP_ARPON + UP_STNDRD},	// [0] [)]	//0x27,
	{'1',        0x02, OP_ARPON + UP_STNDRD},	// [1] [!]	//0x1E,
	{'2',        0x03, OP_ARPON + UP_STNDRD},	// [2] [@]	//0x1F,
	{'3',        0x04, OP_ARPON + UP_STNDRD},	// [3] [#]	//0x20,
	{'4',        0x05, OP_ARPON + UP_STNDRD},	// [4] [$]	//0x21,
	{'5',        0x06, OP_ARPON + UP_STNDRD},	// [5] [%]	//0x22,
	{'6',        0x07, OP_ARPON + UP_STNDRD},	// [6] [^]	//0x23,
	{'7',        0x08, OP_ARPON + UP_STNDRD},	// [7] [&]	//0x24,
	{'8',        0x09, OP_ARPON + UP_STNDRD},	// [8] [*]	//0x25,
	{'9',        0x0A, OP_ARPON + UP_STNDRD},	// [9] [(]	//0x26,
	{':',        0x27, OP_NONE  + SM_SHIFT},                	//0x33,
	{';',        0x27, OP_ARPON + UP_STNDRD},	// [;] [:]	//0x33,
	{'<',        0x33, OP_NONE  + SM_SHIFT},                	//0x36,
	{'=',        0x0D, OP_ARPON + UP_STNDRD},	// [=] [+]	//0x2E,
	{'>',        0x34, OP_NONE  + SM_SHIFT},			//0x37,
	{'?',        0x35, OP_NONE  + SM_SHIFT},			//0x38,
	{'@',        0x03, OP_NONE  + SM_SHIFT},			//0x1F,
	{'A',        0x1E, OP_NONE  + SM_SHIFT},			//0x04,
	{'B',        0x30, OP_NONE  + SM_SHIFT},			//0x05,
	{'C',        0x2E, OP_NONE  + SM_SHIFT},			//0x06,
	{'D',        0x20, OP_NONE  + SM_SHIFT},			//0x07,
	{'E',        0x12, OP_NONE  + SM_SHIFT},			//0x08,
	{'F',        0x21, OP_NONE  + SM_SHIFT},			//0x09,
	{'G',        0x22, OP_NONE  + SM_SHIFT},			//0x0A,
	{'H',        0x23, OP_NONE  + SM_SHIFT},			//0x0B,
	{'I',        0x17, OP_NONE  + SM_SHIFT},			//0x0C,
	{'J',        0x24, OP_NONE  + SM_SHIFT},			//0x0D,
	{'K',        0x25, OP_NONE  + SM_SHIFT},			//0x0E,
	{'L',        0x26, OP_NONE  + SM_SHIFT},			//0x0F,
	{'M',        0x32, OP_NONE  + SM_SHIFT},			//0x10,
	{'N',        0x31, OP_NONE  + SM_SHIFT},			//0x11,
	{'O',        0x18, OP_NONE  + SM_SHIFT},			//0x12,
	{'P',        0x19, OP_NONE  + SM_SHIFT},			//0x13,
	{'Q',        0x10, OP_NONE  + SM_SHIFT},			//0x14,
	{'R',        0x13, OP_NONE  + SM_SHIFT},			//0x15,
	{'S',        0x1F, OP_NONE  + SM_SHIFT},			//0x16,
	{'T',        0x14, OP_NONE  + SM_SHIFT},			//0x17,
	{'U',        0x16, OP_NONE  + SM_SHIFT},			//0x18,
	{'V',        0x2F, OP_NONE  + SM_SHIFT},			//0x19,
	{'W',        0x11, OP_NONE  + SM_SHIFT},			//0x1A,
	{'X',        0x2D, OP_NONE  + SM_SHIFT},			//0x1B,
	{'Y',        0x15, OP_NONE  + SM_SHIFT},			//0x1C,
	{'Z',        0x2C, OP_NONE  + SM_SHIFT},			//0x1D,
	{'[',        0x1A, OP_ARPON + UP_STNDRD},	// [[] [{]	//0x2F,
	{'\\',       0x2B, OP_ARPON + UP_STNDRD},	// [\] [|]	//0x31,
	{']',        0x1B, OP_ARPON + UP_STNDRD},	// []] [}]	//0x30,
	{'^',        0x07, OP_NONE  + SM_SHIFT},			//0x23,
	{'_',        0x0C, OP_NONE  + SM_SHIFT},			//0x2D,
	{'`',        0x29, OP_ARPON + UP_STNDRD},	// [`] [~]	//0x35,
	{'a',        0x1E, OP_ARPON + UP_STNDRD},	// [a] [A]	//0x04,
	{'b',        0x30, OP_ARPON + UP_STNDRD},	// [b] [B]	//0x05,
	{'c',        0x2E, OP_ARPON + UP_STNDRD},	// [c] [C]	//0x06,
	{'d',        0x20, OP_ARPON + UP_STNDRD},	// [d] [D]	//0x07,
	{'e',        0x12, OP_ARPON + UP_STNDRD},	// [e] [E]	//0x08,
	{'f',        0x21, OP_ARPON + UP_STNDRD},	// [f] [F]	//0x09,
	{'g',        0x22, OP_ARPON + UP_STNDRD},	// [g] [G]	//0x0A,
	{'h',        0x23, OP_ARPON + UP_STNDRD},	// [h] [H]	//0x0B,
	{'i',        0x17, OP_ARPON + UP_STNDRD},	// [i] [I]	//0x0C,
	{'j',        0x24, OP_ARPON + UP_STNDRD},	// [j] [J]	//0x0D,
	{'k',        0x25, OP_ARPON + UP_STNDRD},	// [k] [K]	//0x0E,
	{'l',        0x26, OP_ARPON + UP_STNDRD},	// [l] [L]	//0x0F,
	{'m',        0x32, OP_ARPON + UP_STNDRD},	// [m] [M]	//0x10,
	{'n',        0x31, OP_ARPON + UP_STNDRD},	// [n] [N]	//0x11,
	{'o',        0x18, OP_ARPON + UP_STNDRD},	// [o] [O]	//0x12,
	{'p',        0x19, OP_ARPON + UP_STNDRD},	// [p] [P]	//0x13,
	{'q',        0x10, OP_ARPON + UP_STNDRD},	// [q] [Q]	//0x14,
	{'r',        0x13, OP_ARPON + UP_STNDRD},	// [r] [R]	//0x15,
	{'s',        0x1F, OP_ARPON + UP_STNDRD},	// [s] [S]	//0x16,
	{'t',        0x14, OP_ARPON + UP_STNDRD},	// [t] [T]	//0x17,
	{'u',        0x16, OP_ARPON + UP_STNDRD},	// [u] [U]	//0x18,
	{'v',        0x2F, OP_ARPON + UP_STNDRD},	// [v] [V]	//0x19,
	{'w',        0x11, OP_ARPON + UP_STNDRD},	// [w] [W]	//0x1A,
	{'x',        0x2D, OP_ARPON + UP_STNDRD},	// [x] [X]	//0x1B,
	{'y',        0x15, OP_ARPON + UP_STNDRD},	// [y] [Y]	//0x1C,
	{'z',        0x2C, OP_ARPON + UP_STNDRD},	// [z] [Z]	//0x1D,
	{'{',        0x1A, OP_NONE  + SM_SHIFT},			//0x2F,
	{'|',        0x2B, OP_NONE  + SM_SHIFT},			//0x31,
	{']',        0x1B, OP_NONE  + SM_SHIFT},			//0x30,
	{'~',        0x29, OP_NONE  + SM_SHIFT},			//0x35,

//Pseudo ASCII special keys
	{ SK_ENTER,  0x1C, OP_ARPON + UP_STNDRD}, 			//0x28,
	{ SK_ESCAPE, 0x01, OP_ARPON + UP_STNDRD}, 			//0x29,
	{ SK_BACKSP, 0x0E, OP_ARPON + UP_STNDRD}, 			//0x2A,
	{ SK_TAB,    0x0F, OP_ARPON + UP_STNDRD}, 			//0x2B,
	{ SK_PRTSCR, 0x37, OP_NONE  + UP_AACODE},			//0x46,
	{ SK_PAUSE,  0x46, OP_NONE  + UP_STNDRD},			//0x48,
	{ SK_CPSLCK, 0x3A, OP_NONE  + UP_STNDRD},			//0x39,
	{ SK_SCRLCK, 0xC5, OP_NONE  + UP_NOUPCD},			//0x47,
	{ SK_WMENU,  0x00, OP_NONE  + UP_NOUPCD},			//0x65,

//Pseudo ASCII function keys
	{ FK_F01,    0x3B, OP_ARPON + UP_STNDRD},			//0x3A,
	{ FK_F02,    0x3C, OP_ARPON + UP_STNDRD},			//0x3B,
	{ FK_F03,    0x3D, OP_ARPON + UP_STNDRD},			//0x3C,
	{ FK_F04,    0x3E, OP_ARPON + UP_STNDRD},			//0x3D,
	{ FK_F05,    0x3F, OP_ARPON + UP_STNDRD},			//0x3E,
	{ FK_F06,    0x40, OP_ARPON + UP_STNDRD},			//0x3F,
	{ FK_F07,    0x41, OP_ARPON + UP_STNDRD},			//0x40,
	{ FK_F08,    0x42, OP_ARPON + UP_STNDRD},			//0x41,
	{ FK_F09,    0x43, OP_ARPON + UP_STNDRD},			//0x42,
	{ FK_F10,    0x44, OP_ARPON + UP_STNDRD},			//0x43,
	{ FK_F11,    0x57, OP_ARPON + UP_STNDRD},			//0x44,
	{ FK_F12,    0x58, OP_ARPON + UP_STNDRD},			//0x45,

//Pseudo ASCII edit keys
	{ EK_INSERT, 0x52, OP_ARPON + OP_ESCSCD + UP_STNDRD },	//Edit [Insert]	//0x49,
	{ EK_DELETE, 0x53, OP_ARPON + OP_ESCSCD + UP_STNDRD },	//Edit [Delete]	//0x4C,
	{ EK_HOME,   0x47, OP_ARPON + OP_ESCSCD + UP_STNDRD },	//Edit [Home]	//0x4A,
	{ EK_END,    0x4F, OP_ARPON + OP_ESCSCD + UP_STNDRD },	//Edit [End]	//0x4D,
	{ EK_PAGEUP, 0x49, OP_ARPON + OP_ESCSCD + UP_STNDRD },	//Edit [PageUp]	//0x4B,
	{ EK_PAGEDN, 0x51, OP_ARPON + OP_ESCSCD + UP_STNDRD },	//Edit [PageDwn]//0x4E,
	{ EK_CHARLT, 0x4B, OP_ARPON + OP_ESCSCD + UP_STNDRD },	//Edit [<-]	//0x50,
	{ EK_CHARRT, 0x4D, OP_ARPON + OP_ESCSCD + UP_STNDRD },	//Edit [->]	//0x4F,
	{ EK_LINEDN, 0x50, OP_ARPON + OP_ESCSCD + UP_STNDRD },	//Edit [|^]	//0x51,
	{ EK_LINEUP, 0x48, OP_ARPON + OP_ESCSCD + UP_STNDRD },	//Edit [|v]	//0x52,

//Pseudo ASCII keypad keys
	{ NK_NUMLCK, 0x45, OP_ARPON + UP_STNDRD},	//Keypad [Numlock]	//0x53,
	{ NK_DIVIDE, 0x35, OP_ARPON + UP_STNDRD},	//Keypad [/]      	//0x54,
	{ NK_MLTPLY, 0x37, OP_ARPON + UP_STNDRD},	//Keypad [*]      	//0x55,
	{ NK_SBTRCT, 0x4A, OP_ARPON + UP_STNDRD},	//Keypad [-]      	//0x56,
	{ NK_ADDTN,  0x4E, OP_ARPON + UP_STNDRD},	//Keypad [+]      	//0x57,
	{ NK_ENTER,  0x1C, OP_ARPON + UP_STNDRD},	//Keypad [Enter]  	//0x58,
	{ NK_NMBR1,  0x5F, OP_ARPON + UP_STNDRD},	//Keypad [1]      	//0x59,
	{ NK_NMBR2,  0x50, OP_ARPON + UP_STNDRD},	//Keypad [2]      	//0x5A,
	{ NK_NMBR3,  0x51, OP_ARPON + UP_STNDRD},	//Keypad [3]      	//0x5B,
	{ NK_NMBR4,  0x4B, OP_ARPON + UP_STNDRD},	//Keypad [4]      	//0x5C,
	{ NK_NMBR5,  0x4C, OP_ARPON + UP_STNDRD},	//Keypad [5]      	//0x5D,
	{ NK_NMBR6,  0x4D, OP_ARPON + UP_STNDRD},	//Keypad [6]      	//0x5E,
	{ NK_NMBR7,  0x47, OP_ARPON + UP_STNDRD},	//Keypad [7]      	//0x5F,
	{ NK_NMBR8,  0x48, OP_ARPON + UP_STNDRD},	//Keypad [8]      	//0x60,
	{ NK_NMBR9,  0x49, OP_ARPON + UP_STNDRD},	//Keypad [9]      	//0x61,
	{ NK_NMBR0,  0x52, OP_ARPON + UP_STNDRD},	//Keypad [0]      	//0x62,
	{ NK_DECPNT, 0x53, OP_ARPON + UP_STNDRD},	//Keypad [.]      	//0x63,

//Pseudo ASCII modifier keys
	{ MK_LSHIFT, 0x2A, OP_NONE  + UP_STNDRD},	//Modifier [L Shift]
	{ MK_RSHIFT, 0x36, OP_NONE  + UP_STNDRD},	//Modifier [R Shift]
	{ MK_LCTRL,  0x1D, OP_NONE  + UP_STNDRD},	//Modifier [L Control]
	{ MK_RCTRL,  0x1D, OP_NONE  + UP_STNDRD},	//Modifier [R Control]
	{ MK_LALT,   0x38, OP_NONE  + UP_STNDRD},	//Modifier [L Alt]
	{ MK_RALT,   0x38, OP_NONE  + UP_STNDRD},	//Modifier [R Alt]
	{ MK_LGUI,   0x00, OP_NONE  + UP_NOUPCD},	//Modifier [L GUI]
	{ MK_RGUI,   0x00, OP_NONE  + UP_NOUPCD},	//Modifier [R GUI]

//Not defined key
	{ XK_NOTDEF, 0x00, OP_NONE  + UP_NOUPCD},	//Not defined
};
