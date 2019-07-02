// *********************************************************
//  This collection of code is a translation of the
//  'Create New Adventure' function.  It reads an
//  old Dungeon Master  or Prison save file and creates
//  a new Chaos Strikes Back  save file.
// *********************************************************

#include "stdafx.h"

//#include "Objects.h"
#include "Dispatch.h"
#include "UI.h"
#include "CSB.h"
#include "Data.h"

#include <stdio.h>

static void DontDoThis(i32 n)
{
  char msg[1000];
  sprintf(msg, TranslateLanguage("Don't Do This #0x%x\n"
               "The UTILITY functions are designed to create a\n"
               " Chaos Strikes Back game from a Prison SAVEGAME\n"
               " and a 'mini.dat' file.  It will not import nor\n"
               " edit characters nor anything else.  You load a\n"
               " prison savegame, press 'Make New Adventure' twice\n"
               " choose a name for the new adventure, and quit.\n"
               " You can then RESTART at the prison door and\n"
               " select your new game to play.  You will\n"
               " start naked in a dark room filled with worms.\n"
               " If you try to use this program in any other \n"
               " way you will probably be unhappy with the\n"
               " results.  But you need not complain.  I won't listen."), n);
  UI_MessageBox(msg, "Error",MESSAGE_OK);
}


#define CHAOSFILL(a,b) i8 fill##a[a-b];
i32 ReadExtendedFeatures(i32 handle);
ui32 FormChecksum(ui8 *buf, i32 num);
void info(char *, unsigned int);
ui16 countDSAs(void);
void WriteDSAs(i32 handle);
void WriteDSALevelIndex(i32 handle);
const char *TranslateLanguage(const char *text);

//extern bool ForcedScreenDraw;
extern i32 keyboardMode;
extern char ExtendedFeaturesVersion;
extern bool indirectText;
extern bool bigActuators;
extern bool sequencedTimers;
extern bool extendedTimers;
extern bool DefaultDirectXOption;
extern bool SupressDSAWarningOption;
extern ui32 cellflagArraySize;
extern DSAINDEX DSAIndex;

#pragma pack(1)


void _CALL0(CODESTATE s);
void _CALL1(CODESTATE s,i32 a);
void _CALL2(CODESTATE s,i32 a,i32 b);
void _CALL3(CODESTATE s,i32 a,i32 b,uintptr_t);
void _CALL4(CODESTATE s,uintptr_t a,i32 b,uintptr_t c,uintptr_t d);
void _CALL5(CODESTATE s,i32 a,i32 b,i32 c,i32 d,i32 e);
void _CALL6(CODESTATE s,i32 a,i32 b,i32 c,i32 d,i32 e,i32 f);
void _CALL7(CODESTATE s,i32 a,i32 b,i32 c,i32 d,i32 e,i32 f,i32 g);
void _CALL8(CODESTATE s,i32 a,i32 b,i32 c,i32 d,i32 e,i32 f,i32 g,i32 h);
void _CALL9(CODESTATE s,i32 a,i32 b,i32 c,i32 d,i32 e,i32 f,i32 g,i32 h,i32 i);

//class intOrPnt
//{
//public:
//  i32 ivalue;
//  const void *pvalue;
//  intOrPnt(i32 v) {ivalue = v; pvalue = NULL;};
//  intOrPnt(const void *v){ivalue=0; pvalue = v;};
//};


struct DlgButton
{
  i8          byte0;
  i8          byte1;
  wordRectPos rectPos2;
  i8          byte10;
  i8          byte11;
};

struct S12406
{
  i8        byte0;
  i8        byte1;
  DlgButton *pDlgButton2; // An array of DlgButton
  pnt       pnt6;
  pnt       pnt10;
};




struct STRUCT6
{
  i32   int0;
  i16   word4;
};

struct STRUCT12
{
  i32  long0;
  i16  word4;
  i16  word6;
  void (*fnc8)(pnt);
};


#define size3764 880

struct ARRAY_092
{
  ui16   uWord_0;    // total 2
  ui32   uLong_2;    // total 6
};

struct STRUCT070
{
  ui8 uBytes_0[3];
};

struct PORTRAIT // total of 800 bytes
{
  char name_000[8];        // total 008
  char title_008[20];      // total 028
  ui8  uByte_28;           // total 029
  ui8  uByte_29;           // total 030
  ui8  uByte_30;           // total 031
  ui8  uByte_31;           // total 032
  i8   Byte_32;            // total 033
  ui8  uByte_33;           // total 034
  ui8  uByte_34;           // total 035
  ui8  fill_35[5];         // total 040
  ui8  uByte_40;           // total 041
  ui8  uByte_41;           // total 042
  ui8  uByte_42;           // total 043
  ui8  fill_43[1];         // total 044
  ui16 uWord_44;           // total 046
  ui16 uWord_46;           // total 048
  ui16 uWord_48;           // total 050
  ui16 uWord_50;           // total 052
  ui16 uWord_52;           // total 054 
  ui16 health_054;         // total 056
  ui16 uWord_056;          // total 058
  ui16 stamina_058;        // total 060
  ui16 uWord_060;          // total 062
  ui16 mana_062;           // total 064
  ui16 uWord_064;          // total 066
  ui16 uWord_066;          // total 068
  ui16 uWord_068;          // total 070
  STRUCT070 struct070[7];  // total 091 // 3*7 = 21
  ui8  fill_091[1];        // total 092
  ARRAY_092 array_092[20]; // total 212 // each is 6 bytes (20*6 = 120)
  ui16 uWord_212[30];      // total 272 // 2*30 = 60
  ui16 uWord_272;          // total 274
  ui16 uWord_274;          // total 276
  ui8  fill_276[60];       // total 336
  ui8  pixels_336[464];    // total 800  (29 * 16)  = 29*32 pixels
};


struct BLOCK1_FMTx
{
    ui8      ubytes_000[256];    // total 256
    ui8      ubytes_256[32];     // total 288
    ui8      ubytes_288[8];      // total 296
    ui8      ubytes_296[2];      // total 298
    ui8      ubyte_298;          // total 299
    ui8      ubyte_299;          // total 300
    ui8      ubyte_300;          // total 301
    ui8      ubyte_301;          // total 302
    ui8      ubytes_302[8];      // total 310
    ui8      ubytes_310[32];     // total 342
    ui8      ubytes_342[32];     // total 374
    ui16     uword_374;          // total 376
    ui16     uword_376;          // total 378
    ui16     uword_378;          // total 380
    ui8      ubytes_380[134];    // total 512
};



struct BLOCK1_FMT2
{
    ui8      ubytes_000[256];    // total 256
    ui8      ubytes_256[32];     // total 288
    ui8      ubytes_288[8];      // total 296
    ui8      ubytes_296[2];      // total 298
    ui8      ubyte_298;          // total 299
    ui8      ubyte_299;          // total 300
    ui8      ubyte_300;          // total 301
    ui8      ubyte_301;          // total 302
    //ui8      ubytes_302[8];      // total 310
    ui8      ubytes_302[6];      // total 308
    ui32     ulong_308;          // total 312
    //ui8      ubytes_310[32];     // total 342
    ui8      ubytes_312[30];     // total 342
    ui8      ubytes_342[32];     // total 374
    ui16     uword_374;          // total 376
    ui16     uword_376;          // total 378
    ui16     uword_378;          // total 380
    ui8      ubytes_380[134];    // total 512
};


struct BLOCK2
{
  ui8      bytes_000[6];        // total 006
  ui16     word_006;            // total 008
  ui8      bytes_008[2];        // total 010
  ui16     numCharacter_010;    // total 012
  ui16     fill_12;             // total 014
  ui8      bytes_014[14];       // total 028
  ui16     numTimers_028;       // total 030
  ui8      bytes_030[16];       // total 046
  ui16     word_046;            // total 048
  ui8      bytes_048[80];       // total 128
};

struct DUNGEON_DAT_INDEX  // 44 bytes
{
  ui8      bytes_00[2];        // total 2
  ui16     cellArraySize_2;    // total 4
  ui8      numLevel_04;        // total 5
  ui8      bytes_05[1];        // total 6
  ui16     numIndirectText_06; // total 8
  ui8      bytes_08[2];        // total 10
  ui16     numObj_10;          // total 12
  ui16     numDBEnt_12[16];    // total 44
};

struct LEVEL_DESCRIPTOR    // 16 bytes
{
  ui8      bytes_00[8];    // total 8
  ui16     word_08;        // total 10
  ui8      bytes_10[6];    // total 16
};

struct OBJECT_LIST_INDEX  // variable size (numer of objects)
{
  ui16  word_00;
};

struct GAMESTRUCT
{  // total of 4112 bytes;
  ui16               word_0000;              // total 0002
  ui16               word_0002;              // total 0004
  char               filename_0004[32];      // total 0036
  ui16               word_0036;              // total 0038
  //BLOCK1             block1_0038;            // total 0550  512 byte binary block from game file
  union {
    BLOCK1_FMTx block1Fmt1;
    BLOCK1_FMT2 block1Fmt2;
  } block1_0038;
  BLOCK2             block2_0550;            // total 0678
  ui8               *pIndirectTextIndex_0678;// total 0682
  ui8               *pCellArray_682;         // total 0686
  LEVEL_DESCRIPTOR  *pLevelDescriptors_0686; // total 0690
  DUNGEON_DAT_INDEX *pDungeonDatIndex_0690;  // total 0694
  OBJECT_LIST_INDEX *pObjectListIndex_0694;  // total 0698
  ui16              *pWords_0698;            // total 0702
  ui16               numObj_0702;            // total 0704
  ui16              *pWord_0704;             // total 0708
  DBCOMMON          *pDB_0708[16];           // total 0772
  PORTRAIT           portrait_0772[4];       // total 3972
  ui8                bytes_3972[128];        // total 4100
  ui8               *pointer_4100;           // total 4104
  ui8               *pointer_4104;           // total 4108
  ui8               *pointer_4108;           // total 4112
};

struct ARRAY_5202
{
  ui8 *pntr_0;    // total 4  //points to image (464 bytes)
  ui8  ubyte_4;   // total 5
  ui8  fill_5;    // total 6
};

struct CHARACTER_IMAGE
{
  ui8  ubytes[464];
};

struct STRUCT422A
{
  i32   long_0;
  STRUCT422A  *pointer_4;
  STRUCT422A  *pointer_8;
};



struct STRUCT422
{
  pnt     pointer_0;      // Starts as a copy of pAllocDBank434
  i16     Word_4;
  i32     DBankLen_6;
  //i8      Byte_10[8];
  STRUCT422A  *pointer_10;
  i32     bytesRemaining_14;
};

struct HANDLELIST
{
  ui32  memSize_0;   // Size of memory block
  ui8   byte_4; // =0;
  ui8   byte_5; // = memType & 7;
  HANDLELIST *next_6;  // chained to e.Pnt430 //OBJECT_HANDLE objHandle_6;  // Handle of itself
  ui16  maxHandles_10; //ui16  memType_10;
  ui16  numHandles_12;
  ui16  searchStartIndex_14;
  //        data bytes of the object itself
  //    i32  = size of memory block = memSize_0
  //
};

struct OBJHEADER
{
  ui32 length_0;
  ui8 byte_4;
  ui8 byte_5;
  void **self_6;  // My own handle.
  ui16 word_10;
};

struct E
{
  // PALETTE palette24862; (Use palette24862)
  //pnt     Byte24830[16];(use byte24830)
  i16     numCharacter_24814;
  GAMESTRUCT *pGames_24812; //     Pnt24812;
  i16     Word24808;
  i16     Word24806;
  i16     Word24804;
  i16     Word24802;
  i16     Word24800;
  i16     Word24798;
  i16     Word24796;
  i16     Word24794;
  //i8      Byte24792;
  //i8      adjust24792;
  pnt     Pnt24792;
  i32     Long24300;
  //pnt     Pnt24296[100];
  void   **allocatedHandles_24296[100];
  //pnt     Pnt23896[100];
  void   *allocatedObjects_23896[100];
  //i16     Word23496;
  i16     numAllocatedObjects_23496;
  char    Byte23494[1];
  CHAOSFILL(23493,23414)
  //RectPos rectPos23414; (use rectPos23414)
  i16     Word23398;
  i16     Word23396;
  ui8     Byte23394[10608];//bitmap 102 lines by 104 bytes
  //pnt     UtilityDiskMessages; //12786
  //pnt     Pnt12782;  // to
  //pnt     Pnt12678;   // 12618
  //i32     Long12658;   // are
  //i32     Long12654;    // 42 string pointers.
  //                       // (use UtilityDiskMessages[i])
  //pnt     Pnt12446;//use pnt12446;
  //i8      Byte12406;(use s12406)
  // never referenced i8      Byte12392;
  //pnt     Pnt12364;(use pnt12364);
  i8      Byte12336[6]; //actually i32 and i16
  //pnt     Pnt12246;(use pnt12246)
  //pnt     Pnt12242;(use pnt12242)
  //pnt     Pnt12238;(use pnt12238)
  //pnt     Pnt12226;(use pnt12226)
  //pnt     Pnt12222;(use pnt12222)
  i16     Word12196;
  i16     GameFile;//12194;
  i16     Word12184;
  PALETTE palette10938;
  i32     Long9220;
  i16     FirstQueuedMouseEvent;//9216;
  i16     LastQueuedMouseEvent;//9214;
  i8      Byte9212;
  i8      Byte9211;
  i16     Word9210;
  i16     Word9208;
  i16     Word9206;
  i16     Word9204;
  i8      Byte9202[30];// 5 * (i32+i16) ???
  i8      Byte9172[6];//i32 and i16??
  //pnt     Pnt9142;//use pPnt9142
  //i16     Word9138[7];//use word9138
  i16     Word9124;
  i16     Word9122;
  i16     Word9120;
  i16     Word9116;
  i16     Word9110;
  //i16     Word9108xxx;  // Preserved to maintain alignment
  //i16     Word9106xxx;  // Preserved to maintain alignment
  uintptr_t Word9108;
  uintptr_t Word9106;
  //i16     Word9104;
  //i16     Word9102;
  uintptr_t Word9104;
  uintptr_t Word9102;
  i16     Word9098;
  i16     Word8842[1];
  //never referenced i8      Byte8586[1];
  i16     Word8330[1];
  i16     Word8328[1];
  i16     Word8074;
  i16     Word8072;
  i16     Word8070;
  i16     Word8068;
  i16     Word8066;
  i16     align8064[3];  // Add to avoid unaligned references
  char    *Pnt8064;
  char    *Pnt8060;
  i16     Word8056;
  i16     Word8054;
  ui8    *PhysicalBase;//8052;
  i16     Word8048;
  i16     Word7062;
  i16     ActualCharacter; // Word7060
  //i8      CharacterImages_7058[4*464]; // Byte7058
  CHARACTER_IMAGE  characterImages_7058[4]; // Byte7058
  //i8      Byte5202[24];
  ARRAY_5202 array_5202[4];  // Each is 6 bytes  4*6 = 24
  i16     Word5178;
  wordRectPos wRectPos5176;
  i16     Word5168;
  i16     EditBoxSelected; //Word5166 //either NAME or TITLE
  i16     EditBoxPosition; //Word5164
  //wordRectPos wRectPos5162;(use wRectPos5162)
  //wordRectPos wRectPos5154;(use wRectPos5154)
  //wordRectPos wRectPos5146;(use wRectPos5146)
  //wordRectPos wRectPos5138[4];(use wRectPos5138)
  //wordRectPos wRectPos5106[4];(use wRectPos5106)
  //wordRectPos wRectPos5066;(use wRectPos5066)
  //wordREctPos wRectPos5058;(use wREctPos5058)
  //DlgButton dlgButton5050[4];//(use dlgButton5050)
  i8      Byte4930[1];
  CHAOSFILL(4929,4712)
  //i8      Byte4712[16];(use byte4712)
  i16     Word4696;
  i8      Byte4694[464];
  CHAOSFILL(4230,4228)
  i8      Byte4228[464];
  i8      Byte3764[size3764];
  i16     Word3122;
  i16     Word826[5];
  i16     Word816[16];
  i16     Word784[17];
  i16     Word750[8];
  i16     Word734[6];
  i16     Word722[6];
  i16     *Pnt714[7];
  i16     Word686;
  i16     *pw664;
  pnt     Pnt660;
  i16     *pw656;
  i16     *pw652;
  pnt     pAllocDBank434; // The main D-BANK  - free'd  at TAG007f54
  //pnt     Pnt430;
  HANDLELIST *objList430;
  i16     Word426;
  //i16     Word424;
  i16     numHeap_424;   // Always == 1


  // The following 18 bytes are actally a struct
  //pnt     pDBank422;      // Starts as a copy of pAllocDBank434
  //i16     Word418;
  //i32     DBankLen416;
  //i8      Byte412[8];
  STRUCT422 struct422;
};

E e;

unsigned char SelectedColor;


PALETTE palette24862 = {
    0x0000,0x0333,0x0444,0x0310,0x0066,0x0420,0x0040,0x0060,
    0x0700,0x0750,0x0643,0x0770,0x0222,0x0555,0x0007,0x0777};

//i8 byte24830[16] ALIGN4 = {4,6,4,8,16,4,4,4,4,8,4,0,0,0,8,4};
i8 dbEntrySize_24830[16] ALIGN4 = {4,6,4,8,16,4,4,4,4,8,4,0,0,0,8,4};

wordRectPos rectPos23414 = { 0, 193,  0, 101};
wordRectPos rectPos23406 = {62, 255, 48, 149};

static i32 textArraySize; //Number of words
static char *pTextArray;
static DB2 *DB2Address;
static i32 DB2Size;
static i32 compressedTextSize;
char *compressedText = NULL;

const char *positions = "CSBGAME\nCSBGAME2\nCSBGAME3\nCSBGAME4\nCANCEL";

// ARGHH!
const char *filenames[] ={
	"CSBGAME.DAT",
	"CSBGAME2.DAT",
	"CSBGAME3.DAT",
	"CSBGAME4.DAT",
};


const char* UtilityDiskMessages[43] =
{
  "\nPLEASE PUT A\nSAVE GAME DISK IN @",  //0
  "\nPUT PORTRAIT DISK\nIN @",            //1
  "\nPUT A BLANK DISK\nIN @",             //2
  "",                                     //3
  "",                                     //4
  "",                                     //5
  "\n\nREPLACE EXISTING SAVED GAME?",     //6
  "",                                     //7
  "\n\nREPLACE $'s PORTRAIT?",            //8
  "\n\nPLEASE WAIT . . .\nLOADING SAVED GAME",//9
  "\n\nPLEASE WAIT . . .\nSAVING GAME",   //10
  "",                                     //11
  "\n\nCAN'T FIND SAVED GAME",            //12
  "",                                     //13
  "\nSAVED GAME ERROR",                   //14
  "\nERROR WHILE\nSAVING GAME",           //15
  "",                                     //16
  "",                                     //17
  "",                                     //18
  "",                                     //19
  "\nEACH CHAMPION MUST\nHAVE A UNIQUE FIRST NAME!",//20
  "",                                               //21
  "\n\nPLEASE WAIT . .\nLOADING SAVED ADVENTURE",   //22
  "\n\nPLEASE WAIT . .\nSAVING NEW ADVENTURE",      //23
  "CAN'T FIND\nNEW ADVENTURE\nMINI.DAT",  //24
  "\nTHE NEW ADVENTURE\nIS DAMAGED",      //25
  "",                                     //26
  "",                                     //27
  "\nERROR WHILE SAVING\nNEW ADVENTURE",  //28
  "THE NEW ADVENTURE\nCAN ONLY BE MADE AFTER\nA SAVED GAME IS LOADED.",                                     //29
  "\nCAN'T MAKE NEW ADVENTURE\nFROM THIS SAVED GAME.\nALREADY UTILIZED.",//30
  "LOAD WHICH SAVED GAME?",               //31
  "OK",                                   //32
  "OK\nCANCEL",                           //33
  "SAVE\nFORMAT\nCANCEL",                 //34
  "MAKE NEW ADVENTURE\nFORMAT\nCANCEL",   //35
  "YES\nNO",                              //36
  "",                                     //37
  "",                                     //38
  "DUNGEON MASTER\nCHAOS STRIKES BACK\nCANCEL", //39
  "",                                     //40
  "",                                      //41
  "\nPLEASE CHOOSE A\nSAVE GAME SLOT",  //42
};


#ifdef _MSVC_INTEL
#pragma warning (disable:4305)
#pragma warning (disable:4309)
#endif

i8 byte12618[] ALIGN4 = {01,00,00,13,00,00,00,00};

DlgButton dlgButton_12610[] = {
      {0x01,0x00,{0x0050,0x00ed,0x0080,0x0088},0x00,0x01},
      {0x00,0x00,{0x0000,0x0000,0x0000,0x0000},0x00,0x00},
      {0x01,0x00,{0x0050,0x0094,0x0080,0x0088},0x00,0x01},
      {0x02,0x00,{0x00a5,0x00ed,0x0080,0x0088},0x00,0x01},
      {0x00,0x00,{0x0000,0x0000,0x0000,0x0000},0x00,0x00},
      {0x01,0x00,{0x0050,0x0094,0x006c,0x0074},0x00,0x01},
      {0x02,0x00,{0x00a5,0x00ed,0x006c,0x0074},0x00,0x01},
      {0x03,0x00,{0x007b,0x00c4,0x0080,0x0088},0x00,0x01},
      {0x00,0x00,{0x0000,0x0000,0x0000,0x0000},0x00,0x00},
      {0x01,0x00,{0x0050,0x00ed,0x006c,0x0074},0x00,0x01},
      {0x02,0x00,{0x0050,0x0094,0x0080,0x0088},0x00,0x01},
      {0x03,0x00,{0x00a5,0x00ed,0x0080,0x0088},0x00,0x01},
      {0x00,0x00,{0x0000,0x0000,0x0000,0x0000},0x00,0x00},
      {0x01,0x00,{0x0050,0x00ed,0x0058,0x0060},0x00,0x01},
      {0x02,0x00,{0x0050,0x00ed,0x006c,0x0074},0x00,0x01},
      {0x03,0x00,{0x0050,0x00ed,0x0080,0x0088},0x00,0x01},
      {0x00,0x00,{0x0000,0x0000,0x0000,0x0000},0x00,0x00} };


DlgButton four_dlgButon[]={
		{0x01,0x00,{0x0050,0x0094,0x0070,0x0078},0x00,0x01},
		{0x02,0x00,{0x00a5,0x00ed,0x0070,0x0078},0x00,0x01},
		{0x03,0x00,{0x0050,0x0094,0x0080,0x0088},0x00,0x01},
		{0x04,0x00,{0x00a5,0x00ed,0x0080,0x0088},0x00,0x01},
		{0x00,0x00,{0x0000,0x0000,0x0000,0x0000},0x00,0x00}};


#ifdef _MSVC_INTEL
#pragma warning (default:4305)
#pragma warning (default:4309)
#endif

S12406  s12406[] = {
  { 00,00, &dlgButton_12610[0],  byte12618, NULL },
  { 00,00, &dlgButton_12610[2],  NULL,      NULL },
  { 00,00, &dlgButton_12610[5],  NULL,      NULL },
  { 00,00, &dlgButton_12610[9],  NULL,      NULL },
  { 00,00, &dlgButton_12610[13], NULL,      NULL } };


S12406 fourbuttons[]={
  { 00,00, &four_dlgButon[0],	 NULL,      NULL } };



const char *DMGAMEDAT_12246 = "DMGAME.DAT";
const char *DMGAMEBAK_12242 = "DMGAME.BAK";
const char *CSBGAMEDAT_12238 = "CSBGAME.DAT";
const char *CSBGAMEBAK_12234 = "CSBGAME.BAK";
const char *DRIVEB_12222 = "DRIVE B:";
const char *DRIVEA_12226 = "DRIVE A:";

S12406  *pS12406_9166[6] = {NULL,NULL,NULL,NULL,NULL,NULL};//guessed size!
S12406  **ppS12406_9142 = pS12406_9166;

i16  word9138[] = {
          0x4737, //turn right
          0x4838, //forward
          0x4b34, //slide left
          0x4d36, //slide right
          0x5032, //backup
          0x5230, //turn left
          0x532e};//delete

ui32 FrameImage[246] = //byte8046
{
   0xffff0000,0xffff0000,0xffff0000,0xffff0000,
   0xfff00000,0xfff00000,0xbfff3fff,0x80000000,
   0xffffffff,0x00000000,0xfff0fff0,0x00000000,
   0xdfff5fff,0x80000000,0xffffffff,0x00000000,
   0xfff0ffd0,0x00200000,0xefff6fff,0x80000000,
   0xffffffff,0x00000000,0xfff0ff90,0x00600000,
   0xf7ff77ff,0x80000000,0xffffffff,0x00000000,
   0xfff0ff10,0x00e00000,0xfbff7bff,0x80000000,
   0xffffffff,0x00000000,0xfff0fe10,0x01e00000,
   0xfc307c30,0x83ff0030,0x00e400e4,0xffff00e4,
   0x03f00010,0xffe00000,0xfc607c60,0x83ff0060,
   0x01c801c8,0xffff01c8,0x03f00010,0xffe00000,
   0xfc807c80,0x83ff0080,0x03800380,0xffff0380,
   0x07f00410,0xffe00400,0xfc007c00,0x83ff0000,
   0x07000700,0xffff0700,0x0ff00c10,0xffe00c00,
   0xfc007c00,0x83ff0000,0x1e001e00,0xffff1e00,
   0x0bf00810,0xffe00800,0xfc007c00,0x83ff0000,
   0x7c007c00,0xffff7c00,0x13f01010,0xffe01000,
   0xfc007c00,0x83ff0000,0xf800f800,0xfffff800,
   0x23f02010,0xffe02000,0xfc017c01,0x83ff0001,
   0xf000f000,0xfffff000,0x43f04010,0xffe04000,
   0xfc037c03,0x83ff0003,0xe000e000,0xffffe000,
   0x03f00010,0xffe00000,0xfc077c07,0x83ff0007,
   0x00000000,0xffff0000,0x03f00010,0xffe00000,
   0xfc0c7c0c,0x83ff000c,0x00000000,0xffff0000,
   0x03f00010,0xffe00000,0xfc187c18,0x83ff0018,
   0x00000000,0xffff0000,0x03f00010,0xffe00000,
   0xfc207c20,0x83ff0020,0x00000000,0xffff0000,
   0x03f00010,0xffe00000,0xfc407c40,0x83ff0040,
   0x00000000,0xffff0000,0x03f00010,0xffe00000,
   0xfc807c80,0x83ff0080,0x00000000,0xffff0000,
   0x03f00010,0xffe00000,0xfc007c00,0x83ff0000,
   0x00000000,0xffff0000,0x23f02010,0xffe02000,
   0xfc007c00,0x83ff0000,0x00000000,0xffff0000,
   0x43f04010,0xffe04000,0xfc007c00,0x83ff0000,
   0x00010001,0xffff0001,0x83f08010,0xffe08000,
   0xfc007c00,0x83ff0000,0x00030003,0xffff0003,
   0x03f00010,0xffe00000,0xfc007c00,0x83ff0000,
   0x000e000e,0xffff000e,0x03f00010,0xffe00000,
   0xfc007c00,0x83ff0000,0x001c001c,0xffff001c,
   0x03f00010,0xffe00000,0xfc207c20,0x83ff0020,
   0x00380038,0xffff0038,0x03f00010,0xffe00000,
   0xfc407c40,0x83ff0040,0x00780078,0xffff0078,
   0x07f00410,0xffe00400,0xfd807d80,0x83ff0180,
   0x00700070,0xffff0070,0x0ff00c10,0xffe00c00,
   0xff007f00,0x83ff0300,0x00e000e0,0xffff00e0,
   0x3bf03810,0xffe03800,0xfe007e00,0x83ff0200,
   0x00e000e0,0xffff00e0,0x73f07010,0xffe07000,
   0xfc007c00,0x83ff0000,0x81c081c0,0xffff81c0,
   0x43f04010,0xffe04000,0xfc037c03,0x83ff0003,
   0x07800780,0xffff0780,0x83f08010,0xffe08000,
   0xfc0e7c0e,0x83ff000e,0x0f000f00,0xffff0f00,
   0x03f00010,0xffe00000,0xffff7c00,0x83ff0000,
   0xffff0000,0xffff0000,0xfff00210,0xfde00000,
   0xffff7800,0x87ff0000,0xffff0000,0xffff0000,
   0xfff00110,0xfee00000,0xffff7000,0x8fff0000,
   0xffff0000,0xffff0000,0xfff00090,0xff600000,
   0xffff6000,0x9fff0000,0xffff0000,0xffff0000,
   0xfff00050,0xffa00000,0xffff4000,0xbfff0000,
   0xffff0000,0xffff0000,0xfff00030,0xffc00000,
   0xffff7fff,0x80000000,0xffffffff,0x00000000,
   0xfff0fff0,0x00000000 };


wordRectPos wRectPos5162 = {0x009d,0x00fc,0x003c,0x0092};
wordRectPos wRectPos5154 = {0x011e,0x012f,0x002b,0x00a9};
wordRectPos wRectPos5146 = {0x011c,0x0131,0x0029,0x00ab};
wordRectPos wRectPos5138[4] =
{
  {0x000b,0x002a,0x000e,0x002a},
  {0x004e,0x006d,0x000e,0x002a},
  {0x0091,0x00b0,0x000e,0x002a},
  {0x00d3,0x00f2,0x000e,0x002a} };

wordRectPos wRectPos5106[7] =
{
  {0x000b,0x002a,0x000e,0x002a}, //[0]=5106
  {0x004e,0x006d,0x000e,0x002a}, //[1]=5098
  {0x0091,0x00b0,0x000e,0x002a}, //[2]=5090
  {0x00d3,0x00f2,0x000e,0x002a}, //[3]=5082
  {0x004d,0x0078,0x0038,0x0060}, //[4]=5074
  {0x0053,0x0072,0x003e,0x005a}, //[5]=5066
  {0x0000,0x0098,0x0072,0x00af}, //[6]=5058
};

