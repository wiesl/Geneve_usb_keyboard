/*
 * MDos Commands
 */

/*
 * When the keyboard is in MDOS mode ...
 * These lists are used with:
 * GUI+a..z
 * GUI+arrow left
 * GUI+arrow right
 * GUI+arrow up
 * GUI+arrow down
 *
 * db  data bits
 * bd  baudrate
 * dlr drive letter
 * dn  device name
 * fn  file name
 * hh:mm:ss  hour minute second
 * lbl label
 * msg message
 * nbr number
 * nm  name (max 10 characters)
 * pa  parity
 * pn  path name 
 * sb  stop bits
 * sdt slot drive type
 * var variable
 * yy-mm-dd  year-month-day
 *
 * X/Y       Choose X or Y
 * ON/OFF    Choose ON or OFF etc
 */
static const KBSMLIST Mdos_Cmds_A[] =
{
	{ 1,  0, 3, "ASSIGN x=y"},
	{ 2,  1, 2, "ATTRIB fn"},
	{ 2,  2, 2, "ATTRIB +R fn"},
	{ 2,255, 2, "ATTRIB -R fn"},
	{ 0,  0, 0, NULL},
};

static const KBSMLIST Mdos_Cmds_B[] =
{
	{ 1,  0, 3, "BUFFERS=nbr"},
	{ 0,  0, 0, NULL},
};
static const KBSMLIST Mdos_Cmds_C[] =
{
	{ 1,  1, 0, "CASE" },
	{ 1,255, 0, "CASE ON/OFF" },
	{ 2,  0, 2, "CD pn" },
	{ 3,  0, 2, "CHDIR pn" },
	{ 4,  1, 3, "CHKDSK dn:" },
	{ 4,  2, 5, "CHKDSK dn:fn" },
	{ 4,  3, 4, "CHKDSK dn:*" },
	{ 4,255, 6, "CHKDSK dn: /F" },
	{ 5,  0, 0, "CLS" },
	{ 6,  0, 5, "COPY fn fn" },
	{ 0,  0, 0, NULL},
};

static const KBSMLIST Mdos_Cmds_D[] =
{
	{ 1,  0, 8, "DATE mm-dd-yy" },
	{ 2,  0, 5, "DEL dn:fn" },
	{ 3,  1, 5, "DIR dn:fn" },
	{ 3,  2, 6, "DIR dn: /W" },
	{ 3,255, 6, "DIR dn: /P" },
	{ 4,  0, 7, "DISKCOMP dn: dn:" },
	{ 5,  0, 7, "DISKCOPY dn: dn:" },
	{ 0,  0, 0, NULL},
};

static const KBSMLIST Mdos_Cmds_E[] =
{
	{ 1,  1, 3, "ECHO msg" },
	{ 1,255, 0, "ECHO ON/OFF" },
	{ 2,  0, 5, "ERASE dn:fn" },
	{ 0,  0, 0, NULL},
};

static const KBSMLIST Mdos_Cmds_F[] =
{
	{ 1,  0, 3, "FILES=nbr" },
	{ 2,  0,19, "FOR %%var IN (set) DO cmd" },
	{ 3,  1,10, "FORMAT dn: /v /16" },
	{ 3,  2,10, "FORMAT dn: /v /18" },
	{ 3,  3,13, "FORMAT dn: /v /16 /1" },
	{ 3,255,13, "FORMAT dn: /v /18 /1" },
	{ 0,  0, 0, NULL},
};

static const KBSMLIST Mdos_Cmds_G[] =
{
	{ 1,  0, 3, "GOTO lbl" },
	{ 0,  0, 0, NULL},
};

static const KBSMLIST Mdos_Cmds_H[] =
{
	{ 1,  1, 0, "HARD" },
	{ 1,255, 0, "HARD ON/OFF" },
	{ 0,  0, 0, NULL},
};

static const KBSMLIST Mdos_Cmds_I[] =
{
	{ 1,  1, 0, "IF" },
	{ 1,255, 0, "IF NOT" },
	{ 0,  0, 0, NULL},
};

static const KBSMLIST Mdos_Cmds_L[] =
{
	{ 1,  0, 6, "LABEL dn: nm"},
	{ 2,  0, 3, "LASTDRIVE=dlr"},
	{ 0,  0, 0, NULL},
};

static const KBSMLIST Mdos_Cmds_M[] =
{
	{ 1,  0, 2, "MD pn" },
	{ 2,  0, 2, "MKDIR pn" },
	{ 3,  1, 0, "MODE 40" },
	{ 3,  2, 0, "MODE 80" },
	{ 3,  3, 0, "MODE PIO:80" },
	{ 3,  4, 0, "MODE PIO:132" },
	{ 3,  5, 0, "MODE RS232/1:" },
	{ 3,255,11, "MODE RS232/1:bd,pa,db,sb" },
	{ 0,  0, 0, NULL},
};