DlgButton dlgButton5050[17] =
{
  {0x01,0,{  4, 48,  3, 42},0,1},//[ 0]=5050
  {0x02,0,{ 71,115,  3, 42},0,1},//[ 1]=5038
  {0x03,0,{138,182,  3, 42},0,1},//[ 2]=5026
  {0x04,0,{205,249,  3, 42},0,1},//[ 3]=5014
  {0x05,0,{157,252, 60,146},0,1},//[ 4]=5002
  {0x06,0,{157,252, 60,146},0,2},//[ 5]=4990
  {0x07,0,{286,303, 43,169},0,1},//[ 6]=4978
  {0x08,0,{156,196,159,167},0,1},//[ 7]=4966
  {0x09,0,{225,253,159,167},0,1},//[ 8]=4954
  {0x64,0,{350,349,250,249},0,0},//[ 9]=4942
  {0x0b,0,{  2, 92,186,194},0,1},//[10]=4930
  {0x0c,0,{102,192,186,194},0,1},//[11]=4918
  {0x0d,0,{202,316,186,194},0,1},//[12]=4906
  {0x0e,0,{ 15, 59, 87, 95},0,1},//[13]=4894
  {0x0f,0,{ 15,131,100,108},0,1},//[14]=4882
  {0x66,0,{350,349,250,249},0,0},//[15]=4870
  {0x11,0,{288,316,  5, 13},0,1},//[16]=4858
};

i8 byte4846[8] ALIGN4 = {0x09,0x00,0x61,0x00,0x01,0x00,0x3b,0x00};

S12406 s12406_4802 =
{
  01,00, &dlgButton5050[0], byte4846, NULL
};


const char* pnt4788[] =
{
  "FIGHTER",
  "NINJA",
  "PRIEST",
  "WIZARD"
};

const char* pnt4772[16] =
{
  "NEOPHYTE   ",
  "NOVICE     ",
  "APPRENTICE ",
  "JOURNEYMAN ",
  "CRAFTSMAN  ",
  "ARTISAN    ",
  "ADEPT      ",
  "EXPERT     ",
  "` MASTER   ",
  "a MASTER   ",
  "b MASTER   ",
  "c MASTER   ",
  "d MASTER   ",
  "e MASTER   ",
  "ARCHMASTER "
};

i8 byte4712[16] ALIGN4 = {0x00,0x0c,0x01,0x02,
                   0x0d,0x0f,0x03,0x05,
                   0x0a,0x08,0x09,0x0b,
                   0x07,0x06,0x04,0x0e };

i8 Byte1278[420] ALIGN4 = {
         0x00,0x06,0x1b,0x0a,0x0f,0x19,0x0c,0x06,0x06,0x0c,0x09,0x04,0x00,0x00,0x00,0x03,
         0x0e,0x02,0x1e,0x1e,0x06,0x1e,0x0e,0x1f,0x0e,0x0e,0x06,0x06,0x03,0x00,0x0c,0x0e,
         0x0e,0x0e,0x1e,0x0f,0x1e,0x1f,0x1f,0x0f,0x11,0x1f,0x01,0x09,0x08,0x11,0x11,0x0e,
         0x1e,0x0e,0x1e,0x0f,0x1f,0x11,0x11,0x11,0x11,0x11,0x1f,0x0f,0x18,0x0f,0x04,0x00,
         0x04,0x1f,0x00,0x04,0x18,0x01,0x00,0x06,0x1b,0x1f,0x14,0x1a,0x1a,0x06,0x0c,0x06,
         0x06,0x04,0x00,0x00,0x00,0x06,0x13,0x06,0x01,0x01,0x0a,0x10,0x10,0x01,0x11,0x11,
         0x06,0x06,0x06,0x0f,0x06,0x13,0x11,0x11,0x11,0x10,0x11,0x10,0x10,0x10,0x11,0x04,
         0x01,0x0a,0x08,0x1b,0x19,0x11,0x11,0x11,0x11,0x10,0x04,0x11,0x11,0x11,0x0a,0x11,
         0x02,0x0c,0x0c,0x03,0x0e,0x00,0x09,0x11,0x1f,0x0a,0x04,0x03,0x00,0x06,0x12,0x0a,
         0x0e,0x04,0x0c,0x0c,0x0c,0x06,0x0f,0x1f,0x00,0x0f,0x00,0x0c,0x15,0x02,0x0e,0x0e,
         0x12,0x1e,0x1e,0x02,0x0e,0x0f,0x00,0x00,0x0c,0x00,0x03,0x06,0x17,0x1f,0x1e,0x10,
         0x11,0x1e,0x1e,0x13,0x1f,0x04,0x01,0x0c,0x08,0x15,0x15,0x11,0x1e,0x11,0x1e,0x0e,
         0x04,0x11,0x11,0x15,0x04,0x0a,0x04,0x0c,0x06,0x03,0x1b,0x00,0x12,0x11,0x00,0x11,
         0x07,0x05,0x00,0x06,0x00,0x1f,0x05,0x0b,0x1d,0x00,0x0c,0x06,0x06,0x04,0x06,0x00,
         0x06,0x18,0x19,0x02,0x10,0x01,0x1f,0x01,0x11,0x04,0x11,0x01,0x06,0x06,0x06,0x0f,
         0x06,0x06,0x16,0x11,0x11,0x10,0x11,0x10,0x10,0x11,0x11,0x04,0x11,0x0a,0x08,0x11,
         0x13,0x11,0x10,0x11,0x14,0x01,0x04,0x11,0x0a,0x1b,0x0a,0x04,0x08,0x0c,0x03,0x03,
         0x00,0x00,0x09,0x11,0x1f,0x0a,0x04,0x09,0x00,0x00,0x00,0x0a,0x1e,0x13,0x1a,0x00,
         0x06,0x0c,0x09,0x04,0x06,0x00,0x06,0x10,0x0e,0x02,0x1f,0x1e,0x02,0x1e,0x0e,0x04,
         0x0e,0x0e,0x06,0x02,0x03,0x00,0x0c,0x00,0x10,0x11,0x1e,0x0f,0x1e,0x1f,0x10,0x0f,
         0x11,0x1f,0x0e,0x09,0x0f,0x11,0x11,0x0e,0x10,0x0e,0x13,0x1e,0x04,0x0f,0x04,0x11,
         0x11,0x04,0x1f,0x0f,0x01,0x0f,0x00,0x00,0x04,0x1f,0x00,0x04,0x18,0x1f,0x00,0x06,
         0x00,0x00,0x04,0x00,0x0d,0x00,0x00,0x00,0x00,0x00,0x0c,0x00,0x00,0x00,0x00,0x00,
         0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x04,0x00,0x00,0x00,0x06,0x0f,0x00,
         0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x03,
         0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x1f,0x00,0x00,
         0x00,0x00,0x00,0x00};

ui32 Long954[4] = {
  0xfff0fff0, 0xfff8fff8, 0xfffcfffc, 0xfffefffe};
ui32 Long938[4] = {
  0x7fff7fff, 0x3fff3fff, 0x1fff1fff, 0x0fff0fff};

void reverseBMP(ui32 *bmp, i32 n)
{
  ASSERT(((uintptr_t)bmp & 3) == 0,"bmp");
  for (i32 i=0; i<n; i++)
    bmp[i]=LE32(bmp[i]);
}

void InitializeE(void)
{
  memset(&e, 0, sizeof (e));
  reverseBMP(FrameImage, 246);
}


void pumper(void); // Pump any queued Windows messages
ui8 *MALLOC(i32 size);
void  MFREE(ui8 *address);
//i32  DIRECT_CONIN(void); // Trap 1 #0x07
//i16  CONSTAT(void);      // Trap 1 #0x0b
void SetSupervisorMode(void);
void ClearSupervisorMode(void);
//void ClearMemory(pnt dest, i16 numByte); // TAG000a84
ui8 *physbase(void);
void SetDLogicalBase(ui8 *);
//i16  OPEN(char *name, i32 ref);
//i16  CREATE(char *name, i16 flag);
i32  READ(i32 file, i32 len, ui8 *buf);
i32  WRITE(i16 file, i32 len, ui8 *buf);
i16  CLOSE(i32 handle);
void StrCpy(char *dst, const char *src);//TAG003264
i16  StrLen(const char *string);//TAG0032ba
void MemMove(ui8 *src, ui8 *dest, i16 byteCount); //TAG0009dc
//void TextToScreen(i32, i32, i32, i32, pnt);//TAG001c42
i16  Unscramble(ui8 *buf, i32 initialChecksum, i32 numwordM1);
i16  GenChecksum(ui8 *buf, i16 initSum, i32 numword);//TAG01d076
//   TAG000c04
static i16  MyWriteScrambled(ui8 *, i16, ui16, ui16 *);//TAG01d0ea



//     TAG0000d6
ui32 Times(ui32, ui32);
void   TAG00022a(i16);
void   TAG0002b0(void);
void   TAG0002c2(void);
void   TAG0002d0(void);
void   TAG00031c(void);
//ui8   *AllocateMemory(i32);//TAG000328
void  *AllocateMemory(i32);//TAG000328
void   ReleaseObject(void *);//TAG0003a0
char  *GetSubString(const char *, char *, i16); //TAG000472
void   PrintDialogText(const char *, i32, i32, i32); //TAG0004ec
void   TAG00054a(void);
void   TAG0006e4(void);
i16    TAG0008dc(i16);
i16    TAG000952(void);
void   TAG0009a8(void);
i16    ReadGameFile(ui8 *, i32);//TAG000aee
i16    WriteGameFile(ui8 *, i16);//TAG000b28
//i16    TAG000b94(pnt,i16,i16);(use GenChecksum)
//     TAG000bc8
static i16 MyUnscrambleStream(ui8 *buf,i32 size,i16 initialHash,i16 Checksum);
i16    ReadAndChecksum(ui8 *buf, i16 *pChecksum, i32 size);//TAG000c52
static i16 MyWriteAndChecksum(ui8 *, ui16 *, i32);//TAG000c98
i16    ReadGameBlock1(ui8 *);//TAG000cde
i16    UnscrambleBlock1(ui8 *, i16);//TAG000d08
i16    WriteFirstBlock(ui8 *, i16);//TAG000d7c
void   TAG00136c(void);
i16    TAG00142e(ui8 *);
//void   TAG001676(ui8 *, i16);
void   TAG001676(GAMESTRUCT *, i16);
void   TAG002724(void);
i16    TestInRectangle(wordRectPos *, i32 x, i32 y);//TAG002c70
void   TAG002ca8(void);
i16    TAG002cdc(DlgButton *, i32, i32, i32);
void   EnqueMouseClick(i32, i32, i32);//TAG002d3c
void   CheckPendingMouseClick(void);//TAG002e2c
void   TAG002e4a(i32);
i16    TAG002faa(S12406 *P1);
//i16    TAG002faa(pnt);
//     TAG003000
S12406 **SetActiveDialog(S12406 **); // Actually an array of (6) &S12406
void   TAG003106(i16);
void   TAG0031a6(i16);
void   TAG00323c(void);
void   TAG00324c(void);
void   TAG003264(void);
void   TAG003294(i16, i16);
void   TAG0032b4(i16, i16);
void   BitBltSquareImage(pnt, const wordRectPos *, i16, i16); //TAG0033b6
//     TAG0033e6
void   FillScreenRectangle(wordRectPos *, i16);
void   DrawNameTitleCarret(i16, i16); //TAG003406
//     TAG0034dc
void   ExpandRectangle(wordRectPos *, wordRectPos *, i16, i16);
//     TAG0035cc
void   DrawDialogBoxOutline(wordRectPos *, i16, i16, i16);
void   PrintCharacterName(i16); //TAG003748
void   DrawCharacterHeader(i16, i16); //TAG0037ac
void   AddDialogButton(DlgButton *, const char*, i16);//TAG003860
void   TAG0039a8(void);
void   FillCharacterNameEditBox(const char *, i16, i16);//TAG0039ea
void   PrintAttributeValue(const char *, i32, i32); //TAG003a7c
void   DrawMagnifiedCharacterImage(void); //TAG0036c0
void   GetPalleteItemRect(i16, wordRectPos *); //TAG0038ea
void   DrawSelectedColor(i16); //TAG003922
void   TAG00439e(void);
//void TAG004a32(char *); //FadeToPalette
//   TAG004bbc
void TextOutB(ui8 *dest,
              i32 destWidth,
              i32 x,
              i32 y,
              i32 color,
              i32 P6,
              const char *text);
void   TextToScreen(i32, i32, i32, i32, const char *);//TAG004e76
i16    TAG004ec0(i32);
void   TAG004ea2(i16 *);
i16    TAG00507e(void);
i16    TAG0050d2(void);
i16    TAG005868(i16 *, i16 *, i16 *, i16 *);
void   TAG00589e(i16, i16);
//void TAG005c50(i32); //Trap #14
void   TAG005c92(pnt);
void   TAG005cd6(pnt *);
void   TAG005cf6(void);
void   TAG005d12(i16 *, i16 *, i16 *);
void   TAG005d8e(i16 *, i16 *, i16 *);
void   TAG005df2(i16);
void   TAG0061ba(i16, i16);
void   TAG0061e2(i16);
void   TAG006202(i16, i16 *, i16 *, i16 *);
void   TAG00623a(i16, char *, char **);
void   TAG00626e(i16, char *, char **);
pnt    FormatInteger(pnt, ui32, i16);//TAG0064b4
//void   TAG006520(pnt,pnt);StrCpy
//i16    TAG00654e(pnt, pnt);strcmp
//i16    TAG0065bc(pnt);//StrLen
i32    atari_sprintf(char *,const char*, const intptr_t, const intptr_t, const intptr_t); //TAG0065e0
i32    GetPixel(pnt, i16, i16, i16);//TAG006700
//void   TAG006892(...) //Use TAG0088b2 from Graphics.cpp
//void   TAG00716a(pnt, RectPos *, i16 color, i16 dstwidth); (FillRectangle)
//ui8   *TAG0073b0(ui8 *);
OBJHEADER *ObjHandle2ObjHeader(void **);
//ui8   *TAG0073d4(ui8 *);
STRUCT422   *LocateContainingHeap(OBJHEADER *);
//void   TAG007416(char *, char *);
void   DechainMemBlock_7416(STRUCT422A *, STRUCT422 *);
//void   TAG00744c(ui8 *, ui8 *);
void   ChainBlock(STRUCT422A *, STRUCT422 *);
//void   TAG0074ae(char *, i32, char *);
//void   TAG0074ae(char *, i32, STRUCT422 *);
void   TAG0074ae(STRUCT422A *, i32, STRUCT422 *);
void   TAG0074ea(ui8 *, i32, ui8 *);
//ui8   *TAG007552(i32, ui8 *, i16 *);
//STRUCT422A *TAG007552(i32, STRUCT422 *, i16 *);
STRUCT422A *BestBlock(i32, STRUCT422 *, i16 *);
i16    TAG007904(char * *, i32);
//ui8   *TAG007a1a(i16);
HANDLELIST  *NewHandleList(i16);
//char  *TAG007ac0(ui8 **);
void  **TAG007ac0(HANDLELIST **);
//void   TAG007b68(ui8 *);
void   RemoveHandle(void **);
//char  *TAG007bba(STRUCT6 *);
STRUCT422  *TAG007bba(STRUCT6 *);
//ui8   *TAG007cc6(i32, i16, char *);
//ui8   *TAG007cc6(i32, i16, STRUCT422 *);
ui8   *BlockAlloc(i32, i16, STRUCT422 *);
//ui8   *TAG007d4c(STRUCT12 *);
void  **TAG007d4c(STRUCT12 *);
//void   TAG007e16(ui8 *, ui8 *);
void   TAG007e16(ui8 *, STRUCT422 *);
i32    TAG007e42(i16, i16);
void   TAG007f54(void);
//ui8   *TAG007f64(i32, i16);
void **TAG007f64(i32, i16);
//i32    TAG007f92(ui8 *);
i32    TAG007f92(void **);
//ui8   *TAG007fe8(ui8 *);
void   *ObjHandle2ObjData(void **);
//void   TAG00801e(ui8 *);
void   DecrementReferenceCount(void **);
//i32 TAG0080e0(ui8 * /*P1*/)
i32 IsHandleInvalid(void **P1);
//     TAG00858c
void   MemoryMove(void *, void *, i16, i16, i32);//like MemMove
//void TAG0086a8(pnt, i32);//ClearMemory
//i32    TAG008788(i16, i32, ui32);
i32    TAG008788(i16, i32, uintptr_t);
void   EditCharacterName(char key);

/*
void StorePnt(ui8 *addr, ui8 *data)
{
  //A little explanation is in order here, perhaps.
  //Some platforms require that 32-bit words be on 4-byte
  //boundaries.  In the runtime we moved all 32-bit words
  //to conform with this requirement.  But here we simply
  //fetch and store them by referencing two 16-bit words.
#ifdef _bigEndian
  xxxxx
#else
  *((ui16 *)(addr)) = (ui16)((ui32)data);
  *((ui16 *)(addr+2)) = (ui16)(((ui32)data)>>16);
#endif
}

void StorePnt(aReg addr, aReg data)
{
  StorePnt((ui8 *)addr, (ui8 *)data);
}

void StorePnt(aReg addr, ui8 *data)
{
  StorePnt((ui8 *)addr, data);
}


ui8 *LoadPnt(ui8 *addr)
{
  //A little explanation is in order here, perhaps.
  //Some platforms require that 32-bit words be on 4-byte
  //boundaries.  In the runtime we moved all 32-bit words
  //to conform with this requirement.  But here we simply
  //fetch and store them by referencing two 16-bit words.
#ifdef _bigEndian
  xxxxx
#else
  return (ui8 *)((*(ui16 *)(addr)) | ((ui32)((*(ui16 *)(addr+2)))<<16));
#endif
}
ui8 *LoadPnt(aReg addr)
{
  return LoadPnt((ui8 *)addr);
}
*/
void StoreLong(ui8 *addr, i32 data)
{
  //A little explanation is in order here, perhaps.
  //Some platforms require that 32-bit words be on 4-byte
  //boundaries.  In the runtime we moved all 32-bit words
  //to conform with this requirement.  But here we simply
  //fetch and store them by referencing two 16-bit words.
#ifdef _bigEndian
  xxxxx
#else
  *((ui16 *)(addr)) = (ui16)((ui32)data);
  *((ui16 *)(addr+2)) = (ui16)(((ui32)data)>>16);
#endif
}

void StoreLong(aReg addr, i32 data)
{
  StoreLong((ui8 *)addr, data);
}

i32 LoadLong(void *addr)
{
  //A little explanation is in order here, perhaps.
  //Some platforms require that 32-bit words be on 4-byte
  //boundaries.  In the runtime we moved all 32-bit words
  //to conform with this requirement.  But here we simply
  //fetch and store them by referencing two 16-bit words.
#ifdef _bigEndian
  xxxxx
#else
  return (i32)((*(ui16 *)((ui8 *)addr)) | ((ui32)((*(ui16 *)((ui8 *)addr+2)))<<16));
#endif
}

i32 LoadLong(aReg addr)
{
  return LoadLong((ui8 *)addr);
}


/*

void StorePnt(ui8 *addr, aReg data)
{
  //A little explanation is in order here, perhaps.
  //Some platforms require that 32-bit words be on 4-byte
  //boundaries.  In the runtime we moved all 32-bit words
  //to conform with this requirement.  But here we simply
  //fetch and store them by referencing two 16-bit words.
#ifdef _bigEndian
  xxxxx
#else
  *((ui16 *)((pnt)addr)) = (ui16)((ui32)data);
  *((ui16 *)((pnt)addr+2)) = (ui16)(((ui32)data)>>16);
#endif
}

void Storepi16(i16 **addr, i16 *data)
{
  //A little explanation is in order here, perhaps.
  //Some platforms require that 32-bit words be on 4-byte
  //boundaries.  In the runtime we moved all 32-bit words
  //to conform with this requirement.  But here we simply
  //fetch and store them by referencing two 16-bit words.
#ifdef _bigEndian
  xxxxx
#else
  *((ui16 *)((pnt)addr)) = (ui16)((ui32)data);
  *((ui16 *)((pnt)addr+2)) = (ui16)(((ui32)data)>>16);
#endif
}

i16 *Loadpi16(i16 **addr)
{
  //A little explanation is in order here, perhaps.
  //Some platforms require that 32-bit words be on 4-byte
  //boundaries.  In the runtime we moved all 32-bit words
  //to conform with this requirement.  But here we simply
  //fetch and store them by referencing two 16-bit words.
#ifdef _bigEndian
  xxxxx
#else
  return (i16 *)((*(ui16 *)((pnt)addr)) | ((ui32)((*(ui16 *)((pnt)addr+2)))<<16));
#endif
}

*/

// *********************************************************
//
// *********************************************************
//   TAG0000d6
ui32 Times(ui32 P1, ui32 P2)
{
  return P1 * P2;
}

// *********************************************************
//
// *********************************************************
void TAG00022a(i16 P1)
{ // Set/unSet VBL interrupt routine?????
//;;;;;;;;;;;;;;;;;;;;;
  switch (P1)
  {
  case 1:
  SetSupervisorMode();
  //TAG0002b0();
  ClearSupervisorMode();
    break;
  case 2:
    SetSupervisorMode();
    //TAG0002c2();
    ClearSupervisorMode();
    break;




  }; //switch(P1)
}


// *********************************************************
//
// *********************************************************
void TAG0002d0(void)
{
  i32 i;
//;;;;;;;;;;;;;
  //e.Word23496 = 0;
  e.numAllocatedObjects_23496 = 0;
  for (i=0; i<100; i++)
  {
    //e.Pnt23896[i] = NULL; //  StorePnt((ui8 *)&e.Pnt23896[i], (ui8 *)NULL);
    e.allocatedObjects_23896[i] = NULL; //  StorePnt((ui8 *)&e.Pnt23896[i], (ui8 *)NULL);
    //e.Pnt24296[i] = NULL; //  StorePnt((ui8 *)&e.Pnt24296[i], (ui8 *)NULL);
    e.allocatedHandles_24296[i] = NULL; //  StorePnt((ui8 *)&e.Pnt24296[i], (ui8 *)NULL);
//
  };
  TAG007e42(100, 50);
}

// *********************************************************
//
// *********************************************************
void TAG00031c(void)
{
  TAG007f54();
}

// *********************************************************
//
// *********************************************************
//  TAG000328
//ui8 *AllocateMemory(i32 P1)
void *AllocateMemory(i32 P1)
{
  dReg D0;
  //ui8 *pnt_8;
  void *pnt_8;
  //ui8 *pnt_4;
  void **pnt_4;
//;;;;;;;;;;;;;;;;;;;
  pnt_4 = TAG007f64(P1, 0x400);
  D0B = (UI8)(pnt_4==NULL?1:0);
  D0W = sw(-D0B);
  if (D0W != 0)
  {
    die(0,"FATAL ERROR: OUT OF MEMORY\nREBOOT");
    for (;;) {};
  };
  pnt_8 = ObjHandle2ObjData(pnt_4);
  //e.Pnt23896[e.Word23496] = (pnt)pnt_8; // StorePnt((ui8 *)&e.Pnt23896[e.Word23496], pnt_8);
  e.allocatedObjects_23896[e.numAllocatedObjects_23496] = pnt_8; 
  //e.Pnt24296[e.Word23496] = (pnt)pnt_4; // StorePnt((ui8 *)&e.Pnt24296[e.Word23496], pnt_4);
  e.allocatedHandles_24296[e.numAllocatedObjects_23496] = pnt_4; 
  //e.Word23496++;
  e.numAllocatedObjects_23496++;
  return pnt_8;
}

void RemoveObjectAndHandle(int index)
{
  int last = e.numAllocatedObjects_23496;
  e.allocatedHandles_24296[index] = e.allocatedHandles_24296[last];
  e.allocatedObjects_23896[index] = e.allocatedObjects_23896[last];
}

// *********************************************************
//
// *********************************************************
//   TAG0003a0
void ReleaseObject(void *P1)
{
  //ui8  **pntA1;
  void ***pntA0;
  i16  w_6;
  //ui8 *pnt_4;
  void **pnt_4;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;
  pnt_4 = NULL;
  //for (w_6 = 0; w_6<e.Word23496; w_6++)
  for (w_6 = 0; w_6<e.numAllocatedObjects_23496; w_6++)
  {
    //pntA0 = (ui8 **)&e.Pnt23896[w_6];

    // Try to simplify this  PRS 20180505
    //pntA0 = (ui8 **)&e.allocatedObjects_23896[w_6];
    //if (P1 != *pntA0) continue;
    if (P1 != e.allocatedObjects_23896[w_6]) continue;
    //pntA0 = (ui8 **)&e.Pnt24296[w_6];


    pntA0 = &e.allocatedHandles_24296[w_6];
//#ifdef impReleaseMemory
    pnt_4 = *pntA0;
//#else
//    NotImplemented(0x3a1);
//#endif
    //e.Word23496--;
    e.numAllocatedObjects_23496--;
    //if (w_6 < e.Word23496)
    if (w_6 < e.numAllocatedObjects_23496)
    {

      /* *****************************************************************
       *  
       *  This code has never been right.  I am going to replace it with
       *  a single-line function call for clarity.
       *  We simply want to remove a singe entry from the two 
       *  lists of objects and handles.
       *
       *  //pntA0 = (ui8 **)&e.Pnt23896[w_6];
       *  pntA0 = (ui8 **)&e.allocatedObjects_23896[w_6];
       *  //pntA1 = (ui8 **)&e.Pnt23896[e.Word23496];
       *  pntA1 = (ui8 **)&e.allocatedObjects_23896[e.numAllocatedObjects_23496];
       *
       *  //This has been wrong since day 1!!!!!
       *  //pntA0 = pntA1;
       *  *pntA0 = *pntA1;
       *
       *
       *  //pntA0 = (ui8 **)&e.Pnt24296[w_6];
       *  pntA0 = (ui8 **)&e.allocatedHandles_24296[w_6];
       *  //pntA1 = (ui8 **)&e.Pnt24296[e.Word23496];
       *  pntA1 = (ui8 **)&e.allocatedHandles_24296[e.numAllocatedObjects_23496];
       *
       *  // This has been wrong since day 1!!!!!
       *  //pntA0 = pntA1;
       *  *pntA0 = *pntA1;
       ************************************************************** */
      RemoveObjectAndHandle(w_6);

    };

    //pntA0 = (ui8 **)&e.Pnt23896[e.Word23496];
    //*pntA0 = NULL;
    e.allocatedObjects_23896[e.numAllocatedObjects_23496] = NULL;
    //pntA0 = (ui8 **)&e.Pnt24296[e.Word23496];
    //*pntA0 = NULL;
    e.allocatedHandles_24296[e.numAllocatedObjects_23496] = NULL;
//
//
  };
  if (pnt_4 == NULL) return;
  DecrementReferenceCount(pnt_4);
  //TAG007f92((ui8 *)pnt_4);
  TAG007f92(pnt_4);
}

// *********************************************************
//
// *********************************************************
char *GetSubString(const char *P1,char *P2,i16 P3)
{
  dReg D6, D7;
  aReg A0, A3, A4;
  const char *pnt_4;
//;;;;;;;;;
  A4 = (aReg)P1;
  A3 = (aReg)P2;
  D7W = P3;
  while ( (D6B = *(A4++)) != 0)
  {
    if (D6B == D7B)
    {
      *A3 = 0;
      return (char *)A4;
    };
    if (D6B == 64)
    {
      //pnt_4 = TranslateLanguage(pnt12222);
      pnt_4 = TranslateLanguage(DRIVEB_12222);
      goto tag0004aa;
    };
    if (D6B != 35) goto tag0004be;
    //pnt_4 = TranslateLanguage(pnt12226);
    pnt_4 = TranslateLanguage(DRIVEA_12226);
tag0004aa:
    A0 = A3++;
    *A0 = *(pnt_4++);
    if(*A0 != 0) goto tag0004aa;
    A3--;
    continue;
tag0004be:
    if (D6B == 36)
    {
      pnt_4 = (char *)e.Pnt24792; // pnt_4 = (char *)LoadPnt((ui8 *)&e.Pnt24792);
      goto tag0004aa;
    };
    *(A3++) = D6B;
//
  };
  A4 = NULL;
  *A3 = 0;
  return (char *)A4;
}



// *********************************************************
//
// *********************************************************
void PrintDialogText(const char *P1,i32 P2,i32 P3,i32 P4)
{
  dReg D0, D1;
  char b_40[40];
//;;;;;;;;;;;;;;;;;;
  while (P1 != NULL)
  {
    P1 = GetSubString(P1, b_40, 10);
    D0W = StrLen(b_40);
    D0W = sw(6 * D0W / 2);
    D1W = sw(P2 - D0W);
    TextToScreen(D1W, P3, P4, 12, b_40);
    P3 += 13;
//
  };
}

// *********************************************************
//
// *********************************************************
void TAG00054a(void)
{
//;;;;;;;;;;;;;;
  if (e.Word23398 == 0) return;
  e.Word23398 = 0;
  TAG0088b2(e.Byte23394, //src
            e.PhysicalBase, // LoadPnt((ui8 *)&e.PhysicalBase), //dest
            (RectPos *)&rectPos23406,   //dest position
            0,
            0,
            104,
            160,
            -1);
  if (e.Word23396 != 0) e.Word23396--;
  TAG003264();
}

// *********************************************************
//
// *********************************************************
//          TAG000588
RESTARTABLE _UtilityDialogBox(const char *P1, const i32 p2, const char *p3, S12406 *P4)
{ //(i16)
  // Generic Diaog Box??
  static dReg D4, D5, D6;
  static DlgButton *sA4;
  static char b_78[50];
  static S12406 *pS12406_28[6];
  static S12406 **ppS12406_4;
  static i32 P2;
  static const char *P3;
  RESTARTMAP
    RESTART(1)
    RESTART(2)
  END_RESTARTMAP
  P2 = p2;
  P3 = p3;
//;;;;;;;;;;;;;;;;;;;;
  ClearMemory((ui8 *)pS12406_28, 24);
  pS12406_28[0] = P4;
  D4W = uw(P2 & 0x8000);
  P2 &= 0x7fff;
  if (e.Word23396 == 0)
  {
    e.Word23396++;
    TAG00324c();
  };
  if (e.Word23398 == 0)
  {
    e.Word23398 = 1;
    TAG0088b2(e.PhysicalBase, //LoadPnt((ui8 *)&e.PhysicalBase), //src
              (ui8 *)e.Byte23394,   //dst
              (RectPos *)&rectPos23414,  //dstPos
              62,             //srcOffsetX
              48,             //srcOffsetY
              160,            //srcByteWidth
              104,            //dstByteWidth
              -1);            //P8
  };
  DrawDialogBoxOutline(&rectPos23406, -2, 15, 12);
  PrintDialogText(P1, 159, 65, P2);
  if (P4 == 0) RETURN_i16(0);
  for (D5L=0,sA4=P4->pDlgButton2;
       (sA4->byte0!=0)&&(P3!=0);
       D5W++, sA4 ++)
  {
    P3 = GetSubString(P3, b_78, 10);
    AddDialogButton(sA4, b_78, 2);
//
//
  };
  ppS12406_4 = SetActiveDialog(pS12406_28);// save old value
  TAG003264();
  do
  {
    TAG00301c(_1_,e.Byte12336);
    D6W = i16Result;
  } while (D6W == 0);
  TAG00324c();
  if (D4W != 0)
  {
    if ( (D6W & 255) <= D5W )
    {
      InvertButton(_2_, &P4->pDlgButton2[D6W-1].rectPos2);
    };
  }
  else
  {
    TAG00054a();
  };
  SetActiveDialog(ppS12406_4);
  RETURN_i16(D6W);
}

void TAG0006e4(void)
{
//;;;;;;;;;;;;;;;;;;;;;;;;;;;
  SetSupervisorMode();
  // SetCriticalErrorHandler();
  ClearSupervisorMode();
  e.Word12196 = 1;
}

// *********************************************************
//
// *********************************************************
i16 TAG000952(void)
{
//;;;;;;;;;;;;


  TAG0008dc(0);
  return 1;
}

// *********************************************************
//
// *********************************************************
void TAG0009a8(void)
{
  DontDoThis(0x9a8);
}

// *********************************************************
//
// *********************************************************
//  TAG000aee
i16 ReadGameFile(ui8 *buf, i32 len)
{
  dReg D0, D7;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  D7L = len;
  if (D7L == 0) return 1;
  D0L = READ(e.GameFile, D7L, buf);
  if (D7L == D0L) return 1;
  return 0;
}

// *********************************************************
//
// *********************************************************
//  TAG000b28
i16 WriteGameFile(ui8 *buf, i16 size)
{
  dReg D0, D7;
//;;;;;;;;;;;;;;;;;;;
  D7L = size;
  if (D7L == 0) return 1;
  D0L = WRITE(e.GameFile, D7L, buf);
  if (D0L == D7L) return 1;
  return 0;
}

// *********************************************************
//
// *********************************************************
i16 TAG000b94(pnt buf, i16 initSum, i16 numword)
{
  return GenChecksum((ui8 *)buf, initSum, numword);
}

// *********************************************************
//
// *********************************************************
//    TAG000bc8
static i16 MyUnscrambleStream(ui8 *buf,
                            i32 size,
                            i16 initialHash,
                            i16 Checksum)
{
  dReg D0;
//;;;;
  D0W = ReadGameFile(buf, size);
  if (D0W == 0) return 0;
  D0W = Unscramble(buf, initialHash, size/2);
  if (D0W != Checksum) return 0;
  return 1;
}


// *********************************************************
//
// *********************************************************
//  TAG01d0ea
i16 MyWriteScrambled(ui8 *buf,
              i16 byteCount,
              ui16 initialHash,
              ui16 *checksum)
{
  dReg D0, D6, D7;
  aReg A3;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  A3 = (aReg)buf;
  D6W = sw(byteCount/2);
  D0W = Unscramble((ui8 *)A3, initialHash, D6W);
  *checksum = D0W;
  D7W = WriteGameFile((ui8 *)A3, byteCount);
  D0W = Unscramble((ui8 *)A3, initialHash, D6W);
  return D7W;
}


// *********************************************************
//
// *********************************************************
//  TAG000c52
i16 ReadAndChecksum(ui8 *buf, i16 *pChecksum, i32 size)
{
  dReg D0, D6;
  //aReg A4;
  //i16 *wA3;
//;;;;;;;
  //A4 = P1;
  //wA3 = P2;
  //D7W = sw(P3);
  D6W = 0;
  D0W = ReadGameFile(buf, size);
  if (D0W == 0) return 0;
  while ((size--) != 0)
  {
    D6W = (UI16)(D6W + ub(*(buf++)));
  };
  *pChecksum = (UI16)(*pChecksum + D6W);
  return 1;
}

// *********************************************************
//
// *********************************************************
//  TAG000c98
i16 MyWriteAndChecksum(ui8 * P1, ui16 *P2, i32 P3)
{
  dReg D0, D6, D7;
  aReg A4;
  ui16 *uwA3;
//;;;;;;;;;;;;;;;;;;;;;;;;
  A4 = (aReg)P1;
  uwA3 = P2;
  D7W = sw(P3);
  D6W = 0;
  D0W = WriteGameFile((ui8 *)A4, D7W);
  if (D0W == 0) return 0;
  while (D7W != 0)
  {
    D6W = (UI16)(D6W + ub(*(A4++)));

    D7W--;
  };
  *uwA3 = (UI16)(*uwA3 + D6W);
  return 1;
}

// *********************************************************
//
// *********************************************************
//  TAG000cde
i16 ReadGameBlock1(ui8*P1)
{
  dReg D0;
  aReg A4;
//;;;;;;;;;;;;;;;;;;;;;;
  A4 = (aReg)P1;
  D0W = ReadGameFile((ui8 *)A4, 512);
  if (D0W == 0) return 0;
  return 1;
}

// *********************************************************
//
// *********************************************************
//  TAG000d08
i16 UnscrambleBlock1(BLOCK1_FMT2 *P1,i16 P2)
{
  dReg D5, D6, D7;
  //pnt A0;
  i16 *pData;
//;;;;;;;;;;;;;;;;;;;;;;;;;;
  //A4 = (aReg)P1;
  pData = (i16 *)P1;
  D7L = 32;
  D6L = 0;
  do
  {
    //D6W = (UI16)(D6W + LE16(wordGear(A4)));
    D6W = (UI16)(D6W + LE16(*(pData++)));
    //D6W ^= LE16(wordGear(A4+2));
    D6W ^= LE16(*(pData++));
    //D6W = (UI16)(D6W - LE16(wordGear(A4+4)));
    D6W = (UI16)(D6W - LE16(*(pData++)));
    //D6W = (UI16)(D6W ^ LE16(wordGear(A4+6)));
    D6W = (UI16)(D6W ^ LE16(*(pData++)));
    //A4 += 8;
    D7W--;
  } while (D7W != 0);
  //Unscramble((ui8 *)A4, LE16(wordGear(P1 + 2*P2)), 128);
  Unscramble((ui8 *)pData, LE16(((ui16 *)P1)[P2]), 128); // 128 words
  D5W = 0;
  D7W = 128;
  do
  { // 128 words
    //A0 = A4;
    //A4 += 2;
    //pData++;
    D5W = (UI16)(D5W + LE16(*(pData++)));
    D7W--;
  } while (D7W != 0);
  if (D5W == D6W) return 1;
  return 0;
}

// *********************************************************
//
// *********************************************************
//  TAG000d7c
i16 WriteFirstBlock(ui8 *buf, i16 offset)
{
  dReg D5, D6, D7;
  aReg A0, A4;
//;;;;;;;;;;;;;;;;;
  D5W = 0;
  D7W = 128;
  A4 = (aReg)buf+256;
  do
  {
    D5W = (I16)(D5W + LE16(wordGear(A4))); // Second half-block checksum
    A4 += 2;
    D7W--;
  } while (D7W != 0);
  D7W = 32;
  D6W = 0;
  A4 = (aReg)buf;
  for (;;)
  { // 32 times 4 random 16-bit words = 256 bytes.
    // Except we skip the last word in the block.
    wordGear(A4) = 0; //(i16)STRandom();
    D6W = (UI16)(D6W + LE16(wordGear(A4)));
    A4 += 2;
    wordGear(A4) = 0; //(i16)STRandom();
    D6W ^= LE16(wordGear(A4));
    A4 += 2;
    wordGear(A4) = 0; //(i16)STRandom();
    D6W = (UI16)(D6W - LE16(wordGear(A4)));
    A4 += 2;
    D7W--;
    if (D7W == 0) break;
    wordGear(A4) = 0; //(i16)STRandom();
    D6W ^= LE16(wordGear(A4));
    A4 += 2;
  } ;
  wordGear(A4) = LE16((ui16)(D6W ^ D5W));
  A4 += 2;
  A0 = (aReg)buf + 2*offset;
  Unscramble((ui8 *)A4, LE16(wordGear(A0)), 128);
  D7W = WriteGameFile(buf, 512);
  A0 = (aReg)buf + 2*offset;
  Unscramble((ui8 *)A4, LE16(wordGear(A0)), 128);
  return D7W;

}

// *********************************************************
//
// *********************************************************
i16 TAG0008dc(i16)
{
  return 1;
}

// *********************************************************
//
// *********************************************************
//  TAG000e64
RESTARTABLE _CheckChampionNames(void)
{//(i16)
  static dReg D0, D6, D7;
  //static aReg A0, A4;
  static PORTRAIT *pPortrait;
  //static aReg A0;
  static PORTRAIT *pPortrait2;
  static GAMESTRUCT pGame;
  RESTARTMAP
    RESTART(1)
  END_RESTARTMAP
//;;;;;;;;;;;;;;;;;;;;;;
  //for (D7L=0; D7W<e.Word24814; D7W++)
  for (D7L=0; D7W<e.numCharacter_24814; D7W++)
  {
    //A4 = e.Pnt24812 + 772 + 800*D7W; // A4 = (aReg)LoadPnt((ui8 *)&e.Pnt24812) + 800*D7W + 772;
    pPortrait = &e.pGames_24812[0].portrait_0772[D7W];
    if (pPortrait->name_000[0] == 0) // if (A4[0] == 0)
    {
tag000e84:
      UtilityDialogBox(_1_,
                       TranslateLanguage(UtilityDiskMessages[20]),8,
                       TranslateLanguage(UtilityDiskMessages[32]),&s12406[0]);
      RETURN_i16(0);
    };
    //for (D6W=sw(D7W+1); D6W<e.Word24814; D6W++)
    for (D6W=sw(D7W+1); D6W<e.numCharacter_24814; D6W++)
    {
      // A0 = e.Pnt24812 + 800*D6W + 772; //A0 = (aReg)LoadPnt((ui8 *)&e.Pnt24812) + 800*D6W +772;
      pPortrait2 = &e.pGames_24812[0].portrait_0772[D6W]; //A0 = (aReg)LoadPnt((ui8 *)&e.Pnt24812) + 800*D6W +772;
      D0W = sw(strcmp((char *)pPortrait, (char *)pPortrait2));
      if (D0W == 0) goto tag000e84;
//
//
    };
//
  };
  RETURN_i16(1);
}

// *********************************************************
//
// *********************************************************
RESTARTABLE _TAG000ede(const i32 P1, char *P2)
{//(i16)
  static dReg D0;
  static aReg A0;
  static i16 w_2;
  RESTARTMAP
    RESTART(1)
    RESTART(2)
    RESTART(3)
    RESTART(4)
  END_RESTARTMAP
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  do
  {
    for (;;)
    {
      if (P1 == 0)
      {
        UtilityDialogBox(_1_,
                  TranslateLanguage(UtilityDiskMessages[2]),
                  (unsigned)0x800f,
                  TranslateLanguage(UtilityDiskMessages[35]),
                  &s12406[3]);
        D0W = i16Result;
        w_2 = D0W;
      }
      else
      {
        if (P1 == 1)
        {
          A0 = (aReg)UtilityDiskMessages[0];
        }
        else
        {
          A0 = (aReg)UtilityDiskMessages[1];
        };
        UtilityDialogBox(_2_,
                  A0,
                  (unsigned)0x800f,
                  UtilityDiskMessages[34],
                  &s12406[2]);
        D0W = i16Result;
        w_2 = D0W;
      };
      if (w_2 == 3)
      {
        RETURN_i16(0);
      };
      D0W = TAG0008dc(1);

      if (w_2 != 2) break;
      TAG0009a8();
    } ;
	// ask to which game we want to write
	// inserted by LK
    UtilityDialogBox(_3_,
			TranslateLanguage(UtilityDiskMessages[42]),
			(unsigned)0x800f,
			positions,
			&fourbuttons[0]);

    strcpy(P2, filenames[i16Result - 1]);

	// end of insertion
    D0W = OPEN(P2, "rb");
    if (D0W < 0) break;
    if (P1 <= 1)
    {
      A0 = (aReg)TranslateLanguage(UtilityDiskMessages[6]);
    }
    else
    {
      A0 = (aReg)TranslateLanguage(UtilityDiskMessages[8]);
    };

    UtilityDialogBox(_4_,
              A0,
              (unsigned)0x8008,
              TranslateLanguage(UtilityDiskMessages[36]),
              &s12406[1]);
    D0W = i16Result;
  } while (D0W != 1);
  RETURN_i16(1);
}

/*
// *********************************************
static ui32 WCSRCSchecksum;

static ui32 chaosWCS(void)
{
  ui32 temp;
  temp = WCSRCSchecksum;
  WCSRCSchecksum = 0xffff;
  return temp;
}
*/
/*
static ui32 RCS(i32 handle)
{
  ui32 temp;
  WCSRCShandle = handle;
  temp = WCSRCSchecksum;
  WCSRCSchecksum = 0xffff;
  return temp;
}
*/
/*
void chaosWCS(ui8 *buf, i32 n)
{
  i32 i;
  for (i=0; i<n; i++)
  {
    WCSRCSchecksum *= (ui32)0xbb40e62d;
    WCSRCSchecksum += 11 + buf[i];
  };
  WriteGameFile((char *)buf, (ui16)n);
}
*/
/*

static void RCS(ui8 *buf, i32 n)
{
  i32 i;
  READ(WCSRCShandle, n, (char *)buf);
  for (i=0; i<n; i++)
  {
    WCSRCSchecksum *= (ui32)0xbb40e62d;
    WCSRCSchecksum += 11 + buf[i];
  };
}

static void WCS(ui32 *i, i32 n)
{
  WCS((ui8 *)i, 4*n);
}

static void RCS(ui32 *i, i32 n)
{
  RCS((ui8 *)i, 4*n);
}
*/
/*
void chaosWCS(i32 *i, i32 n)
{
  chaosWCS((ui8 *)i, 4*n);
}
/*
static void RCS(i32 *i, i32 n)
{
  RCS((ui8 *)i, 4*n);
}

static void WCS(ui16 *i, i32 n)
{
  WCS((ui8 *)i, 2*n);
}

static void RCS(ui16 *i, i32 n)
{
  RCS((ui8 *)i, 2*n);
}

static void WCS(char *c, i32 n)
{
  WCS((ui8 *)c, n);
}

static void RCS(char *c, i32 n)
{
  RCS((ui8 *)c, n);
}
*/

struct B100STRUCT 
{
  ui8 *pointer;
  ui16 data;
};


// *********************************************************
//
// *********************************************************
RESTARTABLE _TAG000fc4(const i32 P1)
{//(void)
  static dReg D0, D1, D3, D7;
  //static ui8 *pD0, *pD1, *pD2;
  static ui8 *pD0, *pD2;
  B100STRUCT *pD1;
  //static aReg A0, A1, A4;
  static aReg A0, A1;
  static GAMESTRUCT *pGame;
  static ui16  *uwA3;
  static ui8 *pnt_116;
  static ui8 *pnt_112;
  static ui8 *pnt_108;
  static ui8 *pnt_104;
  //static i8   b_100[96];   Change to array of structs 20180410 PRS
  // b_100[0] address
  // b_100[4] = word
  // b_100[6] = address
  // b_100[10] word
  // b_100[12] = address
  // b_100[16] word
  // b_100[18] = address
  // b_100[22] word
  // b_100[24] = address
  // b_100[28 ] word
  // b_100[30] = address
  // b_100[34 ] word
  // b_100[36] = address
  // b_100[40 ] word
  static B100STRUCT struct_100[16];
  static ui16 uw_4;
  static ui16 uw_2;
  RESTARTMAP
    RESTART(1)
    RESTART(2)
    RESTART(3)
    RESTART(4)
  END_RESTARTMAP
//;;;This is only function that calls 'MyWriteScrambled'
//;;; and this function is called from two places
  //A4 = e.Pnt24812 + 4112*P1; // A4 = (aReg)LoadPnt((ui8 *)&e.Pnt24812) + 4112*P1;
  pGame = &e.pGames_24812[P1];
  //if (e.Pnt24812 == NULL) RETURN; // if (LoadPnt((ui8 *)&e.Pnt24812) == NULL) RETURN;
  if (e.pGames_24812 == NULL) RETURN; // if (LoadPnt((ui8 *)&e.Pnt24812) == NULL) RETURN;
  //if (e.Word24814 == 0) RETURN;
  if (e.numCharacter_24814 == 0) RETURN;
  CheckChampionNames(_1_);
  D0W = i16Result;
  if (D0W == 0) RETURN;
  //TAG000ede(_2_,1-P1, A4+4);
  TAG000ede(_2_,1-P1, pGame->filename_0004);
  D0W = i16Result;
  if (D0W != 0)
  {
    if (P1 != 0)
    {
      A0 = (aReg)UtilityDiskMessages[23];
    }
    else
    {
      A0 = (aReg)TranslateLanguage(UtilityDiskMessages[10]);
    };
    UtilityDialogBox(_3_,A0, 15, 0, 0);
    //e.GameFile = CREATE((char *)A4+4, "wb", true);
    e.GameFile = CREATE(pGame->filename_0004, "wb", true);
    if (e.GameFile < 0) goto tag001328;
    memset(struct_100,0,sizeof(struct_100));//ClearMemory((ui8 *)b_100, 96);
    //struct_100[0].pointer = (ui8 *)A4+550; // StorePnt(b_100, A4+550);
    struct_100[0].pointer = (ui8 *)&pGame->block2_0550; // StorePnt(b_100, A4+550);
    struct_100[0].data = 128; // wordGear(b_100+4) = 128;
    //struct_100[1].pointer = (ui8 *)A4+4100; // StorePnt(b_100+6, LoadPnt(A4+4100));
    struct_100[1].pointer = pGame->pointer_4100; // StorePnt(b_100+6, LoadPnt(A4+4100));
    // wordGear(b_100+10) = sw(16 * LE16(wordGear(A4+596)));
    struct_100[1].data = sw(16*LE16(pGame->block2_0550.word_046)); 
    //struct_100[2].pointer = (ui8 *)A4+772; // StorePnt(b_100+12, A4+772);
    struct_100[2].pointer = (ui8 *)pGame->portrait_0772; // StorePnt(b_100+12, A4+772);
    struct_100[2].data = 3328; //  wordGear(b_100+16) = 3328;
    struct_100[3].pointer = pGame->pointer_4104; //  StorePnt(b_100+18, LoadPnt(A4+4104));
    {
      int timerSize;
      //timerSize = sequencedTimers?sizeof(TIMER):10;
      timerSize = sequencedTimers?12:10;
      timerSize = extendedTimers?16:timerSize;
      //wordGear(b_100+22) = sw(timerSize * LE16(wordGear(A4+578)));
      //struct_100[3].data = sw(timerSize*LE16(wordGear(A4+578))); 
      struct_100[3].data = sw(timerSize*LE16(pGame->block2_0550.numTimers_028)); 
    };
    //  StoreLong(b_100+24, LoadLong(A4+4108));
    struct_100[4].pointer = pGame->pointer_4108;
    // wordGear(b_100+28) = sw(2 * LE16(wordGear(A4+578)));
    //struct_100[4].data = sw(2*LE16(wordGear((ui8 *)A4+578))); 
    struct_100[4].data = sw(2*LE16(pGame->block2_0550.numTimers_028)); 
    //pnt_104 = (ui8 *)A4 + 38;
    pnt_104 = (ui8 *)&pGame->block1_0038;
    //if (wordGear(A4+2) == 2)
    if (pGame->word_0002 == 2)
    {
      pnt_108 = pnt_104 + 344;
      pnt_112 = pnt_104 + 312;
      pnt_104[300] = 2;
    }
    else
    {
#ifdef impfc4
      //pnt_108 = (ui8 *)A4 + 380;
      pnt_108 = pGame->block1_0038.ubytes_342;
      //pnt_112 = (ui8 *)A4 + 348;
      pnt_112 = (ui8 *)pGame->block1_0038.ubytes_310;
      //A4[336] = 2;
      pGame->block1_0038.ubyte_298 = 2;
#else
      NotImplemented(0xfc4);
#endif
    };
    for (D7W=0; D7W<16; D7W++)
    {
      pnt_116 = pnt_112 + 2*D7W;
      D0W = 0; //(i16)STRandom();
      wordGear(pnt_116) = D0W;
//
    };
    for (D7L=0; D7W<5; D7W++)
    {
      pD0 = pnt_108 + 2*D7W;
      pD1 = &struct_100[D7W]; // pD1 = (ui8 *)b_100 + 6*D7W;
      pD2 = pnt_112 + 2*D7W;
      //A0 = b_100 + 6*D7W;
      D3W = pD1->data; // D3W = wordGear(A0+4); //Random number generated above
      pnt_116 = pD0;
      D3W /= 2;
      //D0W = GenChecksum(LoadPnt(pD1), //buffer
      //                   LE16(wordGear(pD2)),//initial hash
      //                   D3W);        //# words
      D0W = GenChecksum(pD1->pointer, //buffer
                        LE16(wordGear(pD2)),//initial hash
                        D3W);        //# words
      wordGear(pnt_116) = LE16(D0W);
//
//
    };
    if (ExtendedFeaturesVersion != '@')
    {
      EXTENDEDFEATURESBLOCK efb;
      memset(&efb,0,512);
      strcpy(efb.sentinel, " Extended Features ");
      efb.dataMapLength = dataMapLength;
      efb.version = ExtendedFeaturesVersion;
      efb.dataIndexMapChecksum = FormChecksum((ui8 *)dataIndexMap,2*dataMapLength);
      efb.dataTypeMapChecksum = FormChecksum(dataTypeMap,dataMapLength);
      efb.numDSA = countDSAs();
      efb.flags |= EXTENDEDFEATURESBLOCK::BigActuators;
      if (indirectText)
      {
        efb.flags |= EXTENDEDFEATURESBLOCK::IndirectText;
      };
      if (sequencedTimers)
      {
        efb.extendedFlags |= EXTENDEDFEATURESBLOCK::SequencedTimers;
      };
      if (extendedTimers)
      {
        efb.extendedFlags |= EXTENDEDFEATURESBLOCK::ExtendedTimers;
      };
      if (DefaultDirectXOption)
      {
        efb.extendedFlags |= EXTENDEDFEATURESBLOCK::DefaultDirectX;
      };
      if (SupressDSAWarningOption)
      {
        efb.extendedFlags |= EXTENDEDFEATURESBLOCK::SupressDSAWarning;
      };
      if (efb.numDSA != 0)
      {
        efb.flags |= EXTENDEDFEATURESBLOCK::LevelDSAInfoPresent;
      };
      efb.extendedFeaturesChecksum = 0;
      efb.extendedFeaturesChecksum = FormChecksum((ui8 *)&efb,512);
      WriteGameFile((ui8 *)&efb, 512);
      WriteGameFile(dataTypeMap, (ui16)dataMapLength);
      WriteGameFile((ui8 *)dataIndexMap, (ui16)(2*dataMapLength));
      if (efb.numDSA != 0)
      {
        WriteDSAs(e.GameFile);
        WriteDSALevelIndex(e.GameFile);
//{
        //i32 i;
        //ui32 checksum;
        //WCS(e.GameFile); //Discard old checksum and start new.
        //for (i=0; i<256; i++)
        //{
        //  if (DSAIndex.GetDSA(i) == NULL) continue;
        //  WCS(&i,1);
        //  DSAIndex.GetDSA(i)->Write();
        //};
        //checksum = WCS(0);
        //WriteGameFile((char *)&checksum, 4);
//}

      };
    };
    //D0W = WriteFirstBlock((ui8 *)A4+38, wordGear(A4+36));
    D0W = WriteFirstBlock((ui8 *)&pGame->block1_0038, pGame->word_0036);
    if (D0W == 0) goto tag001328;
    for (D7W=0; D7W<5; D7W++)
    {
      //D0W = MyWriteScrambled(LoadPnt(b_100 + 6*D7W),
      //                      wordGear(b_100 + 6*D7W + 4),
      //                      LE16(wordGear(pnt_112+2*D7W)),
      //                      &uw_2);
      D0W = MyWriteScrambled(struct_100[D7W].pointer,
                            wordGear(&struct_100[D7W].data),
                            LE16(wordGear(pnt_112+2*D7W)),
                            &uw_2);
      if (D0W == 0) goto tag001328;
      A0 = (aReg)pnt_108 + 2*D7W;
      if (uw_2 == LE16(wordGear(A0))) continue;
      //pD0 = pnt_108 + 2*D7W;
      //pD1 = pnt_112 + 2*D7W;
      //atari_sprintf(e.Byte23494,
      //              "SAVEGAME():CHECKSUM %d != %d (KEY = %d)\n",
      //              LE16(wordGear(pD0)),
      //              uw_2,
      //              LE16(wordGear(pD1)) );
      atari_sprintf(e.Byte23494,
                    "SAVEGAME():CHECKSUM %d != %d (KEY = %d)\n",
                    LE16(wordGear(pnt_108+2*D7W)),
                    uw_2,
                    LE16(wordGear(pnt_112+2*D7W)) );
      UI_MessageBox(e.Byte23494, NULL, MESSAGE_OK);
//
//

    };
    uwA3 = &uw_4;
    uw_4 = 0;
    //D0W = MyWriteAndChecksum(LoadPnt(A4+690), uwA3, 44);
    D0W = MyWriteAndChecksum((ui8 *)pGame->pDungeonDatIndex_0690, uwA3, 44);
    if (D0W == 0) goto tag001328;

    //D0W = sw(16 * (UI8)(LoadPnt(A4+690)[4]));
    D0W = sw(16 * (UI8)pGame->pDungeonDatIndex_0690->numLevel_04);
    //D0W = MyWriteAndChecksum(LoadPnt(A4+686),uwA3, D0W); // levelDescriptors
    D0W = MyWriteAndChecksum((ui8 *)pGame->pLevelDescriptors_0686, uwA3, D0W); // levelDescriptors
    if (D0W == 0) goto tag001328;

    //D0W = MyWriteAndChecksum(LoadPnt(A4+694), uwA3, 2*wordGear(A4+702)); //objectListIndex
    D0W = MyWriteAndChecksum((ui8 *)pGame->pObjectListIndex_0694, uwA3, 2*pGame->numObj_0702); //objectListIndex
    if (D0W == 0) goto tag001328;

    //D0W = sw(2 * LE16(wordGear(LoadPnt(A4+690)+10)));
    D0W = sw(2 * LE16(pGame->pDungeonDatIndex_0690->numObj_10));
    //D0W = MyWriteAndChecksum(LoadPnt(A4+704), uwA3, D0W);  // objectList
    D0W = MyWriteAndChecksum((ui8 *)pGame->pWord_0704, uwA3, D0W);  // objectList
    if (D0W == 0) goto tag001328;

    //D0W = sw(2 * LE16(wordGear(LoadPnt(A4+690)+6)));
    D0W = sw(2 * LE16(pGame->pDungeonDatIndex_0690->numIndirectText_06));
    if (indirectText) D0W *= 2;
    D0W = MyWriteAndChecksum((ui8 *)pGame->pIndirectTextIndex_0678, uwA3, D0W);  // indirectTextIndex
    if (D0W == 0) goto tag001328;
    if (indirectText)
    {
      D0W = MyWriteAndChecksum((ui8 *)&compressedTextSize, uwA3, 4);
      if (D0W == 0) goto tag001328;
      D0W = MyWriteAndChecksum((ui8 *)compressedText, uwA3, 2*compressedTextSize);
      if (D0W == 0) goto tag001328;
    };

    {
      int dbNum;
      for (dbNum=0; dbNum<16; dbNum++)
      {
        ui16 entrySize;
        //pD0 = (ui8 *)A4 + 4*dbNum;
        //A0 = &byte24830[dbNum];
        //entrySize = *A0;
        entrySize = dbEntrySize_24830[dbNum];
        if ( (dbNum == dbSCROLL) && (ExtendedFeaturesVersion >= 'B'))
        {
          entrySize = 6;
        };
        if ( (dbNum == dbACTUATOR) && bigActuators)
        {
          entrySize = 10;
        };
        if (dbNum == dbEXPOOL)
        {
          entrySize = (ui16)sizeof(DB11);
        };
        //A1 = (aReg)LoadPnt(A4+690) + 2*dbNum;
        //D1W = sw(entrySize * LE16(wordGear(A1+12))); //# entries
        D1W = sw(entrySize * LE16(pGame->pDungeonDatIndex_0690->numDBEnt_12[dbNum])); //# entries*size
        D0W = MyWriteAndChecksum((ui8 *)pGame->pDB_0708[dbNum], uwA3, D1W);
        if (D0W == 0) goto tag001328;
//
//
      };
    };
    //D0W = MyWriteAndChecksum(LoadPnt(A4+682),
    //                        uwA3,
    //                        LE16(wordGear(LoadPnt(A4+690)+2)));
    D0W = MyWriteAndChecksum(pGame->pCellArray_682,
                            uwA3,
                            LE16(pGame->pDungeonDatIndex_0690->cellArraySize_2));
    if (D0W == 0) goto tag001328;
    *uwA3 = LE16(*uwA3);
    WriteGameFile((ui8 *)uwA3, 2);
    if (D0W == 0)
    {
tag001328:
      if (P1 != 0)
      {
        A0 = (aReg)UtilityDiskMessages[28];
      }
      else
      {
        A0 = (aReg)TranslateLanguage(UtilityDiskMessages[15]);
      };
      UtilityDialogBox(_4_,A0, 8, TranslateLanguage(UtilityDiskMessages[32]), &s12406[0]);
    };
    CLOSE(e.GameFile);
    e.GameFile=-1;
  };
  TAG000952();
  TAG00054a();
  RETURN;
}