static const KBSMLIST Mdos_Cmds_P[] =
{
	{ 1,  0, 4, "PATH path" },
	{ 2,  0, 3, "PAUSE msg" },
	{ 3,  0, 3, "PROMPT msg" },
	{ 4,  1, 0, "PSYS" },
	{ 4,255, 6, "PSYS ON/OFF" },
	{ 0,  0, 0, NULL},
};

static const KBSMLIST Mdos_Cmds_R[] =
{
	{ 1,  0, 4, "RAMDISK size" },
	{ 2,  0, 2, "RD pn" },
	{ 3,  1, 0, "REMAP" },
	{ 3,255, 3, "REMAP sdt" },
	{ 4,  1, 8, "REN dn:fn fn" },
	{ 4,255, 8, "RENAME dn:fn fn" },
	{ 5,  0, 2, "RMDIR pn" },
	{ 6,  0, 3, "REM msg" },
	{ 0,  0, 0, NULL},
};

static const KBSMLIST Mdos_Cmds_S[] =
{
	{ 1,  0, 0, "SETDSK" },
	{ 2,  0, 4, "SPOOL size" },
	{ 0,  0, 0, NULL},
};

static const KBSMLIST Mdos_Cmds_T[] =
{
	{ 1,  0, 8, "TIME hh:mm:ss" },
	{ 2,  1, 0, "TI" },
	{ 2,255, 6, "TI ON/OFF" },
	{ 3,  0, 0, "TIMODE" },
	{ 4,  1, 3, "TREE dn:" },
	{ 4,255, 6, "TREE dn: /F" },
	{ 5,  0, 5, "TYPE dn:fn" },
	{ 0,  0, 0, NULL},
};

static const KBSMLIST Mdos_Cmds_V[] =
{
	{ 1,  0, 0, "VER" },
	{ 2,  1, 0, "VERIFY" },
	{ 2,255, 6, "VERIFY ON/OFF" },
	{ 3,  1, 0, "VIDEO" },
	{ 3,255, 9, "VIDEO SLOW/FAST" },
	{ 4,  0, 2, "VOL dn" },
	{ 0,  0, 0, NULL},
};

/*
 * When the keyboard is in MDOS mode ...
 * This list is used with the TAB key
 */
static const CMDLIST MdosCmds[] =
{
/* A */
	{ "ASSIGN" },
	{ "ATTRIB" },
	{ "ATTRIB +R" },
	{ "ATTRIB -R" },
/* B */
/* C */
	{ "CASE" },
	{ "CD" },
	{ "CHDIR" },
	{ "CHKDSK" },
	{ "CHKDSK A:*" },
	{ "CHKDSK A: /F" },
	{ "CLS" },
	{ "COPY" },
/* D */
	{ "DATE" },
	{ "DEL" },
	{ "DIR" },
	{ "DIR A: /W" },
	{ "DIR A: /P" },
	{ "DISKCOMP" },
	{ "DISKCOPY" },
	{ "DSK1." },
	{ "DSK2." },
	{ "DSK3." },
	{ "DSK4." },
/* E */
	{ "ECHO" },
	{ "ERASE" },
/* F */
	{ "FORMAT A: /v /16" },
	{ "FORMAT A: /v /18" },
	{ "FORMAT A: /v /16 /1" },
	{ "FORMAT A: /v /18 /1" },
/* G */
/* H */
	{ "HARD" },
	{ "HDS1." },
	{ "HDS2." },
	{ "" },
/* I */
/* J */
/* K */
/* L */
	{ "LABEL A:" },
/* M */
	{ "MD" },
	{ "MKDIR" },
	{ "MODE 40" },
	{ "MODE 80" },
	{ "MODE PIO:80" },
	{ "MODE PIO:132" },
	{ "MODE RS232/1:" },
	{ "MODE RS232/1:9600,N,8,1" },
	
/* N */
/* O */
/* P */
	{ "PATH" },
	{ "PAUSE" },
	{ "PSYS" },
/* R */
	{ "RD" },
	{ "REMAP" },
	{ "REN" },
	{ "RENAME" },
	{ "RMDIR" },
/* S */
	{ "SCS1." },
	{ "SCS2." },
	{ "SCS3." },
	{ "SCS4." },
	{ "SETDSK" },
/* T */
	{ "TIME" },
	{ "TREE" },
	{ "TREE A: /F" },
	{ "TYPE" },
/* U */
/* V */
	{ "VER" },
	{ "VIDEO" },
	{ "VOL A:" },
/* W */
	
/*END*/
	{ NULL },
};