// *********************************************************
//
// *********************************************************
void TAG00136c(void)
{
  dReg D0;
  //aReg A0;
  //BLOCK1 *pBlock1_A0;
  BLOCK1_FMT2 *pBlock1F2_A0;
  //pnt pnt_8;
  //BLOCK1 *pBlock1_8;
  BLOCK1_FMT2 *pBlock1F2_8;
  //char *pnt_4;
  GAMESTRUCT *pGame_4;
  GAMESTRUCT *pGame_A0;
//;;;;;;;;;;;;;;;;;;;;;;;;;
//  pnt_4 = (char *)LoadPnt((ui8 *)&e.Pnt24812);
  pGame_4 = e.pGames_24812;
  e.Word24808 = (unsigned)0x91a7;
  //A0 = (aReg)pnt_4 + 38;
  //pBlock1_A0 = &pGame_4->block1_0038;
  pBlock1F2_A0 = &pGame_4->block1_0038.block1Fmt2;
  //pnt_8 = A0;
  //pBlock1_8 = pBlock1_A0;
  pBlock1F2_8 = pBlock1F2_A0;
  //if (wordGear(pnt_4+2) == 2)
  if (pGame_4->word_0002 == 2)
  {
    //A0 = pnt_8;
    //pBlock1_A0 = pBlock1_8;
    pBlock1F2_A0 = pBlock1F2_8;
    //e.Word24806 = wordGear(A0+378);
    //e.Word24806 = pBlock1_A0->uword_378;
    e.Word24806 = pBlock1F2_A0->uword_378;
    //D0W = (UI8)(pnt_8[301]);
    //D0W = (UI8)pBlock1_8->ubyte_301;
    D0W = (UI8)pBlock1F2_8->ubyte_301;
    if (D0W == 1)
    {
      e.Word24804 = 1;
      e.Word24806 = 10;
    }
    else
    {
      //A0 = pnt_8;
      //pBlock1_A0 = pBlock1_8;
      pBlock1F2_A0 = pBlock1F2_8;
      //e.Word24804 = wordGear(A0+376);
      //e.Word24806 = wordGear(A0+378);
      //e.Word24804 = pBlock1_A0->uword_376;
      //e.Word24806 = pBlock1_A0->uword_378;
      e.Word24804 = pBlock1F2_A0->uword_376;
      e.Word24806 = pBlock1F2_A0->uword_378;
    };
  }
  else
  {
    //A0 = (aReg)pnt_4;
    pGame_A0 = pGame_4;
    //e.Word24806 = wordGear(A0+414);
    //e.Word24806 = pGame_A0->block1_0038.uword_376;
    e.Word24806 = pGame_A0->block1_0038.block1Fmt1.uword_376;
    //if (A0[337] == 1)
    //if (pGame_A0->block1_0038.ubyte_299 == 1)
    if (pGame_A0->block1_0038.block1Fmt1.ubyte_299 == 1)
    {
      e.Word24804 = 1;
      e.Word24806 = 10;
    }
    else
    {
      //A0 = (aReg)pnt_4;
      pGame_A0 = pGame_4;
      //e.Word24804 = wordGear(A0+412);
      //e.Word24806 = wordGear(A0+414);
      //e.Word24804 = pGame_A0->block1_0038.uword_374;
      //e.Word24806 = pGame_A0->block1_0038.uword_376;
      e.Word24804 = pGame_A0->block1_0038.block1Fmt1.uword_374;
      e.Word24806 = pGame_A0->block1_0038.block1Fmt1.uword_376;
    };
  };
  e.Word24802 = 1;
  e.Word24800 = 1;
  e.Word24798 = 0;
  e.Word24796 = 0;
  e.Word24794 = 0;
  // never referenced e.Byte24792 = 0;
}




// *********************************************************
//
// *********************************************************
i16 TAG00142e(GAMESTRUCT *P1)
{
  dReg D0, D1, D4, D5, D6, D7;
  //aReg A0, A4;
  //aReg A0;
  DUNGEON_DAT_INDEX *pDungeonDatIndex_A0;
  GAMESTRUCT *pGame_A4;
  //pnt pnt_8;
  i16 w_4;
  i16 w_2;
//;;;;;;;;;;;;;;;;;;;;
  //A4 = P1;
  pGame_A4 = P1;
  w_4 = 0;
  //A0 = (aReg)AllocateMemory(44);
  pDungeonDatIndex_A0 = (DUNGEON_DAT_INDEX *)AllocateMemory(sizeof(*pDungeonDatIndex_A0));
  //StorePnt(A4+690, A0);
  //A4->pDungeonDatIndex_0690 =  A0;
  pGame_A4->pDungeonDatIndex_0690 =  pDungeonDatIndex_A0;
  //D0W = ReadAndChecksum(LoadPnt(A4+690), &w_4, 44);   // d.dungeonDatIndex
  //D0W = ReadAndChecksum(A4->pDungeonDatIndex_0690, &w_4, 44);   // d.dungeonDatIndex
  D0W = ReadAndChecksum((ui8 *)pGame_A4->pDungeonDatIndex_0690, 
                        &w_4, 
                        sizeof(*pGame_A4->pDungeonDatIndex_0690));   // d.dungeonDatIndex
  if (D0W == 0) return 0;
  //D7W = (UI8)(*(LoadPnt(A4+690)+4));//Number of levels
  //D7W = (UI8)(*(A4->pDungeonDatIndex_0690+4));//Number of levels
  D7W = (UI8)(pGame_A4->pDungeonDatIndex_0690->numLevel_04);//Number of levels
  //StorePnt(A4+686, AllocateMemory(16*D7W)); //For level descriptors
  pGame_A4->pLevelDescriptors_0686 = (LEVEL_DESCRIPTOR *)
                       AllocateMemory(D7W * sizeof(*pGame_A4->pLevelDescriptors_0686)); //For level descriptors
  //D0W = ReadAndChecksum(LoadPnt(A4+686), &w_4, 16*D7W); //Level descriptors
  D0W = ReadAndChecksum((ui8 *)pGame_A4->pLevelDescriptors_0686, 
                        &w_4, 
                        D7W * sizeof(*pGame_A4->pLevelDescriptors_0686)); //Level descriptors
  if (D0W == 0) return 0;
  //StorePnt(A4+698, AllocateMemory(2*D7W));
  pGame_A4->pWords_0698 = (ui16 *)AllocateMemory(D7W*sizeof(*pGame_A4->pWords_0698));
  for (D4L=D5L=0; D5W<D7W; D5W++)
  { //For each level
    ui16 *pWord_A0;
    LEVEL_DESCRIPTOR *pLevelDescriptor_A0;
    //A0 = (aReg)LoadPnt(A4+698)+2*D5W;
    pWord_A0 = &pGame_A4->pWords_0698[D5W];
    //wordGear(A0) = LE16(D4W);
    *pWord_A0 = LE16(D4W);
    //A0 = (aReg)LoadPnt(A4+686) + 16*D5W;
    pLevelDescriptor_A0 = &pGame_A4->pLevelDescriptors_0686[D5W];
    //D0W = sw(  (LE16 (wordGear(A0+8))  & 0x07c0)  /64 + 1);
    D0W = sw((LE16(pLevelDescriptor_A0->word_08) & 0x07c0)/64 + 1);
    D4W = sw(D4W + D0W);
//
//
  };
  //wordGear(A4+702) = D4W;
  pGame_A4->numObj_0702 = D4W;
  //A0 = (aReg)LoadPnt(A4+690);
  pDungeonDatIndex_A0 = pGame_A4->pDungeonDatIndex_0690;
  //D5W = LE16(wordGear(A0+10));
  D5W = LE16(pDungeonDatIndex_A0->numObj_10);

  //StorePnt(A4+694, AllocateMemory(2*D4W));  // d.objectListIndex
  pGame_A4->pObjectListIndex_0694 = (OBJECT_LIST_INDEX *)
                AllocateMemory(D4W*sizeof(*pGame_A4->pObjectListIndex_0694));  // d.objectListIndex
  //A0 = (aReg)LoadPnt(A4+690);
  pDungeonDatIndex_A0 = pGame_A4->pDungeonDatIndex_0690;
  //D0W = LE16(wordGear(A0+10));
  D0W = LE16(pDungeonDatIndex_A0->numObj_10);

  //StorePnt(A4+704, AllocateMemory(2*D0W));  // d.objectList
  pGame_A4->pWord_0704 = (ui16 *)AllocateMemory(D0W * sizeof(*pGame_A4->pWord_0704));  // d.objectList
  //A0 = (aReg)LoadPnt(A4+690);
  pDungeonDatIndex_A0 = pGame_A4->pDungeonDatIndex_0690;
  //D0W = LE16(wordGear(A0+6));
  D0W = LE16(pDungeonDatIndex_A0->numIndirectText_06);
  if (indirectText) D0W = (i16)(2 * D0W);    // one word per indirect text entry
  //StorePnt(A4+678, AllocateMemory(2*D0UW)); // d.indirectTextArray
  pGame_A4->pIndirectTextIndex_0678 = (ui8 *)AllocateMemory(2*D0UW); // d.indirectTextArray


  //D0W = ReadAndChecksum(LoadPnt(A4+694), &w_4, 2*D4W); // d.objectListIndex
  D0W = ReadAndChecksum((ui8 *)pGame_A4->pObjectListIndex_0694,
                        &w_4,
                        D4W * sizeof(*pGame_A4->pObjectListIndex_0694)); // d.objectListIndex
  if (D0W == 0) return 0;
  //D0W = ReadAndChecksum(LoadPnt(A4+704), &w_4, 2*D5W); // d.objectList
  D0W = ReadAndChecksum((ui8 *)pGame_A4->pWord_0704, 
                        &w_4, D5W * sizeof(*pGame_A4->pWord_0704)); // d.objectList
  if (D0W == 0) return 0;

  //A0 = (aReg)LoadPnt(A4+690);
  pDungeonDatIndex_A0 = pGame_A4->pDungeonDatIndex_0690;
  //D0W = LE16(wordGear(A0+6));  // numberOfEntriesInTextArray
  D0W = LE16(pDungeonDatIndex_A0->numIndirectText_06);  // numberOfEntriesInTextArray
  if (indirectText) D0W = (i16)(2*D0W);
  //D0W = ReadAndChecksum(LoadPnt(A4+678), &w_4, 2*D0W);  // d.indirectTextIndex
  D0W = ReadAndChecksum(pGame_A4->pIndirectTextIndex_0678,
                        &w_4, 2*D0W);  // d.indirectTextIndex
  textArraySize = D0W; //Number of words
  //textArray = (char *)LoadPnt(A4+678);
  pTextArray = (char *)pGame_A4->pIndirectTextIndex_0678;
  if (D0W == 0) return 0;
  if (indirectText)
  {
  //If the indirectText flag is set then read
  // 1) a 32-bit word giving the size of text
  // 2) the text itself;
    D0W = ReadAndChecksum((ui8 *)&compressedTextSize, &w_4, 4);  // d.sizeOfCompressedText
    if (D0W == 0) return 0;
    compressedText = (char *)malloc(2*compressedTextSize);
    D0W = ReadAndChecksum((ui8 *)compressedText, &w_4, 2*compressedTextSize);  // d.compressedText
    if (D0W == 0) return 0;
  };
  for (D6W=0; D6W<16; D6W++)
  {
    ui16 *pWord_A0;
    //A0 = (aReg)LoadPnt(A4+690) + 2*D6W;
    pWord_A0 = pGame_A4->pDungeonDatIndex_0690->numDBEnt_12;
    //D5W = LE16(wordGear(A0+12));
    D5W = LE16(pWord_A0[D6W]);   // number of entries
    //A0 = byte24830 + D6W; // DB sizes?
    //D4W = (UI8)(*A0);
    //A0 = byte24830 + D6W; // DB sizes?
    D4W = dbEntrySize_24830[D6W];  // DB entry size
    if ( (D6W==dbSCROLL) && (ExtendedFeaturesVersion>='B') )
    {
      D4W = 6;
    };
    if ( (D6W == dbACTUATOR) && bigActuators)
    {
      D4W = 10;
    };
    if (D6W==dbEXPOOL)
    {
      D4W = sizeof(DB11);
    };
    //pnt_8 = A4 + 4*D6W;
    //A0 = (aReg)LoadPnt(A4+690) + 2*D6W;
    pWord_A0 = &pGame_A4->pDungeonDatIndex_0690->numDBEnt_12[D6W];
    //D1L = D4UW * LE16(wordGear(A0+12));
    D1L = D4UW * LE16(*pWord_A0);
    if (D6W == 2)
    {
      //DB2Size = LE16(wordGear(A0+12));
      DB2Size = LE16(*pWord_A0);
    };
    //A0 = pnt_8;
    //StorePnt(A0+708, AllocateMemory(D1L));
    pGame_A4->pDB_0708[D6W] = (DBCOMMON *)AllocateMemory(D1L);
    if (D6W == 2)
    {
      //DB2Address = (DB2 *)LoadPnt(A0+708);
      DB2Address = (DB2 *)pGame_A4->pDB_0708[2];
    };
    //D0W = ReadAndChecksum(LoadPnt(A4+4*D6W+708), &w_4, D4UW*D5UW);
    D0W = ReadAndChecksum((ui8 *)pGame_A4->pDB_0708[D6W], &w_4, D4UW*D5UW);
    if (D0W == 0) return 0;
//
    //if (D6W == 2) ReadExtendedFeatures();
  };
  //A0 = (aReg)LoadPnt(A4+690);
  pDungeonDatIndex_A0 = pGame_A4->pDungeonDatIndex_0690;
  {
    ui32 cellArraySize;
    //cellArraySize = LE16(wordGear(A0+2));
    cellArraySize = LE16(pDungeonDatIndex_A0->cellArraySize_2);
    if (cellflagArraySize != 0) cellArraySize = cellflagArraySize;
    //StorePnt(A4+682, AllocateMemory(cellArraySize));
    pGame_A4->pCellArray_682 = (ui8 *)AllocateMemory(cellArraySize);
    //D0W = ReadAndChecksum(LoadPnt(A4+682), &w_4, cellArraySize);
    D0W = ReadAndChecksum((ui8 *)pGame_A4->pCellArray_682, &w_4, cellArraySize);
    if (D0W ==0) return 0;
  };
  D0W = ReadGameFile((ui8 *)&w_2, 2);//Final checksum
  if (D0W == 0) return 1;
  w_2 = LE16(w_2);
  if (w_2 == w_4) return 1;
  return 0;
}

// *********************************************************
//
// *********************************************************
//void TAG001676(ui8 *P1, i16 P2)
void TAG001676(GAMESTRUCT *P1, i16 P2)
{
  dReg D7;
  //aReg A0, A4;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  //A4 = (aReg)P1;
  GAMESTRUCT *pGame_A4;
  pGame_A4 = P1;

  //if (LoadLong(A4+4100) != 0)
  //{
  //  ReleaseMemory(LoadPnt(A4+4100));
  //};
  if (pGame_A4->pointer_4100 != NULL) ReleaseObject(pGame_A4->pointer_4100);
  //StoreLong(A4+4100, 0);
  pGame_A4->pointer_4100 = NULL;


  //if (LoadLong(A4+4104) != 0)
  //{
  //  ReleaseMemory(LoadPnt(A4+4104));
  //};
  //StorePnt(A4+4104, (ui8 *)NULL);
  if (pGame_A4->pointer_4104 != NULL) ReleaseObject(pGame_A4->pointer_4104);
  pGame_A4->pointer_4104 = NULL;



  //if (LoadPnt(A4+4108) != NULL)
  //{
  //  ReleaseMemory(LoadPnt(A4+4108));
  //};
  //StorePnt(A4+4108, (ui8 *)NULL);
  if (pGame_A4->pointer_4108 != NULL) ReleaseObject(pGame_A4->pointer_4108);
  pGame_A4->pointer_4108 = NULL;


  //if (LoadPnt(A4+678) != NULL) ReleaseMemory(LoadPnt(A4+678));
  //StorePnt(A4+678, (ui8 *)NULL);
  if (pGame_A4->pIndirectTextIndex_0678 != NULL) ReleaseObject(pGame_A4->pIndirectTextIndex_0678);
  pGame_A4->pIndirectTextIndex_0678 = NULL;



  //if (LoadPnt(A4+682) != NULL) ReleaseMemory(LoadPnt(A4+682));
  //StorePnt(A4+682, (ui8 *)NULL);
  if (pGame_A4->pCellArray_682 != NULL) ReleaseObject(pGame_A4->pCellArray_682);
  pGame_A4->pCellArray_682 = NULL;



  //if (LoadPnt(A4+686) != NULL) ReleaseMemory(LoadPnt(A4+686));
  //StorePnt(A4+686, (ui8 *)NULL);
  if (pGame_A4->pLevelDescriptors_0686 != NULL) ReleaseObject(pGame_A4->pLevelDescriptors_0686);
  pGame_A4->pLevelDescriptors_0686 = NULL;



  //if (LoadPnt(A4+690) != NULL) ReleaseMemory(LoadPnt(A4+690));
  //StorePnt(A4+690, (ui8 *)NULL);
  if (pGame_A4->pDungeonDatIndex_0690 != NULL) ReleaseObject(pGame_A4->pDungeonDatIndex_0690);
  pGame_A4->pDungeonDatIndex_0690 = NULL;



  //if (LoadPnt(A4+694) != NULL) ReleaseMemory(LoadPnt(A4+694));
  //StorePnt(A4+694, (ui8 *)NULL);
  if (pGame_A4->pObjectListIndex_0694 != NULL) ReleaseObject(pGame_A4->pObjectListIndex_0694);
  pGame_A4->pObjectListIndex_0694 = NULL;



  //if (LoadPnt(A4+698) != NULL) ReleaseMemory(LoadPnt(A4+698));
  //StorePnt(A4+698, (ui8 *)NULL);
  if (pGame_A4->pWords_0698 != NULL) ReleaseObject(pGame_A4->pWords_0698);
  pGame_A4->pWords_0698 = NULL;



  //if (LoadPnt(A4+704) != NULL) ReleaseMemory(LoadPnt(A4+704));
  //StorePnt(A4+704, (ui8 *)NULL);
  if (pGame_A4->pWord_0704 != NULL) ReleaseObject(pGame_A4->pWord_0704);
  pGame_A4->pWord_0704 = NULL;



  for (D7W=0; D7W<16; D7W++)
  {
    DBCOMMON *pDB_A0;
    //A0 = A4 + 708 + 4*D7W;
    pDB_A0 = pGame_A4->pDB_0708[D7W];
    //if (LoadPnt(A0) != NULL) ReleaseMemory(LoadPnt(A0));
    //StorePnt(A0, (ui8 *)NULL);
    if (pDB_A0 != NULL) ReleaseObject(pDB_A0);
    pGame_A4->pDB_0708[D7W] = NULL;
//
//
  };
  //if (P2 == 0) e.Word24814 = 0;
  if (P2 == 0) e.numCharacter_24814 = 0;
  //wordGear(A4) = 0;
  pGame_A4->word_0000 = 0;
}


// *********************************************************
//
// *********************************************************
RESTARTABLE _TAG00179c(const i32 P1)
{//(i16)
  static dReg D0, D5, D6, D7;
  static aReg A0;
  RESTARTMAP
    RESTART(1)
    RESTART(2)
    RESTART(3)
  END_RESTARTMAP
//;;;;;;;;;;;;;;
  D7W = uw(P1);// (0x8001 for example)
  if (D7W == 2)
  {
    DontDoThis(0x17ae);


    D6W = 1;
  }
  else
  {
    D5L = 1;
    while (D7W & 0x8000)
    {
      D7W &= 0x7fff;
      if (D5W == 0)
      {
        UtilityDialogBox(_1_,
                         UtilityDiskMessages[27],
                         (unsigned)0x8008,
                         TranslateLanguage(UtilityDiskMessages[32]),
                         &s12406[0]);
      };
      D5L = 0;
      if (D7W == 0)
      {
        UtilityDialogBox(_2_,
                  UtilityDiskMessages[0],
                  (unsigned)0x800f,
                  TranslateLanguage(UtilityDiskMessages[32]),
                  &s12406[0]);
        D6W = i16Result;
      }
      else
      {
        if (D7W == 1)
        {
          A0 = (aReg)TranslateLanguage(UtilityDiskMessages[42]);
        }
        else
        {
          A0 = (aReg)TranslateLanguage(UtilityDiskMessages[1]);
        };
       UtilityDialogBox(_3_,
                    	  A0,
		                    (unsigned)0x800f,
                        positions,
                        &fourbuttons[0]);
        D6W = i16Result;
        // we dont need this coz there is 4 buttons with four saves, no cancel
		//if (D6W == 2) RETURN_i16(0);

      };
//
    };
    D0W = TAG0008dc(1);
  };
  RETURN_i16(D6W);
}

// *********************************************************
//
// *********************************************************
//  TAG001868
RESTARTABLE _ReadSavedGame(const i32 P1)
{//(i16)
  //P1 non-zero means read MINI.DAT
  static dReg D0, D7;
  //static aReg A0, A1, A4;
  //static aReg A0, A1;
  static aReg A1;
  static GAMESTRUCT *pGame_A0;
  static GAMESTRUCT *pLocalGame;
  static ui8 *pnt_4654;
  static const char* pnt_4650;
  static ui8 *pnt_4646;
  static ui8 *pnt_4642;
  static i8  b_4638[512];
  static ui8 *pnt_4126;
  static i16 w_4122;
  static i16 w_4120;
  //static i8  b_4118[4112];
  static GAMESTRUCT game_4118;
  static i16 w_6;
  static i16 w_4;
  static i16 w_2;
  static char *savegame;

  RESTARTMAP
    RESTART(1)
    RESTART(2)
    RESTART(3)
    RESTART(4)
    RESTART(5)
    RESTART(6)
  END_RESTARTMAP
//;;;;;;;;;;;;;;;;;;Reads a gamefile????
  D7W = sw(P1);
  //A4 = b_4118;
  pLocalGame = &game_4118;
  //ClearMemory((ui8 *)A4, 4112);
  ClearMemory((ui8 *)pLocalGame, sizeof(*pLocalGame));
  //pnt_4126 = (ui8 *)A4 + 38;
  pnt_4126 = (ui8 *)&pLocalGame->block1_0038;
  w_6 = 1;
  do
  {
    if (D7W != 0)
    {//We are supposed to read MINI.DAT
      TAG000952();
      //pnt_4650 = pnt12238;//Where game will be written
      pnt_4650 = CSBGAMEDAT_12238;//Where game will be written
      e.GameFile = OPEN("mini.dat", "rb");
      if (e.GameFile < 0)
      {
        UtilityDialogBox(_1_,TranslateLanguage(UtilityDiskMessages[24]), 8, 
                         TranslateLanguage(UtilityDiskMessages[32]), s12406);
        RETURN_i16(-1);
      };
      w_2 = 1;
    }
    else
    {
      TAG00179c(_2_, w_6 | 0x8000);
      D0W = i16Result;
   	  savegame = (char*)filenames[D0W - 1];

      if (D0W == 0)
      {
        if (e.Word12184 == 0)
        {
          D0L = -1;
        }
        else
        {
          D0L = -99;
        };
        w_2 = D0W;
        goto tag001d9a;
      };
      w_2 = 1;
      w_4122 = 1;
      w_4120 = 1;
      //pnt_4650 = pnt12246;
      pnt_4650 = DMGAMEDAT_12246;
      //e.GameFile = OPEN(pnt12246, "rb"); //"DMGAME.DAT"
      e.GameFile = OPEN(DMGAMEDAT_12246, "rb"); //"DMGAME.DAT"
      if (e.GameFile < 0)
      {
        //pnt_4650 = pnt12242;
        pnt_4650 = DMGAMEBAK_12242;
        e.GameFile = OPEN(DMGAMEBAK_12242, "rb");//DMGAME.BAK
        if (e.GameFile < 0) w_4120 = 0;
      };
      if (e.GameFile >= 0) CLOSE(e.GameFile);
      //pnt_4654 = pnt12238;
	    pnt_4654 = (ui8 *)savegame;
      e.GameFile = OPEN(savegame, "rb");
      if (e.GameFile < 0)
      {
        //pnt_4654 = pnt12234;
        //e.GameFile = OPEN(pnt12234,"rb");
        if (e.GameFile < 0) w_4122 = 0;
      };
      if (e.GameFile >= 0) CLOSE(e.GameFile);
      if (w_4120 + w_4122 < 1)
      {
        UtilityDialogBox(_3_,
                  TranslateLanguage(UtilityDiskMessages[12]),
                  (unsigned)0x8008,
                  TranslateLanguage(UtilityDiskMessages[32]),
                  &s12406[0]);
        w_6 |= 0x8000;
        w_2 = 0;
      }
      else
      {
        if (w_4120 + w_4122 > 1)
        {
          UtilityDialogBox(_4_,
                    TranslateLanguage(UtilityDiskMessages[31]),
                    (unsigned)0x800f,
                    TranslateLanguage(UtilityDiskMessages[39]),
                    &s12406[4]);
          w_4 = i16Result;
          if (w_4 == 1) goto tag001a4c;
          if (w_4 == 2)
          {
            pnt_4650 = (char *)pnt_4654;
            goto tag001a4c;
          };
          if (e.Word12184 == 0)
          {
            D0L = 1;
          }
          else
          {
            D0L = 99;
          };
          w_2 = D0W;
          goto tag001d9a;
        };
        if (w_4122 != 0)
        {
          pnt_4650 = (char *)pnt_4654;
        };
tag001a4c:
        e.GameFile = OPEN(pnt_4650, "rb");
        if (e.GameFile < 0) w_2 = 0;
      };
    };
  } while (w_2 != 1);
  //StrCpy((char *)A4+4,pnt_4650);
  StrCpy(pLocalGame->filename_0004,pnt_4650);
  {
    const char *A0;
    if (D7W != 0)
    {
      //A0 = (aReg)UtilityDiskMessages[22];
      A0 = UtilityDiskMessages[22];
    }
    else
    {
      //A0 = (aReg)UtilityDiskMessages[9];
      A0 = TranslateLanguage(UtilityDiskMessages[9]);
    };
    UtilityDialogBox(_5_, A0, 15, NULL, NULL);
  };
  //wordGear(A4+36) = 29;
  pLocalGame->word_0036 = 29;
  //wordGear(A4+2)  = 2;
  pLocalGame->word_0002  = 2;
  ReadExtendedFeatures(e.GameFile);
  //D0W = ReadGameBlock1((ui8 *)A4+38);
  D0W = ReadGameBlock1((ui8 *)&pLocalGame->block1_0038);
  if (D0W == 0) goto tag001da8;
  //MemoryMove((ui8 *)A4+38, (ui8 *)b_4638, 0x400, 0x400, 512);
  MemoryMove(&pLocalGame->block1_0038, (ui8 *)b_4638, 0x400, 0x400, 512);
  // We just saved the scrambled version at b_4638
  //D0W = UnscrambleBlock1((ui8 *)A4+38, wordGear(A4+36));
  //D0W = UnscrambleBlock1(&pLocalGame->block1_0038, pLocalGame->word_0036);
  D0W = UnscrambleBlock1(&pLocalGame->block1_0038.block1Fmt2, pLocalGame->word_0036);
  if (D0W == 0)
  { // It didn't work with that key!
    //wordGear(A4+36) = 10;
    pLocalGame->word_0036 = 10;
    //wordGear(A4+2) = 1;
    pLocalGame->word_0002 = 1;
    //MemoryMove((ui8 *)b_4638, (ui8 *)A4+38, 0x400, 0x400, 512);
    MemoryMove((ui8 *)b_4638, &pLocalGame->block1_0038, 0x400, 0x400, 512);
    // We have replaced buffer with original, scrambled version
    //D0W = UnscrambleBlock1((ui8 *)A4+38, wordGear(A4+36));
    //D0W = UnscrambleBlock1(&pLocalGame->block1_0038, pLocalGame->word_0036);
    D0W = UnscrambleBlock1(&pLocalGame->block1_0038.block1Fmt2, pLocalGame->word_0036);
    // Now unscramble with 10 as initial hash.
    if (D0W == 0) goto tag001da8;
  };
//#ifdef alternateFormat
  //if (wordGear(A4+2) == 2)
  if (pLocalGame->word_0002 == 2)
  {
    pnt_4642 = pnt_4126 + 344;
    pnt_4646 = pnt_4126 + 312;
  }
  else
  {
#ifdef impReadSavedGame
    //pnt_4642 = (ui8 *)A4 + 380;
    pnt_4642 = pLocalGame->block1_0038.ubytes_342;
    //pnt_4646 = (ui8 *)A4 + 348;
    pnt_4646 = pLocalGame->block1_0038.ubytes_310;
#else
    NotImplemented(0x1868);
#endif
  };
  //D0W = MyUnscrambleStream((ui8 *)A4+550,  //block 2
  //                         128,
  //                         LE16(wordGear(pnt_4646)),
  //                         LE16(wordGear(pnt_4642))  );
  D0W = MyUnscrambleStream((ui8 *)&pLocalGame->block2_0550,  //block 2
                           128,
                           LE16(wordGear(pnt_4646)),
                           LE16(wordGear(pnt_4642))  );
  if (D0W == 0) goto tag001da8;
//#else
//  NotImplemented(0xc9a1);
//#endif
  //StorePnt(A4+4100, AllocateMemory(16*LE16(wordGear(A4+596))));
  pLocalGame->pointer_4100 = (ui8 *)AllocateMemory(16*LE16(pLocalGame->block2_0550.word_046));
  {
    int timerSize;
    //timerSize = sequencedTimers?sizeof(TIMER):10;
    timerSize = sequencedTimers?12:10;
    timerSize = extendedTimers?16:timerSize;
    //StorePnt(A4+4104, AllocateMemory(timerSize*LE16(wordGear(A4+578))));
    pLocalGame->pointer_4104 = (ui8 *)AllocateMemory(timerSize*LE16(pLocalGame->block2_0550.numTimers_028));
  };
  //StorePnt(A4+4108, AllocateMemory(2*LE16(wordGear(A4+578))));
  pLocalGame->pointer_4108 = (ui8 *)AllocateMemory(2*LE16(pLocalGame->block2_0550.numTimers_028));
  //D0W = MyUnscrambleStream(LoadPnt(A4+4100), //Item16
  //                        16*LE16(wordGear(A4+596)),
  //                        LE16(wordGear(pnt_4646+2)),
  //                        LE16(wordGear(pnt_4642+2)) );
  D0W = MyUnscrambleStream(pLocalGame->pointer_4100, //Item16
                          16*LE16(pLocalGame->block2_0550.word_046),
                          LE16(wordGear(pnt_4646+2)),
                          LE16(wordGear(pnt_4642+2)) );
  if (D0W == 0) goto tag001da8;
  // Next is the character data.
  //D0W = MyUnscrambleStream((ui8 *)A4+772, //characters
  //                        3328,
  //                        LE16(wordGear(pnt_4646+4)),
  //                        LE16(wordGear(pnt_4642+4)) );
  D0W = MyUnscrambleStream((ui8 *)pLocalGame->portrait_0772, //characters portraits
                          3328,
                          LE16(wordGear(pnt_4646+4)),
                          LE16(wordGear(pnt_4642+4)) );
  if (D0W == 0) goto tag001da8;
  //for (w_2=0; w_2<LE16(wordGear(A4+560)); w_2++)
  for (w_2=0; w_2<LE16(pLocalGame->block2_0550.numCharacter_010); w_2++)
  { // Clear out trailing garbage in name and title.
    for (w_4=0; w_4<8; w_4++)
    {
      //A0 = A4 + 800*w_2 + w_4;
      //if (A0[772]== 0) break;
      if (pLocalGame->portrait_0772[w_2].name_000[w_4] == 0) break;
//
//
    };
    while (w_4 < 8)
    {
      //A0 = A4 + 800*w_2 + w_4;
      //A0[772] = 0;
      pLocalGame->portrait_0772[w_2].name_000[w_4] = 0;
      w_4++;
//
    };
    for (w_4=0; w_4<20; w_4++)
    {
      //A0 = A4 + 800*w_2 + w_4;
      //if (A0[780] == 0) break;
      if (pLocalGame->portrait_0772[w_2].title_008[w_4] == 0) break;
//
//
    };
    while (w_4 < 20)
    {
      //A0 = A4 + 800*w_2 + w_4;
      //A0[780] =0;
      pLocalGame->portrait_0772[w_2].title_008[w_4] = 0;
      w_4++;
    };
//
//
  };
  {
    //int timerSize = sequencedTimers?sizeof(TIMER):10;
    int timerSize = sequencedTimers?12:10;
    timerSize = extendedTimers?16:timerSize;
    //D0W = MyUnscrambleStream(LoadPnt(A4+4104), //Timers
    //                         timerSize * LE16(wordGear(A4+578)),
    //                         LE16(wordGear(pnt_4646+6)),
    //                         LE16(wordGear(pnt_4642+6))  );
    D0W = MyUnscrambleStream(pLocalGame->pointer_4104, //Timers
                             timerSize * LE16(pLocalGame->block2_0550.numTimers_028),
                             LE16(wordGear(pnt_4646+6)),
                             LE16(wordGear(pnt_4642+6))  );
  };
  if (D0W == 0) goto tag001da8;
  //D0W = MyUnscrambleStream(LoadPnt(A4+4108),
  //                        2*LE16(wordGear(A4+578)),
  //                        LE16(wordGear(pnt_4646+8)),
  //                        LE16(wordGear(pnt_4642+8)) );
  D0W = MyUnscrambleStream(pLocalGame->pointer_4108,
                          2*LE16(pLocalGame->block2_0550.numTimers_028),
                          LE16(wordGear(pnt_4646+8)),
                          LE16(wordGear(pnt_4642+8)) );
  if (D0W == 0) goto tag001da8;
  D0W = TAG00142e(pLocalGame); //Text (D0W <- numWordsInTextArray)
  if (D0W == 0) goto tag001da8;
  e.Word12184 = 0;
  //A0 = (aReg)LoadPnt((ui8 *)&e.Pnt24812) + 4112*D7W;
  pGame_A0 = &e.pGames_24812[D7W];
  //if (wordGear(A0) != 0)
  if (pGame_A0->word_0000 != 0)
  {
    //TAG001676(LoadPnt((ui8 *)&e.Pnt24812) + 4112*D7W, D7W);
    TAG001676(&e.pGames_24812[D7W], D7W);
  };
  if (D7W == 0)
  {
    //e.Word24814 = LE16(wordGear(A4+560));
    //e.Word24814 = pLocalGame->block2_0550.word_010;
    e.numCharacter_24814 = LE16(pLocalGame->block2_0550.numCharacter_010);
    TAG00136c();
  };
  //wordGear(A4) = 1;
  pLocalGame->word_0000 = 1;
  //A0 = (aReg)LoadPnt((ui8 *)&e.Pnt24812) + 4112*D7W;
  pGame_A0 = &e.pGames_24812[D7W];
  //A1 = A4;
  //A1 = A4;
  //memmove(A0, A1, 4112);
  memmove(pGame_A0, pLocalGame, sizeof(GAMESTRUCT));
  w_2 = 1;
  CLOSE(e.GameFile);
  e.GameFile = -1;
tag001d9a:
  TAG000952();
  TAG00054a();
  RETURN_i16(w_2);

tag001da8:
  {
    const char *pText;
    if (D7W != 0)
    {
      //A0 = (aReg)UtilityDiskMessages[25];
      pText = TranslateLanguage(UtilityDiskMessages[25]);
    }
    else
    {
      //A0 = (aReg)UtilityDiskMessages[14];
      pText = TranslateLanguage(UtilityDiskMessages[14]);
    };
    //UtilityDialogBox(_6_, A0, 8, UtilityDiskMessages[32], &s12406[0]);
    UtilityDialogBox(_6_, pText, 8, 
                     TranslateLanguage(UtilityDiskMessages[32]), &s12406[0]);
  };
  //TAG001676((ui8 *)A4, 0);
  TAG001676(pLocalGame, 0);
  w_2 = -1;
  if (e.GameFile >= 0)
  {
    CLOSE(e.GameFile);
    e.GameFile = -1;
  };
  goto tag001d9a;
  //RETURN_i16(0);
}

// *********************************************************
//
// *********************************************************
RESTARTABLE _TAG00260e(void)
{//(i16)
  static const char* pnt_12;
  //static ui8 *pBlock1;
  //static BLOCK1 *pBlock1;
  static BLOCK1_FMT2 *pBlock1F2;
  //static char *pnt_4;
  static GAMESTRUCT *pGame_4;
  RESTARTMAP
    RESTART(1)
    RESTART(2)
  END_RESTARTMAP
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  //pnt_4 = (char *)LoadPnt((ui8 *)&e.Pnt24812);
  pGame_4 = &e.pGames_24812[0];
  pnt_12 = TranslateLanguage(UtilityDiskMessages[30]);
  for (;;)
  {
    //if (   (wordGear(LoadPnt((ui8 *)&e.Pnt24812)) == 0)
    //if (   (e.pGames_24812[0].word_0000 == 0)
    //    || (e.Word24814 ==0)  )
    if (   (e.pGames_24812[0].word_0000 == 0)
        || (e.numCharacter_24814 ==0)  )
    {
      pnt_12 = TranslateLanguage(UtilityDiskMessages[29]);
    }
    else
    {
      //pBlock1 = (ui8 *)pnt_4 + 38;
      //pBlock1 = &pGame_4->block1_0038;
      pBlock1F2 = &pGame_4->block1_0038.block1Fmt2;
      //if (wordGear((ui8 *)pnt_4+2) != 1) 
      if (pGame_4->word_0002 != 1) 
      {
        //if (   (pBlock1[301] != 1)
        //if (   (pBlock1->ubyte_301 != 1)
        if (   (pBlock1F2->ubyte_301 != 1)
            //&& (LE16(wordGear(pBlock1+376)) != 1)  ) break;//Word22594
            //&& (LE16(pBlock1->uword_376) != 1)  ) break;//Word22594
            && (LE16(pBlock1F2->uword_376) != 1)  ) break;//Word22594
        //if (LE16(wordGear(pBlock1+378)) > 13) break; //Word22592
        //if (LE16(pBlock1->uword_378) > 13) break; //Word22592
        if (LE16(pBlock1F2->uword_378) > 13) break; //Word22592
// original        if (LE16(wordGear(pBlock1+378)) > 12) break; //Word22592
        // Error if !DungeonMaster savegame?
      }
      else
      {
#ifdef imp260e
        //if (pnt_4[337] > 1)
        if (pGame_4->block1_0038.ubyte_299 > 1)
        {
          //if (wordGear(pnt_4+412) != 1) break;
          if (pGame_4->block1_0038.uword_374 != 1) break;
          //if (wordGear(pnt_4+414) > 12) break;
          if (pGame_4->block1_0038.uword_376 > 12) break;
        };
#else
        NotImplemented(0x260e);
#endif
      };
      CheckChampionNames(_1_);
      RETURN_i16(i16Result);
    };
    break;
  };// while (false);
  UtilityDialogBox(_2_, pnt_12, 8, 
                   TranslateLanguage(UtilityDiskMessages[32]), s12406);
  RETURN_i16(0);
}

// *********************************************************
//
// *********************************************************
RESTARTABLE _TAG0026c4(pnt P1)
{//(i16)
  static const char* pnt_8;
  static pnt pnt_4;
  RESTARTMAP
    RESTART(1)
  END_RESTARTMAP
//;;;;;;;;;;;;;;;;;;;;;;;;;;
  pnt_8 = TranslateLanguage(UtilityDiskMessages[30]);
  pnt_4 = P1 + 38;
  if (   (wordGear(P1+2) == 2)
      && ((UI8)(pnt_4[301]) >= 2)
      && (LE16(wordGear(pnt_4+378)) == 13) ) RETURN_i16(1);
  UtilityDialogBox(_1_, pnt_8, 8, 
                   TranslateLanguage(UtilityDiskMessages[32]), s12406);
  RETURN_i16(0);
}

// *********************************************************
//
// *********************************************************
void TAG002724(void)
{
  dReg D0, D6, D7;
  //aReg A0, A1, A4;
  //char *pnt_8;
  //BLOCK1 *pBlock1_8;
  BLOCK1_FMT2 *pBlock1F2_8;
  //char *pnt_4;
  //BLOCK1 *pBlock1_4;
  BLOCK1_FMT2 *pBlock1F2_4;
//;;;;;;;;;;;;;;;;;;;;;;;;;;
  //pnt_4 = (char *)LoadPnt((ui8 *)&e.Pnt24812) + 4150;
  pBlock1F2_4 = &e.pGames_24812[1].block1_0038.block1Fmt2;
  //if (wordGear(LoadPnt((ui8 *)&e.Pnt24812) + 2) == 2)
  if (e.pGames_24812->word_0002 == 2)
  {
    //pnt_8 = (char *)LoadPnt((ui8 *)&e.Pnt24812) + 38;
    pBlock1F2_8 = &e.pGames_24812[0].block1_0038.block1Fmt2;
  }
  else
  {
    //pnt_8 = NULL;
    pBlock1F2_8 = NULL;
  };
  //if (pnt_8 != NULL)
  if (pBlock1F2_8 != NULL)  // Format 2
  {
    //MemoryMove((ui8 *)pnt_8+256, (ui8 *)pnt_4+256, 0x400, 0x400, 32);
    //MemoryMove((ui8 *)pBlock1_8->ubytes_256, (ui8 *)pBlock1_4->ubytes_256, 0x400, 0x400, 32);
    MemoryMove((ui8 *)pBlock1F2_8->ubytes_256, (ui8 *)pBlock1F2_4->ubytes_256, 0x400, 0x400, 32);
    //MemoryMove((ui8 *)pnt_8 +288,(ui8 *)pnt_4+288, 0x400, 0x400, 8);
    //MemoryMove((ui8 *)pBlock1_8->ubytes_288,(ui8 *)pBlock1_4->ubytes_288, 0x400, 0x400, 8);
    MemoryMove((ui8 *)pBlock1F2_8->ubytes_288,(ui8 *)pBlock1F2_4->ubytes_288, 0x400, 0x400, 8);
    //StoreLong((ui8 *)pnt_4+308, LoadLong((ui8 *)pnt_8+308) + 1);
    //pBlock1_4->ulong_308 = pBlock_8->ulong_308 + 1);
    pBlock1F2_4->ulong_308 = pBlock1F2_8->ulong_308 + 1;
    //if (pnt_8[301] == 1)
    //if (pBlock1F2_8[301] == 1)
    if (pBlock1F2_8->ubyte_301 == 1)
    {
      ASSERT(0,"pnt_8");//check for LE
      //wordGear(pnt_4 + 376) = 1;
      pBlock1F2_4->uword_376 = 1;
    }
    else
    {
      //wordGear(pnt_4+376) = wordGear(pnt_8+376);
      pBlock1F2_4->uword_376 = pBlock1F2_8->uword_376;
    };
  }
  else   // Format 1
  {
#ifdef alternateFormat
    MemoryMove(LoadPnt((ui8 *)&e.Pnt24812)+294, (ui8 *)pnt_4+256, 0x400, 0x400, 32);
    MemoryMove(LoadPnt((ui8 *)&e.Pnt24812)+326, (ui8 *)pnt_4+288, 0x400, 0x400, 8);
    ASSERT(0,"pnt_8"); //check for LE
    StoreLong((ui8 *)pnt_4+308, LoadLong(LoadPnt((ui8 *)&e.Pnt24812)+344) + 1);
    if (LoadPnt((ui8 *)&e.Pnt24812)[337] ==1)
    {
      ASSERT(0,"pnt24812");//check for LE
      wordGear(pnt_4+376) = 1;
    }
    else
    {
      wordGear(pnt_4+376) = wordGear(LoadPnt((ui8 *)&e.Pnt24812)+412);
    };
#else
    NotImplemented(0xc410c);
    //pBlock1_8 = NULL;
    pBlock1F2_8 = NULL;
#endif
  };
  //StrCpy((char *)LoadPnt((ui8 *)&e.Pnt24812) + 4116, pnt12238);
  StrCpy(e.pGames_24812[1].filename_0004, CSBGAMEDAT_12238);
  //MemoryMove(LoadPnt((ui8 *)&e.Pnt24812)+772, LoadPnt((ui8 *)&e.Pnt24812)+4884, 0x400, 0x400, 3200);
  MemoryMove(e.pGames_24812[0].portrait_0772, 
             e.pGames_24812[1].portrait_0772, 
             0x400, 0x400, 4*sizeof(PORTRAIT));
  //wordGear(LoadPnt((ui8 *)&e.Pnt24812) + 4672) = LE16(e.Word24814);
  wordGear(&e.pGames_24812[1].block2_0550.numCharacter_010) = LE16(e.numCharacter_24814);
  { 
    PORTRAIT *pPortrait_A4;
    GAMESTRUCT *pGame_A0;
    GAMESTRUCT *pGame_A1;
    //for (D7W=0, A4 = (aReg)LoadPnt((ui8 *)&e.Pnt24812)+4884;
    //     D7W < e.Word24814;
    //     D7W++, A4+=800)
    for (D7W=0, pPortrait_A4 = e.pGames_24812[1].portrait_0772;
         D7W < e.numCharacter_24814;
         D7W++, pPortrait_A4++)
    {
      ASSERT  (sizeof(*pPortrait_A4) == 800, "PORTRAIT");
//#ifdef implemented
      //A0 = (aReg)LoadPnt((ui8 *)&e.Pnt24812);
      pGame_A0 = &e.pGames_24812[0];
      //A1 = (aReg)LoadPnt((ui8 *)&e.Pnt24812);
      pGame_A1 = &e.pGames_24812[0];
      //D6W = (UI16)(LE16(wordGear(A1+4678)) - LE16(wordGear(A0+566)));
      D6W = (UI16)(LE16((pGame_A1 + 1)->block2_0550.word_006) - LE16(pGame_A0->block2_0550.word_006));
      if (D6W != 0)
      {
        D6W += 4;
        //A4[29] = (UI8)((A4[29] + D6W) & 3);
        pPortrait_A4->uByte_29 = (UI8)((pPortrait_A4->uByte_29 + D6W) & 3);
        //A4[28] = (UI8)((A4[28] + D6W) & 3);
        pPortrait_A4->uByte_28 = (UI8)((pPortrait_A4->uByte_28 + D6W) & 3);
      };
      //A4[30] = 1;
      pPortrait_A4->uByte_30 = 1;
      //A4[31] = 2;
      pPortrait_A4->uByte_31 = 2;
      //A4[32] = -1;
      pPortrait_A4->Byte_32 = -1;
      //A4[33] = 0;
      pPortrait_A4->uByte_33 = 0;
      //A4[34] = 0;
      pPortrait_A4->uByte_34 = 0;
      //A4[40] = 0;
      pPortrait_A4->uByte_40 = 0;
      //A4[41] = 0;
      pPortrait_A4->uByte_41 = 0;
      //A4[42] = 0;
      pPortrait_A4->uByte_42 = 0;
      //wordGear(A4+44) = (unsigned)0xffff;
      pPortrait_A4->uWord_44 = (unsigned)0xffff;
      //wordGear(A4+46) = (unsigned)0xffff;
      pPortrait_A4->uWord_46 = (unsigned)0xffff;
      //wordGear(A4+48) = LE16(LE16((ui16)(wordGear(A4+48) & 16)));
      pPortrait_A4->uWord_48 = LE16(LE16((ui16)(pPortrait_A4->uWord_48 & 16)));
      //wordGear(A4+50) = 0;
      pPortrait_A4->uWord_50 = 0;
      //wordGear(A4+52) = wordGear(A4+54);
      pPortrait_A4->uWord_52 = pPortrait_A4->health_054;
      //wordGear(A4+56) = wordGear(A4+58);
      pPortrait_A4->uWord_056 = pPortrait_A4->stamina_058;
      //wordGear(A4+60) = wordGear(A4+62);
      pPortrait_A4->uWord_060 = pPortrait_A4->mana_062;
      //wordGear(A4+64) = 0;
      pPortrait_A4->uWord_064 = 0;
      D0W = (i16)STRandom();
      D0W = sw((D0W & 255) + 1500);
      //wordGear(A4+66) = LE16(D0W);
      pPortrait_A4->uWord_066 = LE16(D0W);
      D0W = (i16)STRandom();
      D0W = sw((D0W & 255) + 1500);
      //wordGear(A4+68) = LE16(D0W);
      pPortrait_A4->uWord_068 = LE16(D0W);
      for (D6W=0; D6W<=6; D6W++)
      {
        //A0 = A4 + 3*D6W + 70;
        //A0[2] = 30;
        pPortrait_A4->struct070[D6W].uBytes_0[2] = 30;
        //A0[1] = A0[0];
        pPortrait_A4->struct070[D6W].uBytes_0[1] = pPortrait_A4->struct070[D6W].uBytes_0[0];
//
//
      };
      //A4[72] = 10;
      pPortrait_A4->struct070[0].uBytes_0[2] = 10;
      for (D6W=0; D6W<20; D6W++)
      {
        //A0 = A4 + 6*D6W + 92;
        //wordGear(A0) = 0;
        pPortrait_A4->array_092[D6W].uWord_0 = 0;
      };
      for (D6W=0; D6W<30; D6W++)
      { //possessions <- empty
        //A0 = A4 + 2*D6W + 212;
        //wordGear(A0) = (unsigned)0xffff;
        pPortrait_A4->uWord_212[D6W] = (unsigned)0xffff;
      };
      //wordGear(A4+272) = 0;
      pPortrait_A4->uWord_272 = 0;
      //wordGear(A4+274) = 0;
      pPortrait_A4->uWord_274 = 0;
//
//
//#else
//      NotImplemented(0xd193);
//#endif
      D6W = 1;  // avoid warning
      D0W = D6W;
      D6W = (i16)(D0W+1);
      pGame_A0 = NULL;
      pGame_A1 = pGame_A0;
    };
  };
}

// *********************************************************
//
// *********************************************************
//   TAG002a38
RESTARTABLE _StartChaos(const CSB_UI_MESSAGE *)
{//(void)
  static i16  w_130;
  RESTARTMAP
    RESTART(1)
    RESTART(2)
    RESTART(3)
    RESTART(4)
  END_RESTARTMAP
//;; This is the main program
//;;;;;;;;;;Does something and then reads a gamefile and then
//;;;;;;;;;;does something else.
//;;;;;;;;;;Calls function that reads gamefile, fiddles with
//;;;;;;;;;;characters and writes gamefile.
  keyboardMode = 2;
  InitializeE();
  w_130 = 1;
// What sense does this make?????
  if (w_130 == 2)
  {
    DontDoThis(0x2a4a);
  };
  FadeToPalette(_1_,&e.palette10938);
  TAG0002d0();
  TAG0031a6(0);
  //StorePnt((ui8 *)&e.PhysicalBase, physbase());
  e.PhysicalBase = physbase();
  //SetDLogicalBase(LoadPnt((ui8 *)&e.PhysicalBase));
  SetDLogicalBase(e.PhysicalBase);
  TAG0032b4(1, e.Word9124);
  TAG003294(e.Word9124, 0);
  TAG00022a(1);
  TAG0006e4(); // Set critical error handler
  TAG00324c();
  //ClearMemory(LoadPnt((ui8 *)&e.PhysicalBase), 32000);
  ClearMemory(e.PhysicalBase, 32000);
  TAG003264();
  FadeToPalette(_2_,&palette24862);
  //StorePnt((ui8 *)&e.Pnt24812, AllocateMemory(2*sizeof(GAMESTRUCT)); //Room for two games.?? (4112 byes each)
  e.pGames_24812 = (GAMESTRUCT *)AllocateMemory(2*sizeof(GAMESTRUCT)); //Room for two games.?? (4112 byes each)
  //ClearMemory(LoadPnt((ui8 *)&e.Pnt24812), 2*sizeof(GAMESTRUCT));
  ClearMemory((ui8 *)e.pGames_24812, 2*sizeof(GAMESTRUCT));
  w_130 = -1;
  while (w_130 == -1)
  {
    ReadSavedGame(_3_, 0);
    w_130 = i16Result;
//
  };

  // ROQUEN: Comparison always true
  //if (w_130 != 0xff9d)
  {
    TAG004518(_4_);
  };
  //TAG001676(LoadPnt((ui8 *)&e.Pnt24812), 0);
  TAG001676(&e.pGames_24812[0], 0);
  //TAG001676(LoadPnt((ui8 *)&e.Pnt24812)+4112, 1);
  TAG001676(&e.pGames_24812[1], 1);
  //ReleaseMemory(LoadPnt((ui8 *)&e.Pnt24812));
  ReleaseObject(e.pGames_24812);
  e.pGames_24812 = NULL; // Added 20180414 PRS
  TAG000952();
  TAG00022a(2);
  TAG0032b4(2, e.Word9124);
  TAG00323c();
  TAG00031c();
  RETURN;
}

// *********************************************************
//
// *********************************************************
//  TAG002c18
//i16 Random(void)
//{
//  dReg D0;
//;;;;;;;;;;;;;;;;;
//  if (LoadLong(&e.Long9220) == 0)
//  {
//    StoreLong(&e.Long9220, LoadLong(&e.Long24300) + 1023013);
//  };
//  D0L = Times(LoadLong(&e.Long9220), 314159) + 11;
//  StoreLong(&e.Long9220, D0L);
//  return (D0L >> 8) & 0xffffff;
//}

// *********************************************************
//
// *********************************************************
//  TAG002c70
i16 TestInRectangle(wordRectPos *rect, i32 x, i32 y)
{
  dReg D6, D7;
  wordRectPos *rectA4;
  rectA4 = rect;
  D7W = sw(x);
  D6W = sw(y);
  if (D7W > rectA4->x2) return 0;
  if (D7W < rectA4->x1) return 0;
  if (D6W > rectA4->y2) return 0;
  if (D6W < rectA4->y1) return 0;
  return 1;
}

// *********************************************************
// Discard any queued keystrokes
// *********************************************************
void TAG002ca8(void)
{
//;;;;;;;;;;;;;;;;;;;;;;;;;
  while (UI_CONSTAT() != 0)
  {
    UI_DIRECT_CONIN();
  };
  e.Byte9212 = -128;
  e.FirstQueuedMouseEvent = 0;
  e.LastQueuedMouseEvent = 4;
  e.Word9210 = 0;
  e.Byte9212 = 0;
}

// *********************************************************
//
// *********************************************************
i16 TAG002cdc(DlgButton *P1, i32 x, i32 y, i32 leftRight)
{
  dReg D0, D5, D6, D7;
  DlgButton *dlgButtonA4;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  dlgButtonA4 = P1;
  D7W = sw(x);
  D6W = sw(y);
  if (dlgButtonA4 == NULL) return 0;
  while ( (D5W = dlgButtonA4->byte0) != 0)
  {
    D0W = TestInRectangle(&dlgButtonA4->rectPos2, D7W, D6W);
    if (D0W != 0)
    {
      if (leftRight & dlgButtonA4->byte11)
      {
        if ((dlgButtonA4->byte1 & 1) == 0)
        {
          return D5W;
        };
      };
    };
    dlgButtonA4++;
  };
  return D5W;
}

// *********************************************************
//
// *********************************************************
//   TAG002d3c
void EnqueMouseClick(i32 P1,i32 P2,i32 P3)
{
  dReg D0, D4, D5, D6, D7;
  aReg A0;
  S12406 **pS12406A4;
  i16 w_2;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  D7W = sw(P1);
  D6W = sw(P2);
  if (e.Byte9212 != 0)
  { // Que is busy.  Put click in pending state.
    e.Word9210 = 1;
    e.Word9208 = D7W;
    e.Word9206 = D6W;
    e.Word9204 = sw(P3);
    return;
  };
  D5W = sw(e.LastQueuedMouseEvent + 2);
  if (D5W > 4) D5W -= 5;
  if (D5W == e.FirstQueuedMouseEvent) return;
  for (w_2 = 0, pS12406A4 = ppS12406_9142;
       w_2 < 6;
       w_2++, pS12406A4++)
  {
    if (*pS12406A4 == NULL) continue;
    if ((*pS12406A4)->byte1 & 1) continue;
    D4W = TAG002cdc((*pS12406A4)->pDlgButton2, D7W, D6W, P3);
    if (D4W == 0) continue;
    D0W = D5W;
    D5W--;
    if (D0W == 0) D5W = 4;
    e.LastQueuedMouseEvent = D5W; //Last item in queue
    A0 = e.Byte9202 + 6*D5W;
    A0[0] = D4B;
    A0[1] = (i8)w_2;
    wordGear(A0+2) = D7W;
    wordGear(A0+4) = D6W;
    break;
//
//
  };
  e.Byte9212 = 0;
}



// *********************************************************
//
// *********************************************************
//   TAG002e2c
void CheckPendingMouseClick(void)
{
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  if (e.Word9210 == 0) return;
  e.Word9210 = 0;
  EnqueMouseClick(e.Word9208, e.Word9206, e.Word9204);
}

// *********************************************************
//
// *********************************************************
void TAG002e4a(i32 P1)
{
  dReg D0, D1, D4, D5, D6, D7;
  aReg A0, A3;
  S12406 **pS12406A4;
  i16 w_4 = 0x1ccc;
  i16 w_2;
//;;;;;;;;;;;;;;;;;;;;;;;;;
  D7L = P1;
  if (D7W == 0)
  {
    D7L >>= 8;
  }
  else
  {
    D0L = D7L & 0xff;
    D1L = D7L >> 8 & 0xff00;
    D4W = sw(D0W | D1W); // UpperHalf,LowerHalf in D4
    for (D5L=0;
         (D5L<7) && (D4W>=word9138[D5W]);
         D5W++)
    {
      if (D4W != word9138[D5W]) continue;
      D7L = D4UW;
      break;
//
//
    };
  };
  e.Byte9212 = -128;
  D6W = sw(e.LastQueuedMouseEvent + 2);
  if (D6W > 4) D6W -= 5;
  if (D6W != e.FirstQueuedMouseEvent)
  {
    w_2 = 0;
    for (D5L=0, pS12406A4 = ppS12406_9142;
         D5W < 6;
         D5W++, pS12406A4++)
    {
      if (*pS12406A4 == NULL) continue;
      if ((*pS12406A4)->byte1 & 1) continue;
      A3 = (*pS12406A4)->pnt6;
      if (A3 == NULL) continue;
      for ( ;
            (D4W = sw(A3[0]&255)) != 0;
            A3+=4)
      {
        if (A3[1] & 1) continue;
        if (wordGear(A3+2) == D7W)
        {
tag002f10:
          D0W = D6W;
          D6W--;
          if (D0W == 0) D6W = 4;
          e.LastQueuedMouseEvent = D6W;
          A0 = e.Byte9202+6*D6W;
          A0[0] = D4B;
          A0[1] = D5B;
          wordGear(A0+2) = (ui16)-1;
          wordGear(A0+4) = D7W;
          goto tag002f9a;
        };
        if (wordGear(A3+2) == -1)
        {
          w_2 = D4W;
          w_4 = D5W;
        };
//
//
      };
//
    };
    if (w_2 != 0)
    {
      D4W = w_2;
      ASSERT(w_4 != 0x1ccc,"w_4");
      D5W = w_4;
      goto tag002f10;
    };
  };
tag002f9a:
  e.Byte9212 = 0;
  CheckPendingMouseClick();
}

// *********************************************************
//
// *********************************************************
i16 TAG002faa(S12406 *P1)
{
  dReg D7;
  S12406 **pS12406A4;
//;;;;;;;;;;;;;;;;;;;;;;;;;
  for (D7W=0, pS12406A4=ppS12406_9142; D7W<6; D7W++, pS12406A4++)
  {
    if (*pS12406A4 != NULL) continue;
    *pS12406A4 = P1;
    TAG002ca8();
    return D7W;
//
  };
  return -1;
}

// *********************************************************
//
// *********************************************************
//       TAG003000
S12406 **SetActiveDialog(S12406 **P1)
{
  S12406 **ppS12406_4;
//;;;;;;;;;;;;;;;;;;;;;;;;;;
  ppS12406_4 = ppS12406_9142;
  ppS12406_9142 = P1;
  TAG002ca8();
  return ppS12406_4;
}

// *********************************************************
//
// *********************************************************
RESTARTABLE _TAG00301c(pnt P1)
{//(i16)
  static dReg D0, D7;
  static S12406 *S12406A0;
  static aReg A0, A1, A3, A4;
  static i32 pressed_key;
  RESTARTMAP
    RESTART(1)
  END_RESTARTMAP
//;;;;;;;;;;;;;;;;;;;;;;
  A4 = P1;
  //while (pumper(), wvbl(), CONSTAT() != 0)
  for (;;)
  {
    wvbl(_1_);
    if (UI_CONSTAT() == 0) break;
    pressed_key = UI_DIRECT_CONIN();
    TAG002e4a(pressed_key);
  	if(pressed_key) EditCharacterName((char)pressed_key);
  };
  e.Byte9212 = -128;
  D7W = sw(e.LastQueuedMouseEvent + 1);
  if (D7W > 4) D7W = 0;
  if (D7W == e.FirstQueuedMouseEvent)
  { // Means queue is empty
    A0 = A4;
    A1 = e.Byte9172;
    StoreLong(A0, LoadLong(A1));
    wordGear(A0+4) = wordGear(A1+4);
    A0 += 6;
    A1 += 6;
  }
  else
  {
    A0 = &e.Byte9202[6*e.FirstQueuedMouseEvent];
    A1 = A4;
    StoreLong(A1, LoadLong(A0));
    wordGear(A1+4) = wordGear(A0+4);
    e.FirstQueuedMouseEvent++;
    if (e.FirstQueuedMouseEvent > 4) e.FirstQueuedMouseEvent = 0;
  };
  e.Byte9212 = 0;
  CheckPendingMouseClick();
  if (*A4 != 0)
  {
    D7W = (UI8)(A4[1]);
    S12406A0 = ppS12406_9142[D7W];
    A4[1] = S12406A0->byte0;
    S12406A0 = ppS12406_9142[D7W];
    A3 = S12406A0->pnt10;
    if (A3 != NULL)
    {
      DontDoThis(0x30e0);; //What to do here?
      //(A3)(A4);
    };
  };
  if (A4[0] == 0) A4[1] =0;
  RETURN_i16 ( (UI16)(((A4[1]<<8)&0xff00) | (A4[0]&0xff)) );
}

// *********************************************************
//
// *********************************************************
void TAG003106(i16)
{
  DontDoThis(0x3106);
}

// *********************************************************
//
// *********************************************************
void TAG0031a6(i16 P1)
{
  i16 w_138[57];
  i16 w_24[10];
  i16 w_4;
  i16 w_2;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  TAG00507e();
  e.Word9124 = TAG005868(&w_2, &w_2, &w_2, &w_2);
  for (w_2=0; w_2<10; w_2++)
  {
    w_24[w_2] = 1;
//
  };
  w_4 = 2;
  if (P1 == 0)
  {
    TAG005d8e(w_24, &e.Word9124, w_138);
  }
  else
  {
    TAG003106(1);
    TAG005d12(w_24, &e.Word9124, w_138);
    TAG003106(2);
  };
}

// *********************************************************
//
// *********************************************************
void TAG00323c(void)
{
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  TAG005df2(e.Word9124);
  TAG0050d2();
}

// *********************************************************
//
// *********************************************************
void TAG00324c(void)
{
  dReg D0;
//;;;;;;;;;;;;;;;;;;;;;
  D0W = e.Word8074;
  e.Word8074++;
  if (D0W == 0)
  {
    TAG0061e2(e.Word8054);
  };
}

// *********************************************************
//
// *********************************************************
void TAG003264(void)
{
  dReg D0;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  D0W = e.Word8074;
  e.Word8074--;
  if (D0W == 1)
  TAG0061ba(e.Word8054, 1);
}

// *********************************************************
//
// *********************************************************
void TAG003294(i16 P1, i16 P2)
{
//;;;;;;;;;;;;;;;;;;;;
  TAG00589e(0, P2);
  TAG0061ba(P1, 0);
}

// *********************************************************
//
// *********************************************************
void TAG00334c(dReg& D0, dReg& D1)
{
    if (e.Word8066 != 0)
    {
      D0W = e.Word8070;
      D1W = e.Word8068;
    }
    else
    {
      e.Word8070 = D0W;
      e.Word8068 = D1W;
    };
}

// *********************************************************
//
// *********************************************************
void TAG003372(dReg& D0)
{
  dReg D3;
  if (e.Word8056 == 0)
  {
    D3W = e.Word8072;
    if ( (D3W != 0) && (D0W!=0) )
    {
      e.Word8072 = D0W;
    }
    else
    {
      e.Word8072 = D0W;
      if (D0W != 0)
      {
        EnqueMouseClick(e.Word8070, e.Word8068, D0W);
      };
    };
    e.Word8056 = 0;
  };
}

// *********************************************************
//
// *********************************************************
void TAG0032b4(i16 P1, i16 P2)
{
  aReg A4;
  pnt  pnt_10;
  i16  w_6;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  switch (P1)
  {
  case 1:
    e.Word8054 = P2;
    TAG006202(e.Word8054, &w_6, &e.Word8070, &e.Word8068);
    A4 = (pnt)0x00334c;
    //TAG00626e(e.Word8054, (char *)A4, (char **)&e.Pnt8064);
    TAG00626e(e.Word8054, (char *)A4, &e.Pnt8064);
    A4 = (pnt)0x003372;
    TAG00623a(e.Word8054, (char *)A4, (char **)&e.Pnt8060);
    break;
  case 2:
    //TAG00626e(e.Word8054, (char *)LoadPnt((ui8 *)&e.Pnt8064), (char **)&pnt_10);
    TAG00626e(e.Word8054, e.Pnt8064, (char **)&pnt_10);
    //TAG00623a(e.Word8054, (char *)LoadPnt((ui8 *)&e.Pnt8060), (char **)&pnt_10);
    TAG00623a(e.Word8054, e.Pnt8060, (char **)&pnt_10);
    break;
//;;;;;;;;;;;;;;;;;;;;;;;;;
  default:{};
  }; //switch (P1);
}

// *********************************************************
//
// *********************************************************
//   TAG0033b6
void BitBltSquareImage(pnt P1,const wordRectPos *rect,i16 P3,i16 P4)
{
//;;;;;;;;;;;;;;;
  e.Word8048 = 0;
  TAG0088b2((ui8 *)P1,
            //LoadPnt((ui8 *)&e.PhysicalBase),
            e.PhysicalBase,
            (RectPos *)rect,
            0,
            0,
            P3,
            160,
            P4);
}

// *********************************************************
//
// *********************************************************
//   TAG0033e6
void FillScreenRectangle(wordRectPos *rp, i16 P2)
{
//;;;;;;;;;;
  //FillRectangle(LoadPnt((ui8 *)&e.PhysicalBase),
  FillRectangle(e.PhysicalBase,
                (RectPos *)rp,
                P2,
                160);
}

// *********************************************************
//
// *********************************************************
//   TAG0034dc
void ExpandRectangle(wordRectPos *rp1,
                     wordRectPos *rp2,
                     i16 deltaX,
                     i16 deltaY)
{
  rp2->x1 = sw(rp1->x1 - deltaX);
  rp2->x2 = sw(rp1->x2 + deltaX);
  rp2->y1 = sw(rp1->y1 - deltaY);
  rp2->y2 = sw(rp1->y2 + deltaY);
}

// *********************************************************
//
// *********************************************************
//   TAG003406
void DrawNameTitleCarret(i16 P1, i16 P2)
{
  dReg D0;
  wordRectPos *rectA4;
//;;;;;;;;;;;;;;;;;;;;

  rectA4 = &dlgButton5050[P1+13].rectPos2;
  e.EditBoxSelected = P1;
  e.EditBoxPosition = P2;
  TAG00324c();
  if (e.Word5168 != 0)
  {
    FillScreenRectangle(&e.wRectPos5176, 1);
  };
  D0W = sw(rectA4->x1 + 6*P2 + 1);
  e.wRectPos5176.x2 = D0W;
  e.wRectPos5176.x1 = D0W;
  e.wRectPos5176.y1 = rectA4->y1;
  e.wRectPos5176.y2 = rectA4->y2;
  FillScreenRectangle(&e.wRectPos5176, 15);
  TAG003264();
  e.Word5168 = 1;
  e.Word5178 = 30;
}

// *********************************************************
//
// *********************************************************
//          _TAG00352c
RESTARTABLE _InvertButton(const wordRectPos *pRect)
{//(void)
  static dReg D5, D6, D7;
  static aReg A3;
  const static wordRectPos *rectA4;
  static wordRectPos rect_8;
  static i32 i;
  RESTARTMAP
    RESTART(1)
  END_RESTARTMAP
//;;;;;;;;;;;;;;
  rectA4 = pRect;
  rect_8.x1 = 0;
  rect_8.y1 = 0;
  D6W = sw(rectA4->x2 - rectA4->x1);
  rect_8.x2 = sw(D6W + 1);
  D5W = sw(rectA4->y2 - rectA4->y1 + 1);
  rect_8.y2 = sw(D5W + 1);
  D6W = sw(((D6W + 15) & 0xfff0) / 2);
  TAG00324c();
  A3 = e.Byte3764;
  ASSERT(size3764 >= D6W*(rect_8.y2+1),"size3467");
  //TAG0088b2(LoadPnt((ui8 *)&e.PhysicalBase), //src
  TAG0088b2(e.PhysicalBase, //src
            (ui8 *)A3,             //dst
            (RectPos *)&rect_8, //dst position
            rectA4->x1,     //src x offset
            rectA4->y1,     //src y offset
            160,            //src byte width
            D6W,            //dst byte width
            -1);
  for (D7W = sw((D6W * D5W)/4);
       D7W > 0;
       D7W--)
  {
    StoreLong(A3, LoadLong(A3) ^ -1); 
    A3 += 4;
  };
  BitBltSquareImage(e.Byte3764, rectA4, D6W, -1);
  for (i=0; i<5; i++)
  {
    wvbl(_1_);
  }
  TAG003264();
  RETURN;
}

// *********************************************************
//
// *********************************************************
void TAG004ea2(i16 *)
{
  HopefullyNotNeeded(0xcd89);
  // This appears to do nothing on the Atari.
  // A parameter of 115 does something.  But 200 does nothing.
//;;;;;;;;;;;;;;;;;;;;;;;;
}

//file = chaos.ftl      offset =68ec
//
char data004f1c[348] = {
           0, 1, 0, 2, 1, 1, 2, 1, 1, 0, 1, 1, 2, 1, 1, 1,
//004f2c
           1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1,
//004f3c
           0, 3, 5, 0, 5, 5, 0, 0, 1, 1, 2, 1, 0,16, 7, 1,
//004f4c
           2, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 2,
//004f5c
           1, 1, 2, 1, 1, 2, 1, 1, 1, 1, 2, 1, 1, 1, 0, 0,
//004f6c
           0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 1, 1, 1, 1, 1,
//004f7c
           6, 1, 1, 4, 1, 1, 1, 3, 1, 2, 1, 1, 4, 2, 1, 8,
//004f8c
           1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 9, 1, 1, 1, 1,
//004f9c
           1, 1, 1, 0, 0, 5, 1, 3, 3, 1, 2, 2, 1, 0, 0, 0,
//004fac
           0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
//004fbc
           0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
//004fcc
           0, 0, 0, 0, 4, 3, 0, 8, 3, 0, 6, 1, 0, 8, 1, 0,
//004fdc
           8, 1, 0, 4, 1, 1, 3, 1, 1, 0, 5, 0, 1, 1, 1, 0,
//004fec
           5, 0, 0, 1, 1, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0,
//004ffc
           0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
//00500c
           0, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
//00501c
           0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 1,
//00502c
           0, 5, 1, 0, 1, 1, 0, 1, 1, 0, 2, 5, 0, 6, 1, 0,
//00503c
           2, 1, 0, 1, 1, 0, 6, 5, 0, 0, 0, 0, 0, 1, 1, 0,
//00504c
           1, 0, 2, 1, 0, 2, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0,
//00505c
           0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 3, 1, 2,
//00506c
           1, 1, 1, 1, 1, 1, 0, 1, 1, 0, 1, 2};


// *********************************************************
//
// *********************************************************
//   TAG0035cc
void DrawDialogBoxOutline(wordRectPos *rectPos,
                          i16 lineWidth,
                          i16 P3,
                          i16 P4)
{
  //dReg D6, D7;
  dReg D6;
  wordRectPos rectPos_24;
  wordRectPos rectPos_16;
  wordRectPos rectPos_8;
  wordRectPos *rpA4;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  rpA4 = rectPos;
  //D7W = lineWidth;
  if (lineWidth < 0)
  {
    ExpandRectangle(rpA4, &rectPos_8, lineWidth, lineWidth);
    lineWidth = sw(-lineWidth);
  }
  else
  {
    MemoryMove((ui8 *)rpA4, (ui8 *)&rectPos_8, 0x400, 0x400, 8);
  };
  FillScreenRectangle(&rectPos_8, P4);
  if (lineWidth == 0) return;
  ExpandRectangle(&rectPos_8, &rectPos_16, lineWidth, lineWidth);
  lineWidth--;
  for (D6W=0; D6W<4; D6W++)
  {
    MemoryMove((ui8 *)&rectPos_16, (ui8 *)&rectPos_24, 0x400, 0x400, 8);
    switch (D6W)
    {
    case 0:
      rectPos_24.x2 = sw(rectPos_24.x1 + lineWidth);
      break;
    case 1:
      rectPos_24.x1 = sw(rectPos_24.x2 - lineWidth);
      break;
    case 2:
      rectPos_24.y2 = sw(rectPos_24.y1 + lineWidth);
      break;
    case 3:
      rectPos_24.y1 = sw(rectPos_24.y2 - lineWidth);
    };// case (D6W)
    FillScreenRectangle(&rectPos_24, P3);
//
  };
}

// *********************************************************
//
// *********************************************************
//   TAG0036c0
void DrawMagnifiedCharacterImage(void)
{
  dReg D0, D6, D7;
  //aReg A4;
  CHARACTER_IMAGE *pCharacterImage_A4;
  wordRectPos rect_8;
//;;;;;;;;;;;;;;;;;;;;;;
  //A4 = e.CharacterImages_7058 + 464*e.ActualCharacter;
  pCharacterImage_A4 = &e.characterImages_7058[e.ActualCharacter];
  rect_8.x1 = wRectPos5162.x1;
  TAG00324c();
  for (D7W=0; D7W<32; D7W++)
  {
    rect_8.x2 = sw(rect_8.x1 + 2);
    rect_8.y1 = wRectPos5162.y1;
    for (D6W=0; D6W<29; D6W++)
    {
      rect_8.y2 = sw(rect_8.y1 + 2);
      //D0W = sw(GetPixel(A4, D7W, D6W, 16));
      D0W = sw(GetPixel((i8 *)pCharacterImage_A4->ubytes, D7W, D6W, 16));
      FillScreenRectangle(&rect_8, D0W);
      rect_8.y1 += 3;
//
    };
    rect_8.x1 += 3;
//
//
  };
  TAG003264();
}

// *********************************************************
//
// *********************************************************
//   TAG003748
void PrintCharacterName(i16 chIdx)
{
  dReg D0;
  //aReg A1;
  PORTRAIT *pPortrait_A1;
  wordRectPos *rectA4;
  char name[9];
  i32 i;
//;;;;;;;;;;;;;;
  ASSERT(chIdx < 4,"chidx");
  rectA4 = &wRectPos5138[chIdx];
  FillScreenRectangle(rectA4, 1);
  if (chIdx != e.ActualCharacter)
  {
    D0L = 15;
  }
  else
  {
    D0L = 9;
  };
  //A1 = (aReg)LoadPnt((ui8 *)&e.Pnt24812) + 800*chIdx + 772;
  pPortrait_A1 = &e.pGames_24812[0].portrait_0772[chIdx];
  // character name should be redrawn completely
  //strcpy(name, (char *)LoadPnt((ui8 *)&e.Pnt24812) + 800*chIdx + 772);
  strcpy(name, pPortrait_A1->name_000);
  for(i=0; i<8; i++)
  {
	  if((name[i]==0) || (name[i]<'A') || (name[i]>'Z'))
		  name[i] = 32;
  }
  name[8] = 0;
  TextToScreen(rectA4->x1+2, 9, D0W, 1, name);
}

// *********************************************************
//
// *********************************************************
//   TAG0037ac
void DrawCharacterHeader(i16 P1, i16 P2)
{
  //aReg A0;
  CHARACTER_IMAGE *pCharacterImage_A0;
  wordRectPos *rectA0, *rectA1;
  DlgButton *dlgButtonA0;
  wordRectPos rect_8;
//;;;;;;;;;;;;;;;;;;;
  TAG00324c();
  //if (P1 < e.Word24814)
  if (P1 < e.numCharacter_24814)
  {
    PrintCharacterName(P1);
    rectA0 = &wRectPos5106[P1];
    DrawDialogBoxOutline(rectA0, 1, P2, 1);
    //A0 = e.CharacterImages_7058 + 464*P1;
    pCharacterImage_A0 = &e.characterImages_7058[P1];
    rectA1 = &wRectPos5106[P1];
    BitBltSquareImage((i8 *)pCharacterImage_A0, rectA1, 16, -1);
  }
  else
  {
    dlgButtonA0 = &dlgButton5050[P1];
    MemoryMove((ui8 *)dlgButtonA0+2, (ui8 *)&rect_8, 0x400, 0x400, 8);
    rect_8.y2--;
    FillScreenRectangle(&rect_8, 0);
  };
  TAG003264();
}

// *********************************************************
//
// *********************************************************
//   TAG003860
void AddDialogButton(DlgButton *P1, const char* P2, i16 P3)
{
  dReg D0, D1;
  DlgButton *dlgButtonA4;
  i16 w_18;
  wordRectPos rectPos_8;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  dlgButtonA4 = P1;
  TAG00324c();
  MemoryMove((ui8 *)&dlgButtonA4->rectPos2, (ui8 *)&rectPos_8, 0x400, 0x400, 8);
  DrawDialogBoxOutline(&rectPos_8, 2, P3, 0);
  w_18 = sw((rectPos_8.x1 + rectPos_8.x2)/2);
  D0W = sw(3* StrLen(P2));
  D1W = sw(w_18 - D0W);
  TextToScreen(D1W+1, rectPos_8.y2-2, 15, 0, P2);
  TAG003264();
}

// *********************************************************
//
// *********************************************************
//   TAG0038ea
void GetPalleteItemRect(i16 P1, wordRectPos *rect)
{
  wordRectPos *rectA4;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  rectA4 = rect;
  rectA4->x1 = wRectPos5154.x1;
  rectA4->x2 = wRectPos5154.x2;
  rectA4->y1 = sw(wRectPos5154.y1 + 8*P1);
  rectA4->y2 = sw(rectA4->y1 + 6);
}

// *********************************************************
//
// *********************************************************
//   TAG003922
void DrawSelectedColor(i16 P1)
{
  dReg D0;
  wordRectPos rect_8;
//;;;;;;;;;;;;;;;;;;;;;;;
  if (P1 == e.Word7062) return;
  TAG00324c();
  if (e.Word7062 >= 0)
  {
    GetPalleteItemRect(e.Word7062, &rect_8);
    D0L = byte4712[e.Word7062];
    DrawDialogBoxOutline(&rect_8, 1, 0, D0W);
  };
  e.Word7062 = P1;
  GetPalleteItemRect(e.Word7062, &rect_8);
  D0W = byte4712[e.Word7062];
  DrawDialogBoxOutline(&rect_8, 1, 15, D0W);
  TAG003264();
}

// *********************************************************
//
// *********************************************************
void TAG0039a8(void)
{
  //aReg A0;
  ARRAY_5202 *pArray_5202_A0;
  CHARACTER_IMAGE *pCharacterImage_A0;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  //A0 = e.Byte5202 + 6*e.ActualCharacter;
  pArray_5202_A0 = &e.array_5202[e.ActualCharacter];
  //e.Word4696 = ub(A0[4]);
  e.Word4696 = pArray_5202_A0->ubyte_4;
  //A0 = e.CharacterImages + 464*e.ActualCharacter;
  pCharacterImage_A0 = &e.characterImages_7058[e.ActualCharacter];
  //MemoryMove((ui8 *)A0, (ui8 *)e.Byte4694, 0x400, 0x400, 464);
  MemoryMove((ui8 *)pCharacterImage_A0, (ui8 *)e.Byte4694, 0x400, 0x400, 464);
}

// *********************************************************
//
// *********************************************************
//   TAG0039ea
void FillCharacterNameEditBox(const char *P1, i16 P2, i16 P3)
{
  dReg D0, D1, D5, D6, D7;
  wordRectPos *rectA4;
//;;;;;;;;;;;;;;;;;;
  rectA4 = &dlgButton5050[P2].rectPos2;
  D5W = StrLen(P1);
  e.Word5168 = 0;
  FillScreenRectangle(rectA4, 1);
  D7W = sw(rectA4->x1 + 2);
  D6W = sw(rectA4->y2 - 2);
  TextToScreen(D7W, D6W, 9, 1, P1);
  D0W = sw(6*D5W + D7W);
  D1W = sw(P3 - D5W);
  TextToScreen(D0W,
               D6W,
               9,
               1,
               "___________________" + 19 - D1W);
}

// *********************************************************
//
// *********************************************************
//   TAG003a7c
void PrintAttributeValue(const char* P1, i32 P2, i32 P3)
{
  char b_20[20];
//;;;;;;;;;;;;;;;;;;;;;;
  //atari_sprintf(b_20, "%s %d", (i32)P1, P2);
  atari_sprintf(b_20, "%s %d", (intptr_t)(P1), P2, 0);
  TextToScreen(17, P3, 13, 0, b_20);
}

// *********************************************************
//
// *********************************************************
//          _TAG003ab6
RESTARTABLE _DrawCharacterDetails(const i32 P1, const i32 P2)
{//(void)
  static dReg D0, D3, D4, D5, D6, D7;
  //static aReg A0, A1, A4;
  static const char *pChar_A0;
  static const char *pChar_A1;
//  static aReg A1;
  static CHARACTER_IMAGE *pCharacterImage_A0;
  static PORTRAIT *pPortrait_A4;
  static char b_28[20];
  static wordRectPos rect_8;
  RESTARTMAP
    RESTART(1)
    RESTART(2)
  END_RESTARTMAP
//;;;;;;;;;;;;;;;;;;;;;;
  D7W = sw(P1);
  D6W = e.ActualCharacter;
  if (D7W == D6W) RETURN;
  e.ActualCharacter = D7W;
  TAG00324c();
  if (D6W >= 0)
  {
    DrawCharacterHeader(D6W, 0);
  };
  if (P2 != 0)
  {
    TAG0039a8();
  };
  DrawCharacterHeader(D7W, 15);
  wvbl(_1_);
  BitBltSquareImage((i8 *)FrameImage, &wRectPos5106[4], 24, -1);
  wvbl(_2_);
  //A0 = e.CharacterImages + 464*D7W;
  pCharacterImage_A0 = &e.characterImages_7058[D7W];
  //BitBltSquareImage(A0, &wRectPos5106[5], 16, 1);
  BitBltSquareImage((i8 *)pCharacterImage_A0, &wRectPos5106[5], 16, 1);
  FillScreenRectangle(&wRectPos5106[6], 0);
  //A4 = (aReg)LoadPnt((ui8 *)&e.Pnt24812) + 800*D7W + 772;
  pPortrait_A4 = &e.pGames_24812->portrait_0772[D7W];
  //FillCharacterNameEditBox((char *)A4, 13, 7);
  FillCharacterNameEditBox(pPortrait_A4->name_000, 13, 7);
  //FillCharacterNameEditBox((char *)A4+8, 14, 19);
  FillCharacterNameEditBox(pPortrait_A4->title_008, 14, 19);
  //D0W = StrLen((char *)A4);
  D0W = StrLen(pPortrait_A4->name_000);
  DrawNameTitleCarret(0, D0W);
  for (D5L=121, D6L=0; D6W<4; D6W++)
  {
    //A0 = A4 + 6*D6W;
    ARRAY_092 *pArray_092_A0;
    pArray_092_A0 = &pPortrait_A4->array_092[D6W];
    //D3L = LE32(LoadLong(A0+94));
    D3L = LE32(LoadLong(&pArray_092_A0->uLong_2));
    for (D4L=0; D3L >= 500; D3L>>=1, D4W++){};
//

    if (D4W == 0) continue;
    D4W--;
    if (D4W >= 15) D4W = 14;
    //A0 = (aReg)pnt4772[D4W];
    pChar_A0 = TranslateLanguage(pnt4772[D4W]);
    //A1 = (aReg)pnt4788[D6W];
    pChar_A1 = TranslateLanguage(pnt4788[D6W]);
    //atari_sprintf(b_28, "%s %s", (i32)A0, (i32)A1);
    atari_sprintf(b_28, "%s %s", (intptr_t)pChar_A0, (intptr_t)pChar_A1, 0);
    TextToScreen(17, D5W, 13, 0, (char *)b_28);
    D5W += 7;
//
  };
  if (D5W < 149)
  {
    rect_8.x1 = 17;
    rect_8.x2 = 131;
    rect_8.y1 = sw(D5W-6);
    rect_8.y2 = 149;
    FillScreenRectangle(&rect_8, 0);
  };
  PrintAttributeValue(TranslateLanguage("HEALTH     "), LE16(pPortrait_A4->health_054), 154);
  PrintAttributeValue(TranslateLanguage("STAMINA    "), LE16(pPortrait_A4->stamina_058)/10, 161);
  PrintAttributeValue(TranslateLanguage("MANA       "), LE16(pPortrait_A4->mana_062), 168);
  DrawMagnifiedCharacterImage();
  TAG003264();
  RETURN;
}

// *********************************************************
//
// *********************************************************
//          _TAG003f12
RESTARTABLE _DrawCharacterEditorScreen(void)
{//(void)
  static dReg D7;
  static wordRectPos rect_8;
  static const char *pTranslatedText;
  static int textLength;
  RESTARTMAP
    RESTART(1)
  END_RESTARTMAP
//;;;;;;;;;;;;;;;
  TAG00324c();
  //ClearMemory(LoadPnt((ui8 *)&e.PhysicalBase),32000);
  ClearMemory(e.PhysicalBase,32000);
  AddDialogButton(&dlgButton5050[10], TranslateLanguage("LOAD CHAMPIONS"), 2);
  AddDialogButton(&dlgButton5050[11], TranslateLanguage("SAVE CHAMPIONS"), 2);
  AddDialogButton(&dlgButton5050[12], TranslateLanguage("MAKE NEW ADVENTURE"), 2);
  AddDialogButton(&dlgButton5050[7],  TranslateLanguage("REVERT"), 2);



  // *****   UNDO  button  **********
  pTranslatedText = TranslateLanguage("UNDO");
  textLength = strlen(pTranslatedText);
  dlgButton5050[8].rectPos2.x1 = (i16)(dlgButton5050[8].rectPos2.x2 - 5*textLength - 10);
  AddDialogButton(&dlgButton5050[8],  TranslateLanguage("UNDO"), 2);

  // *****   QUIT  button  **********
  pTranslatedText = TranslateLanguage("QUIT");
  textLength = strlen(pTranslatedText);
  dlgButton5050[16].rectPos2.x1 = (i16)(dlgButton5050[16].rectPos2.x2 - 5*textLength - 10);
  AddDialogButton(&dlgButton5050[16], TranslateLanguage("QUIT"), 2);

  DrawDialogBoxOutline(&wRectPos5162, 3, 3, 0);
  DrawDialogBoxOutline(&wRectPos5146, 2, 2, 0);
  DrawCharacterDetails(_1_, 0, 1);
  //for (D7W = 1; D7W < e.Word24814; D7W++)
  for (D7W = 1; D7W < e.numCharacter_24814; D7W++)
  {
    DrawCharacterHeader(D7W, 0);
//
  };
  DrawSelectedColor(0);
  for (D7W=1; D7W<16; D7W++)
  {
    GetPalleteItemRect(D7W, &rect_8);
    FillScreenRectangle(&rect_8, byte4712[D7W]);
//
//
  };
  TAG003264();
  RETURN;
}

// *********************************************************
//
// *********************************************************
void TAG00439e(void)
{
  dReg D7;
//;;;;;;;;;;;;;;;
  //for (D7W=0; D7W<e.Word24814; D7W++)
  for (D7W=0; D7W<e.numCharacter_24814; D7W++)
  {
    //MemoryMove(LoadPnt(e.Byte5202 + 6*D7W),
    MemoryMove(e.array_5202[D7W].pntr_0,
               (ui8 *)e.Byte4228,
               0x400,
               0x400,
               464);
    //MemoryMove((ui8 *)e.CharacterImages + 464*D7W,
    MemoryMove(e.characterImages_7058[D7W].ubytes,
               //LoadPnt(e.Byte5202 + 6*D7W),
               e.array_5202[D7W].pntr_0,
               0x400,
               0x400,
               464);
    MemoryMove((ui8 *)e.Byte4228,
               //(ui8 *)e.CharacterImages + 464*D7W,
               e.characterImages_7058[D7W].ubytes,
               0x400,
               0x400,
               464);
//
//
  };
}

// *********************************************************
//
// *********************************************************
RESTARTABLE _TAG004430(i32 P1)
{//(void)
  static dReg D7;
  //static aReg A0, A1;
  //static aReg A0;
  ARRAY_5202 *pArray_5202_A0;
  CHARACTER_IMAGE *pCharacterImage_A1;
  CHARACTER_IMAGE *pCharacterImage_A0;
  RESTARTMAP
    RESTART(1)
  END_RESTARTMAP
//;;;;;;;;;;;;;
  e.ActualCharacter = -1;
  for (D7L=0; D7W<4; D7W++)
  {
    //if (D7W < e.Word24814)
    if (D7W < e.numCharacter_24814)
    {
      //A0 = e.Byte5202 + 6*D7W;
      pArray_5202_A0 = &e.array_5202[D7W];
      //StorePnt(A0, LoadPnt((ui8 *)&e.Pnt24812) + 800*D7W + 1108);
      pArray_5202_A0->pntr_0 = e.pGames_24812->portrait_0772[D7W].pixels_336;
      pCharacterImage_A1 = &e.characterImages_7058[D7W];
      //MemoryMove(LoadPnt(A0), (ui8 *)A1, 0x400, 0x400, 464);
      MemoryMove(pArray_5202_A0->pntr_0, pCharacterImage_A1, 0x400, 0x400, 464);
    }
    else
    {
      //A0 = e.Byte5202 + 6*D7W;
      pArray_5202_A0 = &e.array_5202[D7W];
      //StoreLong(A0, 0);
      pArray_5202_A0->pntr_0 =  NULL;
      //A0 = e.CharacterImages + 464*D7W;
      pCharacterImage_A0 = &e.characterImages_7058[D7W];;
      //ClearMemory((ui8 *)A0, 464);
      ClearMemory(pCharacterImage_A0->ubytes, 464);
    };
    //A0 = e.Byte5202 + 6*D7W;
    pArray_5202_A0 = &e.array_5202[D7W];
    pArray_5202_A0->ubyte_4 = 0;
//
  };
  if (P1 == 0) RETURN;
  //if (e.Word24814 != 0)
  if (e.numCharacter_24814 != 0)
  {
    DrawCharacterDetails(_1_, 0, 1);
  }
  else
  {
    DrawCharacterHeader(0, 0);
  };
  for (D7W=1; D7W<4; D7W++)
  {
    DrawCharacterHeader(D7W, 0);
//
//
  };
  RETURN;
}


// *********************************************************
//
// *********************************************************
RESTARTABLE _TAG004518(void)
{//(void)
  static dReg D0;
  static unsigned char buffer[6];
  RESTARTMAP
    RESTART(1)
    RESTART(2)
    RESTART(3)
    RESTART(4)
    RESTART(5)
    RESTART(6)
    RESTART(7)
    RESTART(8)
    RESTART(9)
    RESTART(10)
  	RESTART(11)
  END_RESTARTMAP
//;;;Lots of something. Read game file.  Fiddle with characters
//;;;WriteGamefile
  e.Word8048 = 0;
  e.Word7062 = -1;
  TAG004430(_6_, 0);
  DrawCharacterEditorScreen(_7_);
  TAG002faa(&s12406_4802);
  do
  {
    TAG00301c(_1_, buffer);
    D0W = i16Result;
    switch (D0W & 255)
    {
    case 1:
    case 2:
    case 3:
    case 4:
	    DrawCharacterDetails( _11_, (D0W & 255)-1,1);
      if (e.Word5178 == 0) RETURN;
      break;
    case 5:
		  {
			  // image size 22x21
			  //i32 mousex=(buffer[2]-156)/4;
			  //i32 mousey=(buffer[4]-60)/4;
			  //i8* start = (i8*)(e.CharacterImages + 464*e.ActualCharacter) + (mousey * 22) + mousex;
			  //(*start) = SelectedColor;
			  // does not work - graphics is stored in planar structure, not chunky!
			  // must rewrite it later
			  //BitBltSquareImage(e.CharacterImages + 464*e.ActualCharacter, &wRectPos5106[e.ActualCharacter], 16, -1);
			  BitBltSquareImage((i8 *)&e.characterImages_7058[e.ActualCharacter], &wRectPos5106[e.ActualCharacter], 16, -1);
			  BitBltSquareImage((i8 *)FrameImage, &wRectPos5106[4], 24, -1);
			  //BitBltSquareImage(e.CharacterImages + 464*e.ActualCharacter, &wRectPos5106[5], 16, 1);
			  BitBltSquareImage((i8 *)&e.characterImages_7058[e.ActualCharacter], &wRectPos5106[5], 16, 1);
			  DrawMagnifiedCharacterImage();
		  // Edit character image
		  }
      break;
    case 6:
      DontDoThis(0x45d0);  goto case17;
      break;
    case 7:
		  // choose color
		  SelectedColor = (unsigned char)((buffer[4]-42)/8);
		  DrawSelectedColor(SelectedColor);
      break;
    case 8:
      //MessageBox(NULL,"I do not plan to implement this function","Error",MB_OK|MB_TASKMODAL);
      DontDoThis(0x4616);  goto case17;
      break;
    case 9:
	  // I have encountered it sometimes
      //DontDoThis(0x46ae);  goto case17;
      break;
    case 11:
      DontDoThis(0x476a); goto case17;
      break;
    case 12:
      DontDoThis(0x4872);  goto case17;
      break;
    case 13: //Make New Adventure
      InvertButton(_8_, &dlgButton5050[12].rectPos2);
      TAG00260e(_2_);
      D0W = i16Result;
      if (D0W != 0)
      {
        //if (wordGear(LoadPnt((ui8 *)&e.Pnt24812)+4112) == 0)
        if (wordGear(&e.pGames_24812[1].word_0000) == 0)
        {
          ReadSavedGame(_3_, 1);
        };
        //if (wordGear(LoadPnt((ui8 *)&e.Pnt24812)+4112) != 0)
        if (wordGear(&e.pGames_24812[1].word_0000) != 0)
        {
         // TAG0026c4(_4_, LoadPnt((ui8 *)&e.Pnt24812) + 4112);
          TAG0026c4(_4_, &e.pGames_24812[1]);
          D0W = i16Result;
          if (D0W != 0)
          {
            TAG00439e();
            TAG002724();
            TAG00439e();
            TAG000fc4(_5_, 1);
          };
        };
      };
      InvertButton(_9_, &dlgButton5050[12].rectPos2);
      break;
    case 14:
		  // Change character name
		  {
		    // First get character name and determine length
        //unsigned char *pBuffer = buffer;
        i16 desired_pos, pos;
		    desired_pos = (i16)(((buffer[2]-17)/6) + 1);
		    //char* name = (char *)LoadPnt((ui8 *)&e.Pnt24812) + 800*e.ActualCharacter + 772;
		    char* name = (char *)&e.pGames_24812[0].portrait_0772[e.ActualCharacter].name_000;
		    // Then get desired carret position and fix it
		    pos = (i16)strlen(name);

		    if(desired_pos > pos) desired_pos = pos;
		    // Draw carret
		    DrawNameTitleCarret(0, desired_pos);
		    break;

		  };
      break;
    case 15:
		  // Change character title
  		{
	    	// First get character name and determine length
		    i16 desired_pos = (i16)(((buffer[2]-17)/6) + 1);
		    i16 pos;
		    //char* title = (char *)LoadPnt((ui8 *)&e.Pnt24812) + 800*e.ActualCharacter + 780;
		    char* title = (char *)&e.pGames_24812->portrait_0772[e.ActualCharacter].title_008;

		    // Then get desired carret position and fix it
		    pos = (i16)strlen(title);
		    if(desired_pos > pos) desired_pos = pos;

		    // Draw carret
		    DrawNameTitleCarret(1,desired_pos);
		    //desired_pos += 32; // I can see if editing name or title 'coz position could not be larger than 32
		  };
      break;
    case 16:
      DontDoThis(0x49c0);  goto case17;
      break;
    case 17:
case17:
      InvertButton(_10_,&dlgButton5050[16].rectPos2);
      RETURN;
    case 10:
    default:
      break;
    }; //case
  } while (e.Word5178 != 0);
  DontDoThis(0x49e0);
  RETURN;
};


// Internal function to make TextOut easier to read
void TAG004d4a(dReg& D2,
               dReg& D3,
               dReg& D4,
               dReg& D5,
               aReg& A2,
               aReg  A3,
               i32 A3inc,
               i32 jA0,
               i32 jA1)
{
  dReg D0, D1, D6, D7;
loop:
  StoreLong(A3+0, LoadLong(A3+0) & LE32(D2L));
  StoreLong(A3+4, LoadLong(A3+4) & LE32(D2L));
  D0L = (UI8)(*A2);
  switch (D5W)
  {
  case 0:
    D0W <<= 11-D3W;
    break;
  case 8:
    D0W >>= D3W-11;
    break;
  case 18:
    D0W <<= 27-D3W;
    break;
  default: DontDoThis(0x4d6e);
  };
  D1H1 = D0H2;
  D1H2 = D0H2;
  //D1W = D0W;
  //SWAP(D1);
  //D1W = D0W;
  //SWAP(D0);
  D0L = ((D0L & 0xffff)<<16) | ((D0L>>16) & 0xffff);
  D6L = 0;
  D7L = 0;
  switch (jA0&0xffff)
  {
  case 1: D6L = D0L; break;
  case 2: D6W = D1W; break;
  case 3: D6L = D1L; break;
  case 4: D7L = D0L; break;
  case 5: D6L = D0L; D7L = D0L; break;
  case 6: D6W = D1W; D7L = D0L; break;
  case 7: D6L = D1L; D7L = D0L; break;
  case 8: D7W = D1W; break;
  case 9: D6L = D0L; D7W = D1W; break;
  case 10: D6W = D1W; D7W = D1W; break;
  case 11: D6L = D1L; D7W = D1W; break;
  case 12: D7L = D1L; break;
  case 13: D6L = D0L; D7L = D1L; break;
  case 14: D6W = D1W; D7L = D1L; break;
  case 15: D6L = D1L; D7L = D1L; break;
  case 0: break;
  default: DontDoThis(0x4db0);
  };
  D0L |= D2L;
  D0L ^= 0xffffffff;
  D0W = 0;
  D1L |= D2L;
  D1L ^= 0xffffffff;
  switch (jA1&0xffff)
  {
  case 1: D6L |= D0L; break;
  case 2: D6L |= D1L; break;
  case 3: D6L |= D1L; break;
  case 4: D7L |= D0L; break;
  case 5: D6L |= D0L; D7L |= D0L; break;
  case 6: D6L |= D1L; D7L |= D0L; break;
  case 7: D6L |= D1L; D7L |= D0L; break;
  case 8: D7L |= D1L; break;
  case 9: D6L |= D0L; D7L |= D1L; break;
  case 10: D6L |= D1L; D7L |= D1L; break;
  case 11: D6L |= D1L; D7L |= D1L; break;
  case 12: D7L |= D1L; break;
  case 13: D6L |= D0L; D7L |= D1L; break;
  case 14: D6L |= D1L; D7L |= D1L; break;
  case 15: D6L |= D1L; D7L |= D1L; break;
  case 0: break;
  default: DontDoThis(0x4de8);
  }
  StoreLong(A3+0, LoadLong(A3+0) | LE32(D6L));
  StoreLong(A3+4, LoadLong(A3+4) | LE32(D7L));
  D4W--;
  if (D4W==0) return;
  A2+=70;
  A3 += A3inc;
  goto loop;
}


// *********************************************************
//
// *********************************************************
//   TAG004bbc
void TextOutB(ui8 *dest,
              i32 destWidth,
              i32 x,
              i32 y,
              i32 color,
              i32 P6,
              const char *text)
{
  dReg D2, D3, D4, D5, D6, D7;
  i32 saveD2;
  aReg A2, A4, saveA2, saveA4;
  const char *A0;
  i32 jA0, jA1;
  A2 = NULL;
  A4  = (aReg)dest;
  D7W = sw(x);
  D6W = sw(y);
  //D5W = P5;
  D4W = sw(P6);
  A0  = text;
  if (*A0 == 0) return;
  A4 += (D6W - 4) * destWidth;
  A4 += (D7W & 0xfff0)>>1;
  D3W = (UI16)(D7W & 15);
  jA0 = color; // 16 colors
  jA1 = D4W;
tag004cce:
  for (;;)
  {
    do
    {
      D2L = 0;
      D2B = *text;
      if (D2W ==0) return;
      text++;
      A2 = Byte1278 + D2W - 32;
      if (D3W > 11) goto tag004d12;
      D4L = 6;
      D2L = 0x07ff07ff;
      D2L >>= D3W;
      D2H1 = D2W;
      D5L = 0;
      TAG004d4a(D2, D3, D4, D5, A2, A4, destWidth, jA0, jA1);
      D3W += 6;
    } while (D3W < 16);
    D3W &= 1;
    A4 += 8;
  };
tag004d12:
  D4L = 6;
  saveA4 = A4 + 8;
  saveA2 = A2;
  D2L = Long954[D3W-12];
  saveD2 = Long938[D3W-12];
  A2 = saveA2;
  D5L = 8;
  TAG004d4a(D2, D3, D4, D5, A2, A4, destWidth, jA0, jA1);
  D2L = saveD2;
  A2 = saveA2;
  A4 = saveA4;;
  D4L = 6;
  D5L = 18;
  TAG004d4a(D2, D3, D4, D5, A2, A4, destWidth, jA0, jA1);

  A4 = saveA4;
  D3W -= 10;
  goto tag004cce;



}

// *********************************************************
//
// *********************************************************
void TextToScreen(i32 xPixel,i32 yPixel,i32 P3,i32 P4,const char *P5)
{
  //TextOutB(LoadPnt((ui8 *)&e.PhysicalBase),
  TextOutB(e.PhysicalBase,
           160,
           xPixel,
           yPixel,
           P3,
           P4,
           P5);
}


// *********************************************************
//
// *********************************************************
i16 TAG004ec0(i32 P1)
{
  aReg A1, A4;
  i16 *wA0;
  i32 i;
//;;;;;;;;;;;;;
  e.Word826[0] = sw(P1);
  ASSERT(P1<111,"P1>=111"); //Else expand table
  A4 = (aReg)data004f1c + 3 * (P1-10);
  for (i=1; i<5; i++)
  {
    wA0 = &e.Word826[i];
    A1 = A4;
    A4 += 1;
    *wA0 = (UI16)(*A1 & 0xff);
//
//
  };
  //TAG004ea2(Loadpi16(&e.Pnt714[6]));
  TAG004ea2(e.Pnt714[6]);
  return e.Word750[0];
//
//
//file = chaos.ftl      offset =6a48
//
}

// *********************************************************
//
// *********************************************************
i16 TAG00507e(void)
{
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  //Storepi16(&e.Pnt714[0], e.Word826);
  e.Pnt714[0] = e.Word826;
  //Storepi16(&e.Pnt714[1], e.Word816);
  e.Pnt714[1] = e.Word816;
  //Storepi16(&e.Pnt714[2], e.Word784);
  e.Pnt714[2] = e.Word784;
  //Storepi16(&e.Pnt714[3], e.Word750);
  e.Pnt714[3] = e.Word750;
  //Storepi16(&e.Pnt714[4], e.Word734);
  e.Pnt714[4] = e.Word734;
  //Storepi16(&e.Pnt714[5], e.Word722);
  e.Pnt714[5] = e.Word722;
  //Storepi16(&e.Pnt714[6], (i16 *)e.Pnt714);
  e.Pnt714[6] = (i16 *)e.Pnt714;
  TAG004ec0(10);
  e.Word686 = e.Word750[0];
  return e.Word686;
}

// *********************************************************
//
// *********************************************************
i16 TAG0050d2(void)
{
  TAG004ec0(19);
  return 1;
}

// *********************************************************
//
// *********************************************************
i16 TAG005868(i16 *P1, i16 *P2, i16 *P3, i16 *P4)
{
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  TAG004ec0(77);
  *P1 = e.Word750[1];
  *P2 = e.Word750[2];
  *P3 = e.Word750[3];
  *P4 = e.Word750[4];
  return e.Word750[0];
}

// *********************************************************
//
// *********************************************************
void TAG00589e(i16 P1, i16 P2)
{
//;;;;;;;;;;;;;;;;
  e.Word784[0] = P1;
  e.Word734[0] = P2;
  TAG004ec0(78);
}

// *********************************************************
//
// *********************************************************
void TAG005c92(char  *P1)
{ //Splits an address into two words.
//#ifdef imp5c92
  //dReg D0;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;
  //D0L = (i32)P1 >> 16;
  //e.Word9108 = D0W;
  //e.Word9106 = (UI16)((i32)P1 & 0xffff);
  e.Word9106 = (uintptr_t)P1;
  e.Word9108 = (uintptr_t)0;
//#else
//  NotImplemented(0x5c92);
//#endif
}

// *********************************************************
//
// *********************************************************
void TAG005cd6(char  * * P1)
{ //Reassemble a pointer from two words.
//#ifdef imp5cd6
  //dReg D0;
//;;;;;;;;;;;;;;;;
  //D0L = e.Word9104 << 16;
  //D0W = e.Word9102;
  //StoreLong((pnt)P1, D0L);
  *P1 = (char *)e.Word9104;
  //*P1 = (pnt)D0L;
//#else
//  NotImplemented(0x5cd6);
//#endif
}

// *********************************************************
//
// *********************************************************
void TAG005cf6(void)
{
//;;;;;;;;;;;;;;;;;;;;;;;;;
  //Trap #2 with parameter 115 does do something on the
  //Atari.  I don't know what.
  HopefullyNotNeeded(0xcd8a);


}

// *********************************************************
//
// *********************************************************
void TAG005d12(i16 *,i16 *,i16 *)
{
  DontDoThis(0x5d12);
}

// *********************************************************
//
// *********************************************************
void TAG005d8e(i16 *P1, i16 *P2, i16 *P3)
{
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  //Storepi16(&e.pw664, P1);
  e.pw664 = P1;
  //Storepi16(&e.pw656, P3);
  e.pw656 = P3;
  //Storepi16(&e.pw652, P3 + 45);
  e.pw652 = P3 + 45;
  e.Word9122 = 100;
  e.Word9120 = 0;
  e.Word9116 = 11;
  e.Word9110 = *P2;
  TAG005cf6();
  *P2 = e.Word9110;
  //Storepi16(&e.pw664, &e.Word9098);
  e.pw664 = &e.Word9098;
  //Storepi16(&e.pw656, e.Word8842);
  e.pw656 = e.Word8842;
  //Storepi16(&e.pw652, e.Word8330);
  e.pw652 = e.Word8330;
  // never referenced//e.Pnt660 = e.Byte8586;
}

// *********************************************************
//
// *********************************************************
void TAG005df2(i16 P1)
{
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  e.Word9122 = 101;
  e.Word9120 = 0;
  e.Word9116 = 0;
  e.Word9110 = P1;
  TAG005cf6();
}

// *********************************************************
//
// *********************************************************
void TAG0061ba(i16 P1, i16 P2)
{
//;;;;;;;;;;;;;;;;
  e.Word9098 = P2;
  e.Word9122 = 122;
  e.Word9120 = 0;
  e.Word9116 = 1;
  e.Word9110 = P1;
  TAG005cf6();
}

// *********************************************************
//
// *********************************************************
void TAG0061e2(i16 P1)
{
//;;;;;;;;;;;;;;;;;;;
  e.Word9122 = 123;
  e.Word9120 = 0;
  e.Word9116 = 0;
  e.Word9110 = P1;
  TAG005cf6();
}

// *********************************************************
//
// *********************************************************
void TAG006202(i16 P1, i16 *P2, i16 *P3, i16 *P4)
{
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  e.Word9122 = 124;
  e.Word9120 = 0;
  e.Word9116 = 0;
  e.Word9110 = P1;
  TAG005cf6();
  *P2 = e.Word8842[0];
  *P3 = e.Word8330[0];
  *P4 = e.Word8328[0];
}

// *********************************************************
//
// *********************************************************
void TAG00623a(i16 P1, char *P2, char **P3)
{
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  TAG005c92(P2);
  e.Word9122 = 125;
  e.Word9120 = 0;
  e.Word9116 = 0;
  e.Word9110 = P1;
  TAG005cf6();
  TAG005cd6(P3);
}

// *********************************************************
//
// *********************************************************
void TAG00626e(i16 P1, char *P2, char * *P3)
{
//;;;;;;;;;;;;;;;;;;
  TAG005c92(P2);
  e.Word9122 = 126;
  e.Word9120 = 0;
  e.Word9116 = 0;
  e.Word9110 = P1;
  TAG005cf6();
  TAG005cd6(P3);
}

// *********************************************************
//
// *********************************************************
//  TAG0064b4
pnt FormatInteger(pnt rslt, ui32 num, i16 hex)
{
  dReg D5, D6;
  aReg A3, A4;
  static char digits[] = "0123456789ABCDEF";
  char b_12[12];
  A4 = rslt;
  D6L = 0;
  A3 = (aReg)b_12;
  D5W = (UI16)(hex ? 16 : 10);
  do
  {
    *(A3++) = digits[num % D5W];
    num /= D5W;
    D6W++;
  } while (num != 0);
  while ((D6W--) != 0)
  {
    *(A4++) = *(--A3);
  };
  return A4;
}


// *********************************************************
//
// *********************************************************
//  TAG0065e0
i32 atari_sprintf(char *dst, const char* fmt, const intptr_t v1, const intptr_t v2, const intptr_t v3)
{ // returns length of result
  dReg D5;
  aReg A2, A3, A4;
  bool fmtSpec, lSpec, uSpec, hex;
  pnt pnt_8;
  //pnt pnt_4;
  const intptr_t *pnt_4;
  //i32 stack[]={V1,V2,V3};
  const intptr_t stack[]={v1,v2,v3};
  //pnt_4=(pnt)stack;
  pnt_4=stack;
//;;;;;;;;;;;;;;;;;;
  A4 = (aReg)dst;
  pnt_8 = A4;
  A3 = (aReg)fmt;
  //pnt_4 = (pnt)&V1;
  while (*A3 != 0)
  {
    if (*A3 != '%')
    {
      *(A4++) = *(A3++);
      continue; // That was rather the easier case.
    };

    A3++;
    fmtSpec=true;
    lSpec = false;
    uSpec = false;
    while (fmtSpec)
    {
      switch (*(A3++))
      {
      case 'l':
        lSpec = true;
        break;
      case 'u':
        uSpec = true;
        break;
      case 'd':
      case 'x':
        hex = (*A3 == 'x');
        if (lSpec)
        {
          //D5L = LoadLong(pnt_4);//next value
          D5L = (i32)*pnt_4;   //next value
          //pnt_4 += 4;
          pnt_4 ++;
          if ((!uSpec) && (!hex))
          {
            if (D5L & 0x80000000)
            {
              *(A4++) = '-';
              D5L = -D5L;
            }
          };
        }
        else
        {
          //D5L = LoadLong(pnt_4) & 0xffff;
          D5L = (i32)*pnt_4 & 0xffff;
          pnt_4+=4;
          if ((!uSpec) && (!hex))
          {
            if (D5W & 0x8000)
            {
              *(A4++) = '-';
              D5W = sw(-D5W);
            };
          };
        };
        A4 = FormatInteger(A4, D5L, hex);
        fmtSpec = false;
        break;
      case 's':
        //A2 = (aReg)LoadPnt(pnt_4);
        //A2 = (aReg)pnt_4;
        A2 = (aReg)pnt_4[0];
        //pnt_4 += 4;
        pnt_4 ++;
        while (*A2 != 0)
        {
          *(A4++) = *(A2++);
        };
        fmtSpec = false;
        break;
      default:
        *(A4++) = *A3;
        fmtSpec = false;
      }; //case
    }; //while (fmtSpec)
//
//
  };
  *A4 = 0;
  return (i32)(A4-pnt_8);
}

// *********************************************************
//
// *********************************************************
//  TAG006700
i32 GetPixel(pnt P1,i16 x,i16 y,i16 width)
{
  dReg D0, D1, D2, D3, D5;
  aReg A0;
//;;;;;;;;;;;;;;;;;;
  D1W = x;
  D2W = (UI16)(D1W & 15);
  A0 = P1;
  D0L = y * width;
  D1W = (UI16)((D1W / 2) & 0xf8);
  D0W = sw(D0W + D1W);
  A0 += D0W;
  D5W = sw(15 - D2W);
  D0W = (UI16)(((LE16(wordGear(A0+0))>>D5W)<<0) & 1);
  D1W = (UI16)(((LE16(wordGear(A0+2))>>D5W)<<1) & 2);
  D2W = (UI16)(((LE16(wordGear(A0+4))>>D5W)<<2) & 4);
  D3W = (UI16)(((LE16(wordGear(A0+6))>>D5W)<<3) & 8);
  return D3W | D2W | D1W | D0W;
}

// *********************************************************
//
// *********************************************************
//ui8 *TAG0073b0(ui8 *P1)
//OBJHEADER *TAG0073b0(void ** /*P1*/)
OBJHEADER *ObjHandle2ObjHeader(void ** P1)
{
//#ifdef imp73b0
  dReg D0;
  //aReg A0, A4;
  ui8 *A0, *A4;
//;;;;;;;;;;;;;;;;;;;;;;
  //A0 = (aReg)LoadPnt(P1);
  //A0 = (aReg)P1;      // The object data
//#ifdef imp73b0
  A0 = (ui8 *)(*P1);      // The object data
//#else
//  NotImplemented(0x73b0);
//#endif
  A4 = A0 - 2;   // Pointer to preamble size
  D0L = 0;
  D0W = wordGear(A4);  // Preamble Size
  //D0L += 12;
  D0L += sizeof(OBJHEADER);
  return (OBJHEADER *)((ui8 *)A4 - D0L);
//#else
//  NotImplemented(0x73b0);
//  return 0;
//#endif
}

// *********************************************************
//
// *********************************************************
//ui8 *TAG0073d4(pnt /*P1*/)
//STRUCT422 *TAG0073d4(pnt /*P1*/)
//STRUCT422 *TAG0073d4(OBJHEADER *P1)
STRUCT422 *LocateContainingHeap(OBJHEADER *P1)
{
  dReg D7;
  //aReg A3, A4;
  OBJHEADER *A4;
  STRUCT422 *A3;
//;;;;;;;;;;;;;;;;;;;
  A4 = P1;
//#ifdef imp73d4
  //for (D7L=0, A3=(pnt)&e.pDBank422;
  for (D7L=0, A3=&e.struct422;
       D7W<e.numHeap_424;
       //D7W++, A3+=18)
       D7W++, A3++)
  {
    //if ((ui8 *)A4 >= LoadPnt(A3))
    if ((pnt)A4 >= A3->pointer_0)
    {
      //if ((ui8 *)A4 <= LoadPnt(A3) + LoadLong(A3+6)) break;
      if ((pnt)A4 <= A3->pointer_0 + A3->DBankLen_6) break;
    };
//
//
  };
  if (D7W == e.numHeap_424) A3=NULL;
  //return (ui8 *)A3;
  return A3;
//#else
//  NotImplemented(0x73d4);
//  return NULL;
//#endif
}

// *********************************************************
//
// *********************************************************
//void TAG007416(char * /*P1*/,char * /*P2*/)
//void TAG007416(STRUCT422A *P1,STRUCT422 *P2)
void DechainMemBlock_7416(STRUCT422A *P1,STRUCT422 *P2)
{
//#ifdef imp7416
  //aReg A2, A3, A4;
  STRUCT422A *A4;
  STRUCT422A *A2;
  STRUCT422A *A3;
//;;;;;;;;;;
  //A4 = (aReg)P1;
  A4 = P1;
  //A2 = (aReg)LoadPnt(A4+4);
  A2 = A4->pointer_4;
  //A3 = (aReg)LoadPnt(A4+8);
  A3 = A4->pointer_8;
  if (A2 != NULL)
  {
    //StorePnt(A2+8, A3);
    A2->pointer_8 = A3;
  }
  else
  {
    //StorePnt((ui8 *)P2+10, (ui8 *)A3);
    P2->pointer_10 = A3;
  };
  //if (A3 != NULL) StorePnt(A3+4, A2);
  if (A3 != NULL) A3->pointer_4 = A2;
//#else
//  NotImplemented(0x7416);
//#endif
}

// *********************************************************
//
// *********************************************************
//void TAG00744c(char *P1,char * /*P2*/)
//void TAG00744c(char *P1, STRUCT422 *P2)
//void TAG00744c(STRUCT422A *P1, STRUCT422 *P2)
void ChainBlock(STRUCT422A *P1, STRUCT422 *P2)
{
  dReg D7;
  //aReg A2, A3, A4;
  STRUCT422A *A2;
  STRUCT422A *A3;
  STRUCT422A *A4;
//;;;;;;;;;;;;;;;;;;;;;;;;;;
  //A4 = (aReg)P1;
  A4 = (STRUCT422A *)P1;
  //A3 = (aReg)LoadPnt((ui8 *)P2+10);
  A3 = (STRUCT422A *)P2->pointer_10;
  A2 = NULL;
  //D7L = LoadLong(A4) & 0x7fffffff;
  D7L = A4->long_0 & 0x7fffffff;
  while (A3 != NULL)
  {
//#ifdef imp744c
    //if (D7L <= (LoadLong(A3) & 0x7fffffff))
    if (D7L <= (A3->long_0 & 0x7fffffff))
    A2 = A3;
    //A3 = (aReg)LoadPnt(A3+8);
    A3 = A3->pointer_8;
//#else
//    NotImplemented(0x744c);
//#endif
//
  };
  if (A2 != NULL)
  {
    //StorePnt(A2+8, A4);
    A2->pointer_8 = A4;
  }
  else
  {
    //StorePnt((ui8 *)P2+10, (ui8 *)A4);
    P2->pointer_10 = A4;
  };
//#ifdef imp744c
  if (A3 != NULL)
  {
    //StorePnt(A3+4, A4);
    A3->pointer_4 = A4;
  };
  //StorePnt(A4+8, A3);
  A4->pointer_8 = A3;
  //StorePnt(A4+4, A2);
  A4->pointer_4 = A2;
//#else
//  NotImplemented(0x744c);
//#endif
}

//void TAG0074ae(char *P1,i32 P2,char *P3)
//void TAG0074ae(char *P1,i32 P2,STRUCT422 *P3)
void TAG0074ae(STRUCT422A *P1,i32 P2,STRUCT422 *P3)
{
  dReg D6, D7;
  //aReg A3, A4;
  STRUCT422A *A4;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  //A4 = (aReg)P1;
  A4 = P1;
  D7L = P2;
  //A3 = (aReg)P3;
  D6L = D7L | 0x80000000;
  //StoreLong(A4, D6L);
  A4->long_0 = D6L;
  //StoreLong(A4+D7L-4, D6L);
  *(ui32*)((ui8 *)A4+D7L-4) = D6L;
  //TAG00744c((char *)A4, (char *)A3);
  ChainBlock(A4, P3);
}

// *********************************************************
//
// *********************************************************
//void TAG0074ea(pnt P1, i32 P2, pnt P3)
void TAG0074ea(pnt P1, i32 P2, STRUCT422 *P3)
{
  dReg D6, D7;
  //aReg A0, A3, A4;
  aReg A0;
  STRUCT422 *A3;
  STRUCT422A *A4;
//;;;;;;;;;;;;;;
  A4 = (STRUCT422A *)P1;
  D7L = P2;
  A3 = P3;
  //D6L = LoadLong(A4-4);
  D6L = *(ui32 *)((ui8 *)A4-4);  // Length of preceeding block
  if (D6L & 0x80000000)
  {
    D6L &= 0x7fffffff;
    //A4 -= D6L;
    A4 =  (STRUCT422A *)((ui8 *)A4 - D6L);
    //TAG007416((char *)A4, (char *)A3);
    DechainMemBlock_7416(A4, A3);
    D7L += D6L;
  };
  A0 = (aReg)A4 + D7L;
  //D6L = LoadLong(A0);
  D6L = *(ui32 *)A0;   // Length of following block
  if (D6L & 0x80000000)
  {
    D6L &= 0x7fffffff;
    //TAG007416((char *)A4 + D7L, (char *)A3);
    DechainMemBlock_7416((STRUCT422A *)((ui8 *)A4 + D7L), A3);
    D7L += D6L;
  };
  //TAG0074ae((char *)A4, D7L, (char *)A3);
  //TAG0074ae((char *)A4, D7L, A3);
  TAG0074ae(A4, D7L, A3);
}

// *********************************************************
//
// *********************************************************
//ui8 *TAG007552(i32 P1, ui8 * P2, i16 *P3)
//STRUCT422A *TAG007552(i32 P1, STRUCT422 *P2, i16 *P3)
STRUCT422A *BestBlock(i32 P1, STRUCT422 *P2, i16 *P3)
{
//#ifdef imp7552
  dReg D6, D7;
  //aReg A2, A3, A4;
  STRUCT422A *A2;
  STRUCT422A *A3;
  STRUCT422 *A4;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  D7L = P1;
  //A4 = (aReg)P2;
  A4 = P2;
  D7L |= 0x80000000;
  //A3 = (aReg)LoadPnt(A4+10);
  A3 = A4->pointer_10;
  A2 = NULL;
  while (A3 != NULL)
  {
    //D6L = LoadLong(A3);
    D6L = A3->long_0;
    if (D6L < D7L) break;
    if (D6L - D7L <= 34) A2 = A3;
    //A3 = (aReg)LoadPnt(A3+8);
    A3 = A3->pointer_8;
//
  };
  if (A2 == NULL)
  {
    //A3 = (aReg)LoadPnt(A4+10);
    A3 = A4->pointer_10;
    //if (D7L > LoadLong(A3)) return (ui8 *)A2;
    if (D7L > A3->long_0) return A2;
    A2 = A3;
    *P3 = 0;
  }
  else
  {
    *P3 = 1;
  };
  return A2;
//#else
//  NotImplemented(0x7552);
//  return NULL;
//#endif
}

// *********************************************************
//
// *********************************************************
i16 TAG007904(char *,i32)
{
  DontDoThis(0x7904); return 0;
}

//ui8 *TAG007a1a(i16 P1)
HANDLELIST *NewHandleList(i16 P1)
{
  dReg D6, D7;
  //aReg A2, A3, A4;
  STRUCT422 *A2;
  HANDLELIST *A4;
  HANDLELIST *A3;
  STRUCT6  s6_12;
//;;;;;;;;;;;;;;;;;;;;;;
  D7W = P1;
  A4 = NULL;
  //D6L = 4 * D7UW + 20;
  D6L = sizeof(pnt) * D7UW + sizeof(HANDLELIST) + sizeof(ui32);
  //A3 = (aReg)LoadPnt((ui8 *)&e.Pnt430);
  //A3 = e.Pnt430;
  A3 = e.objList430;
  s6_12.word4 = 1024;
  s6_12.int0 = D6L;
  A2 = TAG007bba(&s6_12);
//#ifdef imp7a1a
  if (A2 != NULL)
  {
    //A4 = (aReg)TAG007cc6(D6L, 1024, (char *)A2);
    //A4 = (aReg)TAG007cc6(D6L, 1024, A2);
    A4 = (HANDLELIST *)BlockAlloc(D6L, 1024, A2);
    if (A4 != NULL)
    {
      //A4[4] = -1;
      A4->byte_4 = 0xff;
      //A4[5] = -128;
      A4->byte_5 = 0x80;
      //StoreLong(A4+6, 0);
      A4->next_6 = NULL;
      //wordGear(A4+10) = D7W;
      A4->maxHandles_10 = D7W;
      //wordGear(A4+12) = 0;
      A4->numHandles_12 = 0;
      //wordGear(A4+14) = 0;
      A4->searchStartIndex_14 = 0;
      //ClearMemory((ui8 *)A4+16, D6L-20);
      ClearMemory((ui8 *)A4+sizeof(HANDLELIST), D6L-sizeof(HANDLELIST)-sizeof(ui32));
//#ifdef imp7a1a
      if (A3 != NULL)
      {
        while (LoadLong(A3+6) != 0)
        {
          //A3 = (aReg)LoadPnt(A3+6);
          A3 = A3->next_6;
        };
        //StorePnt(A3+6, A4);
        A3->next_6 = A4;
      }
      else
      {
        //StorePnt((ui8 *)&e.Pnt430, A4);
        e.objList430 = A4;
      };
//#else
//      NotImplemented(0x7a1a);
//#endif
    };
  };
  return A4;
//#else
//  NotImplemented(0x7a1a);
//  return 0;
//#endif
}

// *********************************************************
//
// *********************************************************
//char *TAG007ac0(pnt * P1)
void **TAG007ac0(HANDLELIST **P1)
{
  dReg D0, D5, D6, D7;
  //aReg A2, A3;
  ui8 *A3;
  HANDLELIST *A2;
  //pnt  *pA4;
  HANDLELIST  **pA4;
  A3 = (ui8 *)0xbaddade; //debugging
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  pA4 = P1;
  D5L = 0;
  //for (A2 = (aReg)LoadPnt((ui8 *)&e.Pnt430); A2 != NULL; A2=(aReg)LoadPnt(A2+6))
  for (A2 = e.objList430; A2 != NULL; A2=A2->next_6)
  {
//#ifdef imp7ac0
    //if (wordGear(A2+12) >= wordGear(A2+10)) continue;
    if (A2->numHandles_12 >= A2->maxHandles_10) continue;
    //D6W = wordGear(A2+14);
    D6W = A2->searchStartIndex_14;
    //D0L = (D6UW << 2) & 0xffff;
    D0L = (D6UW *sizeof(pnt)) & 0xffff;
//#ifdef imp7ac0
    //A3 = (ui8 *)A2 + D0L + 16;
    A3 = (ui8 *)A2 + D0L + sizeof(HANDLELIST);
    //for (D7W = wordGear(A2+10); (D7W--)!=0; )
    for (D7W = A2->maxHandles_10; (D7W--)!=0; )
    {
      //if (wordGear(A2+10) == D6W)
      if (A2->maxHandles_10 == D6W)
      {
        D6L = 0;
        //A3 = (ui8 *)A2 + 16;
        A3 = (ui8 *)A2 + sizeof(HANDLELIST);
      };
      //if (LoadPnt(A3) == NULL)
      if (*((ui8 **)A3) == NULL)
      {
        //wordGear(A2+14) = D6W;
        A2->searchStartIndex_14 = D6W;
        //wordGear(A2+12)++;
        A2->numHandles_12++;
        D5W = 1;
        break;
      };
      D6W++;
      A3 += 4;
//
    };
    if (D5W) break;
//
//
//#else
//    NotImplemented(0x7ac0);
//#endif
  };
//#ifdef imp7ac0
  if (D5W == 0)
  {
    A2 = NewHandleList(e.Word426);
    if (A2 != NULL)
    {
      //A3 = A2 + 16;
      A3 = (ui8 *)A2 + sizeof(HANDLELIST);
      //wordGear(A2+12)++;
      A2->numHandles_12++;
      D5W = 1;
    };
  };
  if (D5W == 0) A3 = NULL;
  *pA4 = A2;
  ASSERT((pnt)A3 != (pnt)0xbaddade,"A3 not initialized");
//#else
//  NotImplemented(0x7ac0);
//  return 0;
//#endif
  return (void **)A3;
}

// *********************************************************
//
// *********************************************************
//void TAG007b68(void **P1)
void RemoveHandle(void **P1)
{
//#ifdef imp7b68
  dReg D0;
  //aReg A0, A3, A4;
  aReg A0;
  void **A4;
  HANDLELIST *A3;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  A4 = P1;
  //for (A3=(aReg)LoadPnt((ui8 *)&e.Pnt430); A3!=NULL; A3=(aReg)LoadPnt(A3+6))
  for (A3=e.objList430; A3!=NULL; A3=A3->next_6)
  {
    if ((ui8 *)A4 < (ui8 *)A3) continue;
    //D0L = 4 * (UI16)(wordGear(A3+10));
    D0L =  A3->maxHandles_10 * sizeof(void *);
    //A0 = A3 + D0L + 20;
    A0 = (aReg)A3 + D0L + sizeof(HANDLELIST) + sizeof(ui32);
    if ((aReg)A4 >= A0) continue;
    //StoreLong(A4, 0);
    *A4 = NULL;
    //wordGear(A3+12)--;
    A3->numHandles_12--;
    //D0L = A4 - A3;
    D0L = (ui32)((ui8 *)A4 - (ui8 *)A3);
    //D0L -= 16;
    D0L -= sizeof(HANDLELIST);
    //D0L /= 4;
    D0L /= sizeof (void *);
    //wordGear(A3+14) = D0W;
    A3->searchStartIndex_14 = D0W;
    break;
//
//
  };
//#else
//  NotImplemented(0x7b68);
//#endif
}

// *********************************************************
//
// *********************************************************
//char *TAG007bba(STRUCT6 *P1)
STRUCT422 *TAG007bba(STRUCT6 *P1)
{
  dReg D0, D1, D3, D4, D5, D6, D7;
  //aReg A2, A3;
  STRUCT422 *A3;
  STRUCT422 *A2;
  STRUCT6 *pS6_A4;
//;;;;;;;;;;;;;;;;
  pS6_A4 = P1;
  if (pS6_A4->word4 & 0x01)
  {
    D7L = 1;
    //A3 = (aReg)&e.pDBank422;
    A3 = &e.struct422;
  }
  else
  {
    D7L = -1;
    //A3 = (pnt)&e.pDBank422 + 18*(e.Word424-1);
    A3 = &e.struct422 + e.numHeap_424 - 1;
  };
  D5L = pS6_A4->int0;
  A2 = A3;
  for (;;)
  {
    D4L = 0;
    D3L = 0;
    //for (D6L=0; D6W<e.Word424; D6W++, A3+=18*D7W)
    for (D6L=0; D6W<e.numHeap_424; D6W++, A3++)
    {
      //D0W = sw(wordGear(A3+4) & pS6_A4->word4);
      D0W = sw(A3->Word_4 & pS6_A4->word4);
      D1W = (UI16)(pS6_A4->word4 & 0x7f8);
      if (D0W != D1W) continue;
      D4L = 1;
      //D0L = LoadLong(LoadPnt(A3+10)) & 0x7fffffff;
      D0L = A3->pointer_10->long_0 & 0x7fffffff;
      if (D0L >= D5L)
      {
        D3L = 1;
        break;
      };
//

    }; /// for (D6W)
//#ifdef imp7bba
    if (D4W == 0)
    {
      TAG008788(1, 0x010d0005, pS6_A4->word4 & 0xffff);
    };
    if (D3W == 0)
    {
      NotImplemented(0x7bbc);
      A3 = A2;
      for (D6L=0; D6W<e.numHeap_424; D6W++, A3+=18*D7W)
      {
        D0W = sw(wordGear(A3+4) & pS6_A4->word4);
        D1W = (UI16)(pS6_A4->word4 & 0x07f8);
        if (D0W != D1W) continue;
        if (TAG007904((char *)A3, D5L) == 0) break;
//
//
      };
      if (e.numHeap_424 == 0)
      {
        D0L = TAG008788(0, 0x010d0001, pS6_A4->word4 & 0xffff);
        if (D0L != 0x010d0001)
        {
          A3 = A2;
          continue;
        };
      };
    };
//#else
//  NotImplemented(0x7bba);
//  return 0;
//#endif
    //return (char *)A3;
    return A3;
  } ;
}

// *********************************************************
//
// *********************************************************
//ui8 *TAG007cc6(i32 P1, i16 P2, char *P3)
//ui8 *TAG007cc6(i32 P1, i16 P2, STRUCT422 *P3)
ui8 *BlockAlloc(i32 P1, i16 P2, STRUCT422 *P3)
{
  dReg D5, D6, D7;
  //aReg A0, A2, A3, A4;
  //aReg A0;
  ui8 *A0;
  STRUCT422 *A4;
  STRUCT422A *A2, *A3;
  i16 w_2;
//;;;;;;;;;;;;;;;;;;;;;
  D7L = P1;
  D6W = P2;
  //A4 = (aReg)P3;
  A4 = P3;
  //A3 = (aReg)TAG007552(D7L, (ui8 *)A4, &w_2);
  A3 = BestBlock(D7L, A4, &w_2);
  if (A3 != NULL)
  {
    //TAG007416((char *)A3, (char *)A4);
    DechainMemBlock_7416(A3, A4);
    //D5L = LoadLong(A3) & 0x7fffffff;
    D5L = A3->long_0 & 0x7fffffff;
    if (w_2 != 0)
    {
      D7L = D5L;
    }
    else
    {
      D5L -= D7L;       // What will remain on original block
      if (D6L & 1)      // Allocate front or bank?
      { // Allocate front
        //A2 = A3 + D7L;
        A2 = (STRUCT422A *)((ui8 *)A3 + D7L);  // Old block at end of old block
      }
      else
      {  // Allocate back
        A2 = A3;    // Old Block at front of old block
        //A3 += D5L;
        A3 = (STRUCT422A *)((ui8 *)A3 + D5L);  // New block allocated at end of old block
      };
      //TAG0074ae((char *)A2, D5L, (char *)A4);
//#ifdef imp7cc6
      TAG0074ae(A2, D5L, A4);  // Return the original (shorter) block to the chain
//#else
//    NotImplemented(0x7cc6);
//#endif
    };
    //StoreLong(A4+14, LoadLong(A4+14) - D7L);
    A4->bytesRemaining_14 = A4->bytesRemaining_14 - D7L;
    //StoreLong(A3, D7L);
    A3->long_0 = D7L;
    //A0 = A3 + D7L;
    A0 = (ui8 *)A3 + D7L;
    //StoreLong(A0-4, D7L);
    *(ui32 *)(A0-4) = D7L;
  };
  return (ui8 *)A3;
}


// *********************************************************
//
// *********************************************************
//ui8 *TAG007d4c(STRUCT12 *P1)
void **TAG007d4c(STRUCT12 *P1)
{
  dReg D0, D7;
  //aReg A0, A2, A3;
  aReg A0;
  void **A2;
  OBJHEADER *A3;
  ui8 *preamble_A3;
  STRUCT12 *pS12_A4;
  //pnt   pnt_8;
  HANDLELIST *pnt_8;
  //char *pnt_4;
  STRUCT422 *pnt_4;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  pS12_A4 = P1;
  D0L = pS12_A4->word6 & 0xffff;
  //D7L = D0L + pS12_A4->long0 + 18;
  //D7L = D0L + pS12_A4->long0 + 18;
  D7L = D0L + pS12_A4->long0 + sizeof(OBJHEADER) + sizeof(ui16) +sizeof(ui32);
  if (D7L & 1) D7L++;
  pS12_A4->long0 = D7L;
  //A2 = (aReg)TAG007ac0(&pnt_8);
  A2 = TAG007ac0(&pnt_8);
  if (A2 == NULL) return NULL;
  pnt_4 = TAG007bba((STRUCT6 *)pS12_A4);
  if (   (pnt_4 != NULL)
      //&& ((A3 = (aReg)TAG007cc6(D7L, pS12_A4->word4, pnt_4))!=NULL) )
      && ((A3 = (OBJHEADER *)BlockAlloc(D7L, pS12_A4->word4, pnt_4))!=NULL))
//#ifdef imp7d4c
  {
    //StorePnt(A2, A3 + (pS12_A4->word6&0xffff) + 14);
    //*A2 = (ui8 *)A3 + (pS12_A4->word6&0xffff) + 14;
    *A2 = (ui8 *)A3 + (pS12_A4->word6&0xffff) + sizeof(OBJHEADER) + sizeof(ui16);
    //A3[4] = 0;
    A3->byte_4 = 0;
    //A3[5] = (UI8)(pS12_A4->word4 & 7);
    A3->byte_5 = (UI8)(pS12_A4->word4 & 7);
    //wordGear(A3+10) = pS12_A4->word4;
    A3->word_10 = pS12_A4->word4;
    //StorePnt(A3+6, A2);
    A3->self_6 = A2;
    //A3 += 12;
    preamble_A3 = (ui8 *)A3 + sizeof(OBJHEADER);

    if (   (pS12_A4->fnc8 != NULL)
        && (pS12_A4->word6 != 0)  )
    {
      //(*pS12_A4->fnc8)(A3);
      (*pS12_A4->fnc8)((pnt)preamble_A3);
    };
    //A0 = A3 + (pS12_A4->word6 & 0xffff);
    //A0 = (aReg)A3 + (pS12_A4->word6 & 0xffff);
    A0 = (aReg)preamble_A3 + (pS12_A4->word6 & 0xffff);
    wordGear(A0) = pS12_A4->word6;
    return A2;
  }
  else
  {
    //wordGear(pnt_8+12)--;
    pnt_8->numHandles_12--;
    return NULL;
  };
//#else
//  NotImplemented(0x7d4c);
//  return NULL;
//#endif
}

// *********************************************************
//
// *********************************************************
//void TAG007e16(ui8 *P1,  ui8 *P2)
void TAG007e16(ui8 *P1,  STRUCT422 *P2)
{
  dReg D7;
  //aReg A3, A4;
  aReg A4;
  STRUCT422 *A3;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  A4 = (aReg)P1;
  //A3 = (aReg)P2;
  A3 = P2;
  D7L = LoadLong(A4);
  //StoreLong(A3+14,LoadLong(A3+14) + D7L);
  A3->bytesRemaining_14 = A3->bytesRemaining_14 + D7L;
  TAG0074ea(A4, D7L, A3);
}

// *********************************************************
//
// *********************************************************
i32 TAG007e42(i16 P1, i16 P2)
{
  dReg D5, D7;
  //aReg A0, A3, A4;
  aReg A0, A4;
  STRUCT422 *A3;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  //D7L = MALLOC(-1);
  D7L = 240000;
  if (D7L < 120000)
  {
    DontDoThis(0x7e62);
  };
  D7L = (D7L & -2) - 4000;
  A4 = (aReg)MALLOC(D7L);
  if (A4==NULL)
  {
    DontDoThis(0x7e96);
  };
  //StorePnt((ui8 *)&e.pAllocDBank434, (ui8 *)A4);
  e.pAllocDBank434 = A4;
  D7L &= -2;
  e.numHeap_424 = 1;
  //StorePnt((ui8 *)&e.pDBank422, (ui8 *)A4);
  //e.pDBank422 = A4;
  e.struct422.pointer_0 = A4;
  //StoreLong((pnt)&e.DBankLen416, D7L);
  //e.DBankLen416 = D7L;
  e.struct422.DBankLen_6 = D7L;
  //e.Word418 = 2040;
  e.struct422.Word_4 = 2040;
  //for (D5L=0, A3=(pnt)&e.pDBank422;
  for (D5L=0, A3=&e.struct422;
       D5W<e.numHeap_424;
       //A3+=18, D5W++)
       A3++, D5W++)
  {
    //A4 = (aReg)LoadPnt(A3);
    A4 = A3->pointer_0;
    //StoreLong(A4, 0);
    *(ui32 *)A4 = 0;
    //A0 = A4 + LoadLong(A3+6);
    A0 = A4 + A3->DBankLen_6;
    //StoreLong(A0-4, 0);
    *((ui32 *)(A0-4)) = 0;
    A4 += 4;
    //StorePnt(A3, A4);
    A3->pointer_0 = A4;
    //StoreLong(A3+6, LoadLong(A3+6) - 8);
    A3->DBankLen_6 = A3->DBankLen_6 - 2*sizeof(ui32);
    //StoreLong(A3+14, LoadLong(A3+6));
    A3->bytesRemaining_14 = A3->DBankLen_6;
    //StoreLong(A3+10, 0);
    A3->pointer_10 = NULL;
    //TAG0074ae((char *)A4, LoadLong(A3+14), (char *)A3);
    //TAG0074ae((char *)A4, A3->long_14, A3);
//#ifdef imp7e42
    TAG0074ae((STRUCT422A *)A4, A3->bytesRemaining_14, A3);
//#else
//    NotImplemented(0x7e43);
//#endif
//
//
  };
  e.Word426 = P2;
  //StorePnt((ui8 *)&e.Pnt430, (ui8 *)NULL);
  //e.Pnt430 = NULL;
  e.objList430 = NULL;
  if (NewHandleList(P1) == NULL)
  {
    DontDoThis(0x7f34);
  };
  return 0;
}

// *********************************************************
//
// *********************************************************
void TAG007f54(void)
{
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  //MFREE(LoadPnt((ui8 *)&e.pAllocDBank434));
  MFREE((ui8 *)e.pAllocDBank434);
}

// *********************************************************
//
// *********************************************************
//ui8 *TAG007f64(i32 P1, i16 P2)
void **TAG007f64(i32 P1, i16 P2)
{
  STRUCT12 s_12;
//;;;;;;;;;;;;;;;;;;;;;;
  s_12.long0 = P1;
  s_12.word4 = P2;
  s_12.word6 = 0;
  s_12.fnc8 = NULL;
  return TAG007d4c(&s_12);
}

// *********************************************************
//
// *********************************************************
//i32 TAG007f92(ui8 *P1)
i32 TAG007f92(void **P1)
{
  dReg D7;
  //aReg A3, A4;
  OBJHEADER *A3;
  void **A4;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  //A4 = (aReg)P1;
  A4 = P1;
  //D7L = TAG0080e0((ui8 *)A4);
  D7L = IsHandleInvalid(A4);
  if (D7L == 0)
  {
//#ifdef imp7f92
    //A3 = (aReg)ObjHandle2ObjHeader((ui8 *)A4);
    A3 = ObjHandle2ObjHeader(A4);
    TAG007e16((ui8 *)A3, LocateContainingHeap(A3));
//#else
//    NotImplemented(0x7f92);
//#endif
  };
  if ( (D7L == 0) || (D7L == 0x010d0003) )
  {
    RemoveHandle(A4);
    D7L = 0;
  };
  return D7L;
}

// *********************************************************
//
// *********************************************************
//ui8 *TAG007fe8(ui8 * /*P1*/)
void *ObjHandle2ObjData(void **P1)
{
//#ifdef imp7fe8
  dReg D7;
  //aReg A2, A3, A4;
  OBJHEADER *A3;
  void *A2;
  void **A4;
//;;;;;;;;;;;;;;
  //A4 = (aReg)P1;
  A4 = P1;
  A2 = NULL;
  //D7L = TAG0080e0((ui8 *)A4);
  D7L = IsHandleInvalid(A4);
  if (D7L == 0)
  {
//#ifdef imp7fe8
    //A3 = (aReg)ObjHandle2ObjHeader((ui8 *)A4);
    A3 = ObjHandle2ObjHeader(A4);
//#else
//    NotImplemented(0x7fe8);
//#endif
    //A3[4]++;
    A3->byte_4++;
    //A2 = (aReg)LoadPnt(A4);
    A2 = *A4;
  };
  return A2;
//#else
//  NotImplemented(0x7fe8);
//  return NULL;
//#endif
}

// *********************************************************
//
// *********************************************************
//void TAG00801e(ui8 * /*P1*/)
void DecrementReferenceCount(void** P1)
{
  //ObjHandle2ObjHeader(P1)[4]--;
  ObjHandle2ObjHeader(P1)->byte_4--;
}

// *********************************************************
//
// *********************************************************
//i32 TAG0080e0(ui8 * /*P1*/)
//i32 TAG0080e0(void **P1)
i32 IsHandleInvalid(void **P1)
{
  dReg D7;
  //aReg A3, A4;
  void **A4;
  OBJHEADER *A3;
  D7L = 0xbaddade;//debugging
//;;;;;;;;
  //A4 = (aReg)P1;
  A4 = P1;
//#ifdef imp80e0
  //if ( (A4 == NULL) || (LoadLong(A4)==0) )
  if ( (A4 == NULL) || (*A4 == NULL) )
  {
    TAG008788(1, 0x010d0002, (uintptr_t)A4);
  }
  else
  {
    //if (LoadLong(A4) == -1)
    if ((uintptr_t)(*A4) == -1)
    {
      D7L = 0x010d0003;
    }
    else
    {
      //A3 = (aReg)ObjHandle2ObjHeader((ui8 *)A4);
      A3 = ObjHandle2ObjHeader(A4);
//#ifdef imp80e0
      if (LocateContainingHeap(A3) == NULL)
      {
        D7L = 0x010d0002;
      }
      else
      {
        D7L = 0;
      };
//#else
//      NotImplemented(0x80e0);
//#endif
    };
  };
  ASSERT(D7L != 0xbaddade,"D7 not initialized");
  return D7L;
//#else
//  NotImplemented(0x80e0);
//  return 0;
//#endif
}

// *********************************************************
//
// *********************************************************
//   TAG00858c
void MemoryMove(void *src,
                void *dst,
                i16 , //garbage
                i16 , //garbage
                i32 len)
{
  MemMove(src, dst, len);
}

// *********************************************************
//
// *********************************************************
i32 TAG008788(i16,i32,uintptr_t)
{
  DontDoThis(0x8788); return 0;
//;;;;;;;;;;;;;



//file = chaos.ftl      offset =a1bc


//file = chaos.ftl      offset =a1dc



}

void EditCharacterName(char key)
{
#ifdef impEditCharacterName
	char* name;
	i32 len;
	i32 max_len;
	i32 position;

	//name = (char *)LoadPnt((ui8 *)&e.Pnt24812) + 800*e.ActualCharacter + 772;
	//name = (char *)((ui8 *)e.pGames_24812) + 800*e.ActualCharacter + 772;
	name = (char *)((ui8 *)&e.pGames_24812->portrait_0772[e.ActualCharacter]);
	if(e.EditBoxSelected == 1) //TITLE
	{
		 name += 8;
		 max_len = 18;
	}
	else
		max_len = 7;
	len = strlen(name);

	// first check for backspace key
	if(key==8)
	{
		if(e.EditBoxPosition == 0)
			return;
		if(e.EditBoxPosition  == len ) // deleting from the end
		{
			name[--len] = 0;
		}
		else // deleting from the middle
		{ // insert in the middle
			i32 i;
			for(i = e.EditBoxPosition;i < max_len;i++)
				name[i-1] = name[i];
			name[len] = 0;
		}
		position = e.EditBoxPosition - 1;
	}
	else
	{
		if(len == max_len) // maximum name lenght
			return;

		if(key!=32)
			key &= ~32; // uppercase
		if ((key!=32) && ((key<'A') || (key>'Z'))) // bad key pressed
			return;

		if(e.EditBoxPosition == len) // appending to the end
			name[len++] = key;
		else
		{ // insert in the middle
			i32 i;
			for(i=max_len;i>=e.EditBoxPosition;i--)
				name[i+1] = name[i];
			name[e.EditBoxPosition] = key;
		}
		position = e.EditBoxPosition + 1;
	}

	if(e.EditBoxSelected == 0)
		FillCharacterNameEditBox(name, 13, 7);
	else
		FillCharacterNameEditBox(name, 14, 19);

	DrawNameTitleCarret(e.EditBoxSelected, (i16)position);
#else
  NotImplemented(0x8e0f);
#endif
}
