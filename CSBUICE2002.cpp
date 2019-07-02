#include "stdafx.h"
#include <Aygshell.h>
#include "resourceCE2002.h"
#include <EXCPT.H>

#ifdef _MSVC_INTEL
#pragma warning(disable:4201) //Allow nameless struct
#include <mmsystem.h>
#pragma warning(default:4201)
#endif


#ifdef _LINUX
# include <sys/time.h>
# include <ctype.h>
# include "CSBTypes.h"
#endif

#include <stdio.h>

#include "UI.h"
//#include "Objects.h"
#include "Dispatch.h"
#include "CSB.h"
#include "Data.h"

void DSAInstrumentation_Dump();
void EnqueMouseClick(i32, i32, i32);
void  TAG001afe(i32, i32, i32);
//i32 AddSD(char *, i32, float, float, float);
void ItemsRemaining(i32 mode);
char *parentFolder(char *folderName, char *endName);
bool PlayfileIsOpen(void);
void MapCoordinate(i32 a, i32 b, i32& x, i32& y);
bool IsPortraitBar(int, int);
void info(char *, unsigned int);
POINT LastTapPoint;
bool ReadCEdisplayData(char *buf, i32& col);
bool IsTextScrollArea(int x, int y);
void GetTimeLocation(i32& x, i32& y);

extern i32 WindowHeight;
extern i32 WindowWidth;
extern i32 WindowX;
extern i32 WindowY;
extern i32 windowPos[4];
extern bool fullscreenRequested;
extern bool virtualFullscreen;
extern HINSTANCE hInst;
extern bool hidingTaskBar;
extern bool hidingMenu;
extern bool hidingTime;
extern RECT defaultWindowRect;
extern HWND g_hwndCB;


extern i32 VBLperTimer;
extern i32 VBLMultiplier;
extern i32 screenSize;
extern i32 keyboardMode;
extern i32 trace;
void PlayfileOpen(bool);
void RecordfileOpen(bool);
void RecordfilePreOpen(void);
extern bool BeginRecordOK;
extern bool TimerTraceActive;
extern bool AttackTraceActive;
extern bool RepeatGame;
extern i32 NoSpeedLimit;
bool OpenTraceFile(void);
void CloseTraceFile(void);
extern bool cursorIsShowing;
extern bool RecordCommandOption;
extern bool NoRecordCommandOption;

extern HWND hWnd;
extern HDC hdc;

#define maxMessageStack 20
CSB_UI_MESSAGE msgStack[maxMessageStack];
i32 msgStackLen = 0;
i32 messageMask = 1; //Right now just 0 or 1

i32 rightButtonX = -1;
i32 rightButtonY = 0;
i32 rightButtonTime = 0;

i32 latestScanp1 = 0;
i32 latestScanp2 = 0;
XLATETYPE latestScanType = TYPEIGNORED;
i32 latestScanXlate = 0;
XLATETYPE latestCharType = TYPEIGNORED;
i32 latestCharp1 = 0;
i32 latestCharXlate = 0;
char playFileName[80] = "Playfile.log";
char *PlayfileName = playFileName;
char dungeonname[80] = "dungeon.dat";
char *dungeonName = dungeonname;
bool PlaybackCommandOption = false;
bool RecordCommandOption = false;
bool NoRecordCommandOption = false;
bool RecordDesignOption = false;
bool DMRulesDesignOption = false;
bool RecordMenuOption = false;
bool NoRecordMenuOption = false;
bool extendedPortraits = false;

i32 keyQueueStart=0; // Next key to process
i32 keyQueueEnd=0;   // Next Empty entry
i32 keyQueueLen=3;
i32 keyQueue[3];

ui8 *allocatedMemoryList = NULL;
ui32 listLength = 0;



void AllocationError(void)
{
  UI_MessageBox("Cannot allocate memory","Fatal error",MESSAGE_OK);
  die(0xdab6);
};


/*
class EditDialog
{
  //Text strings are nul-terminated.
public:
  char *m_initialText;//This will not be free'd.  You are
                      //responsible for it.
  char *m_finalText;  //We will allocated this from the
                      //heap. You are responsible for it.
  EditDialog(void);
  ~EditDialog(void);
  i32 DoModal(void);
};
*/
EditDialog::EditDialog(void)
{
  m_initialText = NULL;
  m_finalText = NULL;
}

EditDialog::~EditDialog(void)
{
}


static char *initialEditText;
static char *finalEditText= NULL;
// Mesage handler for EditDialog

LRESULT CALLBACK EditTextCallback(
                       HWND hDlg, 
                       UINT message, 
                       WPARAM wParam, 
                       LPARAM /*lParam*/)
{
  i32 len;
	switch (message)
	{
		case WM_INITDIALOG:
      {
        TEMPORARY_WIDE wide;
        if (initialEditText == NULL)
        initialEditText = "No Game Information available";
        wide = initialEditText;
        SetDlgItemText(hDlg, IDC_GameInformationEdit, wide.Text());
      };
		return TRUE;

		case WM_COMMAND:
			if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) 
			{
        HWND hEdit = GetDlgItem(hDlg, IDC_GameInformationEdit);
        len = GetWindowTextLength(hEdit);
        if (finalEditText == NULL)
        {
          finalEditText = (char *)UI_malloc(len+1, MALLOC015);
        };
        //GetWindowText(hEdit, finalEditText, len+1);
				EndDialog(hDlg, LOWORD(wParam));
				return TRUE;
			}
			break;
	}
    return FALSE;
}


int EditDialog::DoModal(void)
{
  i32 result;
  bool saveCursorShowing;
  initialEditText = m_initialText;
  saveCursorShowing = cursorIsShowing;
  if (!cursorIsShowing) ShowCursor(true);
  i32 mask = UI_DisableAllMessages();
  result = DialogBox(hInst, MAKEINTRESOURCE(IDD_GameInformation2), hWnd, (DLGPROC)EditTextCallback);
  DWORD errorCode = GetLastError();
  if (!saveCursorShowing) ShowCursor(false);
  UI_EnableMessages(mask);
  m_finalText = finalEditText;
  if (finalEditText != NULL) UI_free(finalEditText);
  finalEditText = NULL;
  initialEditText = NULL;
  return result;
}


class KEYXLATE 
{
private:
  i32 m_numkey;
  i32 *m_xlate;
public:
  KEYXLATE(void)
  {
    m_numkey=0;
    m_xlate=NULL;
  };
  ~KEYXLATE(void)
  {
    if (m_xlate!=NULL) UI_free(m_xlate);
    m_xlate = NULL;
  };
  void addkey(i32 scan, i32 key, i32 mode, XLATETYPE type);
  i32 translate(i32 scan, i32 mode, XLATETYPE type);
};

void KEYXLATE::addkey(i32 scan, i32 key, i32 mode, XLATETYPE type)
{
  m_xlate = (i32 *)UI_realloc(m_xlate, 16*(m_numkey+1), 0xffff);
  m_xlate[m_numkey*4+0] = scan;
  m_xlate[m_numkey*4+1] = key;
  m_xlate[m_numkey*4+2] = mode;
  m_xlate[m_numkey*4+3] = type;
  m_numkey++;
}

i32 KEYXLATE::translate(i32 scan, i32 mode, XLATETYPE type)
{
  for (i32 i=0; i<m_numkey; i++)
  {
    if (   (m_xlate[4*i+0] == scan)
        && (m_xlate[4*i+2] == mode) 
        && (m_xlate[4*i+3] == type) ) return m_xlate[4*i+1];
  };
  return 0;
}

KEYXLATE keyxlate;

char *getfield(char *buf, i32& col)
{
  static char result[200];
  char term;
  i32 len = 0;
  term = ' ';
  while (buf[col] != '\n')
  {
    if ((buf[col]!=' ') && (buf[col]!='\t')) break;
    col++;
  };
  if (buf[col] == '"')
  {
    term = '"';
    col++;
  };
  while ((buf[col] != '\n') && (buf[col] != 0))
  {
    if (   (buf[col]==term) 
        || ((term==' ') && (buf[col]=='\t'))) break;
    result[len++] = buf[col];
    if (len>198) len--;
    col++;
  };
  result[len]=0;
  return result;
}

i32 gethex(char *buf, i32& col)
{
  i32 result;
  char *field;
  field = getfield(buf,col);
  sscanf(field,"%x", &result);
  return result;
}


bool CheckData(char *buf, i32& col, i32& data, i32 min, i32 max)
{
  i32 n, result;
  char *field;
  field = getfield(buf, col);
  n = sscanf(field, "%d", &result);
  if (n==0)
  {
    n = sscanf(field,"%x", &result);
  };
  if (n==0) return false;
  if (result < min) return false;
  if (result > max) return false;
  data = result;
  return true;
}
/*
i32 AddSmartDiscard(char *buf)
{
  char fld[10][200];
  i32 i, j, col=0;
  i32 minimum;
  float level,y_intercept,slope;
  char *pfld;
  for (i=0; i<10; i++)
  {
    pfld=getfield(buf,col);
    if (*pfld == 0) break;
    strcpy(fld[i],pfld);
  };
  j=1;
  while (i>5)
  {
    if ((fld[j][0]>='A') && (fld[j][0]<='Z'))
    {
      strcat(fld[0]," ");
      strcat(fld[0],fld[j]);
      j++;
      i--;
    }
    else break;
  };
  if (i<5) return 1; //error
  if (sscanf(fld[j],"%d",&minimum)!=1) return 1;
  if (sscanf(fld[j+1],"%f",&level)!=1) return 1;
  if (sscanf(fld[j+2],"%f",&y_intercept)!=1) return 1;
  if (sscanf(fld[j+3],"%f",&slope)!=1) return 1;
  return AddSD(fld[0],minimum,level,y_intercept,slope);
  //return 0;
}
*/

//************************************************************************
// Smart Discard definitions
//
//Smart Discard only operates on databases which have been enlarged
// to size 1023.  Each item in a database is given a discard priority
// which is compared to other items in the same database.  The item with
// the highest priority is discarded first.
//
// The program operates as follows:  every clock tick initiates the
// "Smart Discard" process.  The databases are considered one at a time.
// Each clock tick causes the items in a single room to be evaluated and
// added to the list.  When all rooms have been evaluated and there are
// fewer than 900 items in the database then the next database is considered.
// If more than 900 items exist then on each tick of the clock the item
// with the highest discard priority has its priority recomputed.  If it
// is still at least as great as the next higher item then it is deleted.
// On the other hand, if its priority has declined (perhaps due to party
// movement, because we just discarded an item of this type, etc) then it 
// is put back in the list in its proper place and we wait for the next 
// clock tick.
//
// The priority calculation includes a random number between 0.0 and 1.0
// so that items with the same basic priority will be randomly selected.
//
// Format:
//
//   name  minimum  level  y-intercept  slope
//
//     name         item name - blanks are allowed  (or "default") (case-insensitive)
//     minimum      integer - If this many or fewer of the item exist then
//                            none will be considered for discard.
//     level        float - additional priority for not being on a level 
//                          adjacent to the party.
//     y-intercept  float - discard priority for minimum
//     slope        float - additional discard priority for each item over minimum
//
// The item's deletion priority is  y-intercept + slope*(num-minimum) + level
//     
// Let us provide an example:
//
// example   -->  Discard   Worm Round  20  10.0  0  1
//
// Worm Rounds are not considered for deletion unless there are more than
// 20 of them in the dungeon.  Assume there are 30 of them in the dungeon.
// The discard priority for each of the 30 Worm Rounds is equal to:
//         0 + 1*(30-20).
// Each Worm Round which is on a level not immediately adjacent to the party
// will have 10.0 added to its priority.
//
//
char *SmartDiscards[] = {
"default       30  50  20.0   1.0",
"Wooden Shield 10  50  20.0   1.0", // We hardly need more than 10 of these 
"Rock          10  50  20.0   1.0",
"Sword         10  50  20.0   1.0",
"Dagger        10  50  20.0   1.0",
"Falchion      10  50  20.0   1.0",
"Club          10  50  20.0   1.0",
"Stone Club    10  50  20.0   1.0",
"Armet         10  50  20.0   1.0",
"Torso Plate   10  50  20.0   1.0",
"Leg Plate     10  50  20.0   1.0",
"Foot Plate    10  50  20.0   1.0",
//; Miscellaneous items
"Boulder       20  50  70.0   1.0", // Be sure Boulders go before food
"Dragon Steak 150   0  20.0   0.0", // Default goes before Dragon Steak.
 NULL
};


//#define strupr( a ) _strupr( a )
//static void _strupr(char *str) {
//    if( str ) {
//	while(*str) {
//	    *str = toupper( *str );
//	    str++;
//	}
//    }
//}

void ReadConfigFile(void)
{
  char buf[501];
  i16 f;
  i32 lineNumber = 0;
  i32 col, numerror=0;
  char *field;
  i32 scancode, key, mode;
  for (i32 i=0; SmartDiscards[i] != NULL; i++)
  {
    char *temp;
    int len;
    len = strlen(SmartDiscards[i]);
    temp = (char *)UI_malloc(len+1, MALLOC016);
    strcpy (temp, SmartDiscards[i]);
    _strupr(temp);
    AddSmartDiscard(temp);
    UI_free (temp);
  };
  f=OPEN("config.txt", "r");
  if (f < 0) 
  {
    UI_MessageBox("Cannot find config.txt","Warning",MESSAGE_OK);
    return;
  };
  while (GETS(buf,500,f) != NULL)
  {
    lineNumber++;
    col=0;
    if (buf[0] == ';') continue;
    field=getfield(buf,col);
    _strupr(field);
    if (strcmp(field, ";")==0) continue;
    if (strcmp(field, "") ==0) continue;
    if (strcmp(field, "KEY")==0)
    {
      mode    =gethex(buf,col);
      scancode=gethex(buf,col);
      key     =gethex(buf,col);
      keyxlate.addkey(scancode, key, mode, TYPEKEY);
      continue;
    };
    if (strcmp(field,"HIDEMENU") == 0)
    {
      hidingMenu = true;
      continue;
    };
    if (strcmp(field,"HIDETIME") == 0)
    {
      hidingTime = true;
      continue;
    };
    if (strcmp(field,"HIDETASKBAR") == 0)
    {
      hidingTaskBar = true;
      continue;
    };
    if (strcmp(field,"SCAN")==0)
    {
      mode    =gethex(buf,col);
      scancode=gethex(buf,col);
      key     =gethex(buf,col);
      keyxlate.addkey(scancode, key, mode, TYPESCAN);
      continue;
    };
    if (strcmp(field,"MSCAN")==0)
    {
      i32 x, y;
      XLATETYPE type;
      char *button;
      mode    =gethex(buf,col);
      scancode=gethex(buf,col);
      x       =gethex(buf,col);
      y       =gethex(buf,col);
      button  =getfield(buf,col);
      _strupr(button);
      if (strcmp(button,"L")==0) type=TYPEMSCANL; else type=TYPEMSCANR;
      keyxlate.addkey(scancode, (x<<16)+y, mode, type);
      continue;
    };
    if (strcmp(field,"DIRECTORY")==0)
    {
      field = getfield(buf,col);
      if (folderName != NULL) UI_free(folderName);
      folderName = (char *) UI_malloc(strlen(field) + 1, MALLOC017);
      if (folderName!=NULL)
      {
        strcpy(folderName, field);
        folderParentName = parentFolder(folderName, folderName+strlen(folderName+1));
      };
      continue;
    };
/*
    if (strcmp(field,"CEPLACE") == 0)
    {
      i32 k, size, srcx, srcy, width, height, dstx, dsty;
      i32 copymode, palette, id, zorder;
      bool overlay;
      field = getfield(buf,col);
      if ( (field[0] < 'A')||(field[0] > 'E') ) continue;
      k = field[0] - 'A';
      size = gethex(buf,col);
      if ( (size < 0)||(size > 4) ) continue;
      srcx = gethex(buf,col);
      if ( (srcx<0) || (srcx>319) ) continue;
      srcy = gethex(buf,col);
      if ( (srcy<0) || (srcy>199) ) continue;
      width = gethex(buf,col);
      if ( (width<1) || (srcx+width>320) )continue;
      height = gethex(buf,col);
      if ( (height<1) || (srcy+height>200) )continue;
      x = gethex(buf,col);
      if ( (x < 0)||(x > 4096) ) continue;
      y = gethex(buf,col);
      if ( (y < 0)||(y >4096) ) continue;
      videoSegSize[k] = size;
      videoSegSrcX[k] = srcx;
      videoSegSrcY[k] = srcy;
      videoSegWidth[k] = width;
      videoSegHeight[k] = height;
      videoSegX[k] = x;
      videoSegY[k] = y;
      continue;
    };
*/
    if (strcmp(field,"CEDISPLAY") == 0)
    {
      if (ReadCEdisplayData(buf, col)) continue;
    };

    i = -1;
    if (strcmp(field, "WINDOWTOP")    == 0) i=0;
    if (strcmp(field, "WINDOWBOTTOM") == 0) i=1;
    if (strcmp(field, "WINDOWLEFT")   == 0) i=2;
    if (strcmp(field, "WINDOWRIGHT")  == 0) i=3;
    if (i >= 0)
    {
      char *pcf;
      int val;
      pcf = getfield(buf,col);
      if (strcmp(pcf, "=") == 0)
      {
        pcf = getfield(buf,col);
        if (sscanf(pcf,"%d",&val) == 1)
        {
          windowPos[i] = val;
          continue;
        };
      };
    };

    if (strcmp(field, "PLACE") == 0) continue;

    if (numerror==0)
    {
      char msg[200];
      if (strlen(buf)>0)
        if (buf[strlen(buf)-1] == '\n')
          buf[strlen(buf)-1] = 0;
      buf[100] = 0; //Just in case it is very long;
      sprintf(msg,"Line number = %d\n%s",lineNumber,buf);
      UI_MessageBox(msg,"Bad Config Line",MESSAGE_OK);
      numerror++;
    };
  };
  CLOSE(f);
}




i32 UI_DisableAllMessages(void)
{
  i32 oldmask = messageMask;
  messageMask = 0;
  return oldmask;
}

i32 UI_EnableMessages(i32 mask)
{
  messageMask |= mask;
  return messageMask;
}

void DisplayTime(void)
{
  unsigned int ticks;
  HDC hDC;
  unsigned short line[40];
  i32 x,y;
  GetTimeLocation(x, y);
  //SYSTEMTIME time;
  //ticks = GetTickCount();
  ticks = d.Time;
  //GetSystemTime(&time);
  swprintf(line,L"%d, ASCII=0x%02x, scancode=0x%02x",ticks,latestScanp1,latestScanp2);
  hDC = GetDC(hWnd);
  SetTextColor(hDC,RGB(255,0,0));
  ExtTextOut(hDC,x,y,ETO_OPAQUE,NULL,line,wcslen(line),NULL);
  ReleaseDC(hWnd,hDC);
}

bool IsClock(i32 x, i32 y)
{
  if (x > 40) return false;
  if (y < 250) return false;
  if (y > 262) return false;
  return true;
}

void EditGameInfo(void)
{
  EditDialog dlg;
  dlg.m_initialText = gameInfo;
  if (dlg.DoModal() == IDOK)
  {
    if (gameInfo != NULL) UI_free (gameInfo);
    gameInfo = dlg.m_finalText;
  }
  else
  {
    UI_free (dlg.m_finalText);
  };
}

int exceptionCount = 0;
void ec(void)
{
  exceptionCount++;
}


_EXCEPTION_POINTERS *per=NULL;

_EXCEPTION_RECORD exceptionRecord;

void CaptureExceptionInfo(void)
{
  memcpy(&exceptionRecord, per->ExceptionRecord, sizeof(exceptionRecord));
}

void PrintExceptionInfo(void)
{
  char message[300];
  sprintf(message,"%x\n%x\n%x\n%x\n%x", 
          PrintExceptionInfo,
          exceptionRecord.ExceptionCode,
          exceptionRecord.ExceptionAddress,
          exceptionRecord.ExceptionInformation[0],
          exceptionRecord.ExceptionInformation[1]
         );
  info(message, 0);
}


i32 CSBUI(CSB_UI_MESSAGE *msg)
{
  i32 CallCheckVBL= 0;
  if (messageMask == 0) return UI_STATUS_NORMAL;
  CEtry
  {
    switch (msg->type)
    {
    case UIM_INITIALIZE:
//        ReadConfigFile();
        //Set the GameMode //0=quit
                           //1=dungeon
                           //2=utility
                           //3=hint
        DispatchCSB(st_AskWhatToDo);
        break;
    case UIM_SETOPTION:
      switch (msg->p1)
      {
      case OPT_DMRULES:
        DM_rules = !DM_rules;
        RecordFile.Record(DM_rules ? "#DMRules\n" : "#NoDMRules\n");
        break;
      case OPT_NORMAL:
        UI_ClearScreen();
        screenSize = 1;
        break;
      case OPT_DOUBLE:
        UI_ClearScreen();
        if (msg->p2 == 1) screenSize = 1;
        else screenSize = 2;
        break;
      case OPT_TRIPLE:
        UI_ClearScreen();
        if (msg->p2 == 1) screenSize = 1;
        else screenSize = 3;
        break;
      case OPT_QUADRUPLE:
        UI_ClearScreen();
        if (msg->p2 == 1) screenSize = 1;
        else screenSize = 4;
        break;
      case OPT_RECORD:
        if (msg->p2 == 1) 
        {
          RecordMenuOption = true;
          NoRecordMenuOption = false;
        }
        else 
        {
          RecordfileOpen(false); //This will override any other
                                 //Record options.
          RecordMenuOption = false;
          NoRecordMenuOption = true;
        };
        break;
      case OPT_PLAYBACK:
        if (msg->p2 == 1) PlayfileOpen(true);
        else PlayfileOpen(false);
        if (PlayfileIsOpen())
        {
          extraTicks = false;
        };
        break;
      case OPT_QUICKPLAY:
        if (msg->p2 == 1) NoSpeedLimit = 2000000000;
        else 
        {
          NoSpeedLimit = 0;
          VBLMultiplier = 1;
        };
        break;
      case OPT_CLOCK:
        gameSpeed = (SPEEDS)msg->p2;
        break;
      case OPT_VOLUME:
        gameVolume = (VOLUMES)msg->p2;
        break;
      case OPT_PLAYERCLOCK:
        playerClock = !playerClock;
        break;
      case OPT_EXTRATICKS:
        extraTicks = !extraTicks;
        break;
      case OPT_ITEMSREMAINING:
        ItemsRemaining(0);
        break;
      case OPT_NONCSBITEMSREMAINING:
        ItemsRemaining(1);
        break;
      case OPT_TIMERTRACE:
        if (TimerTraceActive)
        {
          TimerTraceActive = false;
          if (!AttackTraceActive && !AITraceActive)
          {
            CloseTraceFile();
          };
        }
        else
        {
          if (AttackTraceActive || AITraceActive || OpenTraceFile())
          {
            TimerTraceActive = true;
          };
        };
        break;
      case OPT_DSATRACE:
        //DSAInstrumentation.Dump();
        DSATraceActive = !DSATraceActive;
        break;
      case OPT_ATTACKTRACE:
        if (AttackTraceActive)
        {
          AttackTraceActive = false;
          if (!TimerTraceActive && !AITraceActive)
          {
            CloseTraceFile();
          };
        }
        else
        {
          if (TimerTraceActive || AITraceActive || OpenTraceFile())
          {
            AttackTraceActive = true;
          };
        };
        break;
      case OPT_AITRACE:
        if (AITraceActive)
        {
          AITraceActive = false;
          if (!TimerTraceActive && !AttackTraceActive)
          {
            CloseTraceFile();
          };
        }
        else
        {
          if (TimerTraceActive || AttackTraceActive || OpenTraceFile())
          {
            AITraceActive = true;
          };
        };
        break;
      default:
        ASSERT(0,"CSB_UI_MSG");
      };
      break;
    case UIM_TERMINATE:
        return UI_STATUS_TERMINATE; 
        //break;
    case UIM_CHAR:
        {
          static bool topmost = true;
          if ((latestCharp1 == 3) && (msg->p1 == 3))
          {
            UI_Die();
            break;
          };
          latestCharp1 = msg->p1;
          latestCharType = TYPEIGNORED;
          i32 key = keyxlate.translate(msg->p1, keyboardMode, TYPEKEY);
          if (key == 0x1b) // Escape
          {
            if (PlayfileIsOpen())
            {
              char msg[80];
              sprintf(msg,"Playback paused at %d", d.Time);
              UI_MessageBox(msg,"Pause",MESSAGE_OK);
              break;
            };
          };
          if (key != 0)
          {
            if (key == 0x1d) // No longer topmost window
            {
              if (PlayfileIsOpen())
              {
                UI_MessageBox("Playback Paused","Pause",MESSAGE_OK);
                break;
              }
              else
              {
                if (gameFrozen) break; //already frozen so don't toggle
                topmost = false;
                key = 0x1b;  // Let event driver freeze the game
              };
            };
            if (key == 0x1c) // We are now topmost window
            {
              if (PlayfileIsOpen()) break; //Ignore this
              if (topmost) break;
              topmost = true;
              if (!gameFrozen) break; //already running so don't freeze.
              key = 0x1b; //Let event driver unfreeze the game
            };
            i32 next = keyQueueEnd+1;
            latestCharType = TYPEKEY;
            latestCharXlate = key;
            if (next >= keyQueueLen) next=0;
            if (next != keyQueueStart)
            {
              keyQueue[keyQueueEnd]= key;
              keyQueueEnd = next;
            };
          }
        }
        break;
    case UIM_KEYDOWN:
        {
          i32 key;
          latestScanp1 = msg->p1;        // The ascii character
          latestScanp2 = msg->p2;
          if ((key = keyxlate.translate(msg->p2&0xff, 
                                        keyboardMode, 
                                        TYPESCAN))!= 0)
          {
            latestScanType = TYPESCAN;
            latestScanXlate = key;
            i32 next = keyQueueEnd+1;
            if (next >= keyQueueLen) next=0;
            if (next != keyQueueStart)
            {
              keyQueue[keyQueueEnd]= key;
              keyQueueEnd = next;
            };
          }
          else
          if ((key = keyxlate.translate(msg->p1, keyboardMode, TYPEMSCANL))!= 0)
          {
            latestScanType = TYPEMSCANL;
            latestScanXlate = key;
            OnMouseSwitchAction(2, key>>16, key & 0xffff);
            OnMouseSwitchAction(0);
          }
          else
          if ((key = keyxlate.translate(msg->p1, keyboardMode, TYPEMSCANR))!= 0)
          {
            latestScanType = TYPEMSCANR;
            latestScanXlate = key;
            OnMouseSwitchAction(1, key>>16, key & 0xffff);
            OnMouseSwitchAction(0);
          }
          else
          {
            latestScanType = TYPEIGNORED;
          };
        }
        break;
    case UIM_TIMER:
        //The UIM_TIMER messages come no more often than
        //20 per second on my machine.  So we can cause multiple
        //VBLs per UIM_TIMER.  Our stategy is to call CheckVBL.
        //Then dispatch until nothing is left to do.  Then
        //call CheckVBL again.  Repeat entire process 10 times
        if (!hidingTime) DisplayTime();
        if (rightButtonX >= 0)
        {
          if (rightButtonTime > 0)
          {
            rightButtonTime--;
            if (rightButtonTime == 0)
            {
              msg->type = UIM_RIGHT_BUTTON_DOWN;
              msg->p1 = rightButtonX;
              rightButtonX = -1;
              msg->p2 = rightButtonY;
              CSBUI(msg);
            };
          };
        };
        CallCheckVBL = VBLperTimer*VBLMultiplier; // do it 10 times
        //NoSpeedLimit = 100000;
        if (trace >= 0) 
        {
          fprintf(GETFILE(trace),"UIM_TIMER\n");
          fflush(GETFILE(trace));
        }
        break;
    case UIM_PAINT:
        display();
        break;
    case UIM_REDRAW_ENTIRE_SCREEN:
        ForceScreenDraw();
        UI_Invalidate();
        break;
    case UIM_LEFT_BUTTON_DOWN:
        {
          i32 x,y;
          MapCoordinate(msg->p1,msg->p2,x,y);
          rightButtonX = msg->p1;
          rightButtonY = msg->p2;
          rightButtonTime = 4;
          LastTapPoint.x = x;
          LastTapPoint.y = y;

          if (IsPortraitBar(x, y))
          { //Pretend it is a Right-Click.
            OnMouseSwitchAction(0x01, x, y);
          }
          else
          {
            OnMouseSwitchAction(0x2,x, y);
          };
          EnqueMouseClick(x, y, 1);
          TAG001afe(x, y, 1);
        };
        UI_PushMessage(msg->type);

        break;
    case UIM_LEFT_BUTTON_UP:
        //OnMouseSwitchAction(0x0);
      OnMouseUnClick();
        rightButtonX = -1;
        break;
    case UIM_RIGHT_BUTTON_DOWN:
      {
        i32 x, y;
        MapCoordinate(msg->p1, msg->p2, x, y);
		    if (IsTextScrollArea(x, y))
		    {
          CSB_UI_MESSAGE csbMessage;
          csbMessage.type = UIM_CHAR;
          csbMessage.p1   = 0x1b;
          CSBUI(&csbMessage);
          break;
        };
        OnMouseSwitchAction(0x1);
		  	  
        if (virtualFullscreen)
        {
          i32 x, y, size;
          size = screenSize;
          TranslateFullscreen(msg->p1,msg->p2,x,y);
          EnqueMouseClick(x, y, 1);
          TAG001afe(x, y, 1); //[Erik]: Is this correct?
        }
        else
        {
          EnqueMouseClick(X_TO_CSB(msg->p1,screenSize), Y_TO_CSB(msg->p2,screenSize),1);
          TAG001afe(X_TO_CSB(msg->p2,screenSize), Y_TO_CSB(msg->p1,screenSize), 1); //[Erik]: Is this correct?
        };
        UI_PushMessage(msg->type);
      };
        break;
    case UIM_RIGHT_BUTTON_UP:
        OnMouseSwitchAction(0x0);
        break;
    case UIM_Statistics:
      NotImplemented();
      break;
    case UIM_EditGameInfo:
        EditGameInfo();
        break;
    case UIM_NOOP:
      break;
    default:
      ASSERT(0,"CSBUI");
    };
    do
    {
      if (CallCheckVBL > 0)
      {
        checkVBL();
        CallCheckVBL--;
      };
      while (msgStackLen != 0)
      {
        msgStackLen--;
        if (msgStack[msgStackLen].type == UIM_TERMINATE)
        { 
          //break;
          return UI_STATUS_TERMINATE;
        };
        DispatchCSB(&msgStack[msgStackLen]);
      };
    } while (CallCheckVBL > 0);
      return UI_STATUS_NORMAL;
  }
  info("exitMessageHandler",0);
  CEexception(exceptionCount)
  return UI_STATUS_TERMINATE;
};

void UI_PushMessage(MTYPE type,
                    i32 p1,i32 p2,i32 p3,i32 p4,i32 p5,i32 p6)
{
  if (msgStackLen >= maxMessageStack-1) UI_Die();
  msgStack[msgStackLen].type = type;
  msgStack[msgStackLen].p1 = p1;
  msgStack[msgStackLen].p2 = p2;
  msgStack[msgStackLen].p3 = p3;
  msgStack[msgStackLen].p4 = p4;
  msgStack[msgStackLen].p5 = p5;
  msgStack[msgStackLen].p6 = p6;
  msgStackLen++;
}
  


void UI_Invalidate(bool erase)
{
    InvalidateRect(hWnd, NULL, erase);
}

i32 UI_DeleteFile(char *fname)
{
  TEMPORARY_WIDE tw;
  tw = fname;
  DeleteFile(tw.Text());
  return 0;
}

#if defined _MSVC_INTEL || defined _MSVC_CE2002ARM
i32 UI_MessageBox(char *msg, char *title, i32 flags)
{
  TEMPORARY_WIDE twidemsg, twidetitle;
  i32 i;
  i32 MB_FLAGS = MB_APPLMODAL ;// | MB_TOPMOST;
  if (flags & MESSAGE_OK) MB_FLAGS |= MB_OK;
  if (flags & MESSAGE_YESNO) MB_FLAGS |= MB_YESNO;
  if (flags & MESSAGE_ICONERROR) MB_FLAGS |= MB_ICONERROR;
  if (flags & MESSAGE_ICONWARNING) MB_FLAGS |= MB_ICONWARNING;
  bool saveCursorShowing;
  saveCursorShowing = cursorIsShowing;
  if (!cursorIsShowing) ShowCursor(true);
  i32 mask = UI_DisableAllMessages();
  if (title == NULL) title = "Error";
  twidemsg = msg;
  twidetitle = title;
  i = MessageBox(hWnd, twidemsg.Text(), twidetitle.Text(), MB_FLAGS);
  UI_EnableMessages(mask);
  if (!saveCursorShowing) ShowCursor(false);
  if (i == IDYES) return MESSAGE_IDYES;
  if (i == IDNO)  return MESSAGE_IDNO;
  return MESSAGE_IDOK;
}
#endif //_MSVC_INTEL
#ifdef _LINUX
# include <gnome.h>
i32 UI_MessageBox(char *msg, char *title, i32 flags ) {
    g_warning("[%s] %s", title, msg);
    GtkWidget *dialog;
    GString *text = g_string_new(msg);
	
  i32 i=1;// default answer
    if(title==NULL){
	title="Error: \n";    
    }else{
	text = g_string_prepend(text,": \n");
    }
    
    text = g_string_prepend(text,title);
   
  bool saveCursorShowing, cursorIsShowing, is_ok=false;
  saveCursorShowing = cursorIsShowing = ( SDL_ENABLE == SDL_ShowCursor(SDL_QUERY) );
  if (!cursorIsShowing) SDL_ShowCursor(SDL_ENABLE);
  
    
  if (flags & MESSAGE_OK){
      dialog = gnome_message_box_new(text->str, GNOME_MESSAGE_BOX_INFO, GNOME_STOCK_BUTTON_OK, NULL);
      is_ok=true;
  }
    else if (flags & MESSAGE_YESNO)
      dialog = gnome_message_box_new(text->str,GNOME_MESSAGE_BOX_QUESTION, GNOME_STOCK_BUTTON_YES, GNOME_STOCK_BUTTON_NO, NULL);
    else if (flags & MESSAGE_ICONERROR)
      dialog = gnome_message_box_new(text->str,GNOME_MESSAGE_BOX_ERROR,"Die", NULL);
    else if (flags & MESSAGE_ICONWARNING)
      dialog = gnome_message_box_new(text->str,GNOME_MESSAGE_BOX_WARNING,"Close",NULL);
    
    i32 mask = UI_DisableAllMessages();
    
     i = gnome_dialog_run_and_close (GNOME_DIALOG (dialog));
     
  UI_EnableMessages(mask);
  if (!saveCursorShowing) SDL_ShowCursor(SDL_DISABLE);
  if (i == 0 && is_ok == false) return MESSAGE_IDYES; // 0 == first button pressed
  if (i == 1 || i == -1)  return MESSAGE_IDNO; // 1 == second button pressed, (-1) == if MsgBox is 'destroyed'
  return MESSAGE_IDOK;
}
#endif //_LINUX





i32 UI_CONSTAT(void)
{
  if (keyQueueStart != keyQueueEnd) return 1;
  return 0; // No console input available
}

i32 UI_DIRECT_CONIN(void)
{ //We should wait for a keystroke.  But we don't.  It is
  // up to you to be sure that input exists before
  // calling DIRECT_CONIN.
  ASSERT(keyQueueStart != keyQueueEnd, "KeyQueue");
  if (keyQueueStart == keyQueueEnd) return -1;
  i32 key = keyQueue[keyQueueStart];
  keyQueueStart++;
  if (keyQueueStart >= keyQueueLen) keyQueueStart=0;
  return key;
}

void UI_Die(void)
{
  PostQuitMessage(0);
}

void UI_Sleep(i32 milliseconds)
{
#ifdef _MSVC_INTEL
  if (!NoSleep) Sleep(milliseconds); //Windows sleep function
#endif //_MSVC_INTEL
#ifdef TARGET_OS_MAC
  Sleep(milliseconds); 
#endif
#ifdef _LINUX
  usleep(milliseconds*1000);
#endif //_LINUX
}

void UI_StopSound(void)
{
  sndPlaySound(NULL,SND_SYNC);
}


bool UI_PlaySound(char *wave, i32 flags, int volume)
{
  //Return true if new sound successfully started.
  i32 sndflags;
  ASSERT (wave != NULL,"PLaySound");
  sndflags = 0;
  if (flags & SOUND_SYNC)  sndflags |= SND_SYNC;
  if (flags & SOUND_ASYNC) sndflags |= SND_ASYNC;
  if (flags & SOUND_MEMORY)sndflags |= SND_MEMORY;
  sndflags |= SND_ASYNC | SND_NOSTOP;
  return sndPlaySound((unsigned short *)wave, sndflags) != 0;
}


#ifdef _MSVC_CE2002ARM
void UI_ClearScreen(void)
{
  NotImplemented();// RedrawWindow(hWnd,NULL,NULL,RDW_ERASE|RDW_INVALIDATE);
}

unsigned short *VIDEOCE::GetBitmap(int width, int height)
{
  bmi.bmih.biBitCount = 16;
  bmi.bmih.biCompression = BI_BITFIELDS;
  bmi.bmih.biHeight = -height;
  bmi.bmih.biPlanes = 1;
  bmi.bmih.biSize = sizeof(BITMAPINFOHEADER);
  bmi.bmih.biWidth = width;
  bmi.bmih.biSizeImage = 0;
  bmi.bmih.biClrImportant = 16;
  bmi.bmih.biClrUsed = 16;
  bmi.bmih.biXPelsPerMeter = 0;
  bmi.bmih.biYPelsPerMeter = 0;
  
  bmi.colors[0] = 0x007c00;//0xda;//255;
  bmi.colors[1] = 0x0003e0;//255;
  bmi.colors[2] = 0x00001f;//255;

  /*
  bmi.colors[3] = 0x00ffff;//255;
  bmi.colors[4] = 0xff0000;//255;
  bmi.colors[5] = 0xff00ff;//255;
  bmi.colors[6] = 0xffff00;//255;
  bmi.colors[7] = 0xffffff;//255;
  bmi.colors[8] = 0x800000;//255;
  bmi.colors[9] = 0x008000;//255;
  bmi.colors[10] = 0x808000;//255;
  bmi.colors[11] = 0x000080;//255;
  bmi.colors[12] = 0x800080;//255;
  bmi.colors[13] = 0x008080;//255;
  bmi.colors[14] = 0x808080;//255;
  bmi.colors[15] = 0x404040;//255;
  */
  hDC = GetDC(hWnd);
  if (!hDC) return NULL;
  hDCmem = CreateCompatibleDC(hDC);
  if (hDCmem == NULL) return NULL;
  // Create a bitmap compatible with the device associated with the 
  // device context.
  hBitmap = CreateDIBSection (hDC, (BITMAPINFO *)(&bmi), DIB_RGB_COLORS, (void**)&bitmap, NULL, 0);
  if (hBitmap == NULL) return NULL;
  // Select the new bitmap object into the memory device context. 
  hOldBitmap = (HBITMAP)SelectObject (hDCmem, hBitmap);
  if (hOldBitmap == NULL) return NULL;
  return bitmap;
};

void VIDEOCE::DrawBitmap(int srcX, int srcY, int dstX, int dstY, int width, int height)
{
  RECT rect;
  static int maxh = 300;
  if (hidingTaskBar)
  {
  	SetWindowPos(hWnd, HWND_TOP, 0,0, 0,0, SWP_NOSIZE);
	  //SHFullScreen(hWnd,SHFS_HIDETASKBAR);
	  SHFullScreen(hWnd,SHFS_HIDETASKBAR);
  }
  else
  {
	  SetWindowPos(hWnd, HWND_TOP, 0, defaultWindowRect.top, 0,0, SWP_NOSIZE);
	  SHFullScreen(hWnd,SHFS_SHOWTASKBAR);
  };
  if (hidingMenu)
  {
    ShowWindow(g_hwndCB, false);
	  SHFullScreen(hWnd,SHFS_HIDESIPBUTTON);
  }
  else
  {
    ShowWindow(g_hwndCB, true);
	  SHFullScreen(hWnd,SHFS_SHOWSIPBUTTON);
  };
  GetWindowRect(hWnd,&rect);
  BitBlt (hDC,  dstX, dstY, width, height, hDCmem,  srcX, srcY, SRCCOPY);
}

void VIDEOCE::ClearScreen(void)
{
  RECT rect;
  int result;
  static int maxh = 300;
  GetWindowRect(hWnd,&rect);
  result = FillRect(hDC, &rect, (HBRUSH)GetStockObject(BLACK_BRUSH));
  //BitBlt (hDC,  rect.left, rect.top, width, height, hDCmem,  srcX, srcY, SRCCOPY);
}

/*
void VIDEOCE::SetPalette(i32 *palette)
{
  int i;
  for (i=0; i<16; i++) bmi.colors[i] = palette[i];
}
*/
void VIDEOCE::ReleaseBitmap(void)
{
  // Select the old bitmap back into the device context.
  if (hOldBitmap != NULL) SelectObject (hDCmem, hOldBitmap);
  hOldBitmap = NULL;

  // Delete the bitmap object and free all resources associated with it. 
  if (hBitmap != NULL) DeleteObject (hBitmap);
  hBitmap = NULL;

  // Delete the memory device context and the display device context.
  if (hDCmem != NULL) DeleteDC (hDCmem);
  hDCmem = NULL;

  if (hDC != NULL) DeleteDC (hDC);
  hDC = NULL;
}
/*
void UI_SetDIBitsToDevice(i32 dstX,
                          i32 dstY,
                          i32 srcWidth,
                          i32 srcHeight,
                          i32 srcX,
                          i32 srcY,
                          i32 firstScan,
                          i32 numScan,
                          char *bitmap,
                          void *bInfo,//struct tagBITMAPINFO *bInfo,
                          i32 flags)
{
  HDC newhdc;
  newhdc = 0;
  if (hdc == 0)
  {
    newhdc = GetDC(hWnd);
    hdc = newhdc;
  };
  NotImplemented();
  SetDIBitsToDevice(hdc,
                    dstX,
                    dstY,
                    srcWidth,
                    srcHeight,
                    srcX,
                    srcY,
                    firstScan,
                    numScan,
                    bitmap,
                    (tagBITMAPINFO *)bInfo,
                    flags);
  if (newhdc != 0)
  {
    ReleaseDC(hWnd,newhdc);
    hdc = 0;
  };
}
*/
#endif

void UI_GetCursorPos(i32 *x, i32 *y)
{
  //On the PocketPC we don't really have a cursor position.
  //We will record the last place that the screen was 'tapped'
  //and report that as the current position.
  *x = LastTapPoint.x;
  *y = LastTapPoint.y;
}

bool UI_ProcessOption(char *key, char *value)
{
  if (strcmp(key,"DIRECTORY")==0)
  {
    if (folderName != NULL) UI_free(folderName);
    folderName = (char *)UI_malloc(strlen(value)+2, MALLOC018);
    if (folderName != NULL) 
    {
      strcpy(folderName,value);
      if (folderName[strlen(folderName)-1] != '\\')
      {
        folderName[strlen(folderName)+1] = 0;
        folderName[strlen(folderName)] = '\\';
      };
      if (folderParentName != NULL) UI_free(folderParentName);
      folderParentName = parentFolder(folderName, folderName+strlen(folderName));
    };
    return true;
  };
  if (strcmp(key,"REPEAT") == 0)
  {
    RepeatGame = true;
    return true;
  };
  if (strcmp(key,"HEIGHT") == 0)
  {
    sscanf(value,"%d", &WindowHeight);
    return true;
  };
  if (strcmp(key,"QUICK") == 0)
  {
    sscanf(value,"%d", &NoSpeedLimit);
    return true;
  };
  if (strcmp(key,"VBLMULTIPLIER") == 0)
  {
    sscanf(value,"%d", &VBLMultiplier);
    return true;
  };
  if (strcmp(key,"DUNGEON") == 0)
  {
    strcpy(dungeonName,value);
    return true;
  };
  if (strcmp(key,"WIDTH") == 0)
  {
    sscanf(value,"%d", &WindowWidth);
    return true;
  };
  if (strcmp(key,"X") == 0)
  {
    sscanf(value,"%d", &WindowX);
    return true;
  };
  if (strcmp(key,"Y") == 0)
  {
    sscanf(value,"%d", &WindowY);
    return true;
  };
  if (strcmp(key,"SIZE") == 0)
  {
    _strupr(value);
    if (strcmp(value,"FULL") == 0)
    {
      fullscreenRequested = true;
    };
    if (strcmp(value,"EXTRALARGE") == 0)
    {
      screenSize = 4;
      return true;
    };
    if (strcmp(value,"VERYLARGE") == 0)
    {
      screenSize = 3;
      return true;
    };
    if (strcmp(value,"LARGE") == 0)
    {
      screenSize = 2;
      return true;
    };
    if (strcmp(value,"SMALL") == 0)
    {
      screenSize = 1;
      return true;
    };
    return false;
  };
  if (strcmp(key,"PLAY") == 0)
  {
    strcpy(PlayfileName,value);
    extraTicks = false;
    PlaybackCommandOption = true;
    return true;
  };
  if (strcmp(key,"RECORD") == 0)
  {
    RecordCommandOption = true;
    NoRecordCommandOption = false;
  };
  if (strcmp(key,"NORECORD") == 0)
  {
    NoRecordCommandOption = true;
    RecordCommandOption = false;
  };
  if (strcmp(key,"DIRECTX") == 0)
  {
    usingDirectX = true;
  };
  return false;
}

void UI_BeginRecordOK(bool ok)
{
  BeginRecordOK = ok;
  if (ok)
  {
    if (RecordCommandOption || RecordMenuOption)
    {
      RecordfileOpen(true);
    }
    else if (!NoRecordMenuOption)
    {
      RecordfilePreOpen();
    };
    if (PlaybackCommandOption)
    {
      PlayfileOpen(true);
      if (PlayfileIsOpen())
      {
        extraTicks = false;
      };
    };
  };
}

char *parentFolder(char *filename, char *endname)
{
  char *result=NULL;
  // filename points to first character; endname points to last+1;
  // Now we back up, looking for a '\'
#ifdef _MSVC_INTEL
  if (*(endname-1) == '\\') endname--;
  while (endname > filename)
  {
    endname--;
    if (*endname == '\\') break;
  };
#endif
#if defined _LINUX || defined TARGET_OS_MAC
   if (*(endname-1) == '/') endname--;
  while (endname > filename)
  {
    endname--;
    if (*endname == '/') break;
  };
#endif
  if (endname > filename)
  {
    endname++; //include the backslash at the end
    result = (char *)UI_malloc(endname-filename+1, MALLOC019);
    if (result != NULL)
    {
      memcpy(result,filename,endname-filename);
      result[endname-filename] = 0;
    };
  };
  return result;
}

i64 UI_GetSystemTime(void)
{
  return GetTickCount();
  /*
  // Cumulative milliseconds since 1980 (or whenever..not important)
  SYSTEMTIME st;
  i64 result;
  i32 Day;
  GetSystemTime(&st);
  if (st.wMonth < 3)
  {
    st.wYear--;
    st.wMonth += 12;
  };
  Day = st.wDay;
  Day += 365 * st.wYear;
  Day += st.wYear/4;//We won't worry about -year/400 ;-)
  Day += 30 * st.wMonth;
  Day += (3 * (st.wMonth + 1))/5;
  result = Day;
  result *= 24;
  result += st.wHour;
  result *= 60;
  result += st.wMinute;
  result *= 60;
  result += st.wSecond;
  result *= 1000;
  result += st.wMilliseconds;
  return result;
  */
}

FILE *UI_fopen(char *name, char *flags)
{
  return fopen(name, flags);
}

void *UI_malloc(i32 size, ui32 id)
{
  void *result;
#ifdef _DEBUG
  result = malloc(size+12);
#else
  result = malloc(size);
#endif
  if (result == NULL) AllocationError();
#ifdef _DEBUG
  *(ui8 **)((ui8 *)result+0) = allocatedMemoryList;
  *(ui8 **)((ui8 *)result+4) = NULL;
  *(i32 *)((ui8 *)result+8) = id;
  if (allocatedMemoryList != NULL)
  {
    *(void **)(allocatedMemoryList+4) = result;
  };
  allocatedMemoryList = (ui8 *)result;
  if (id != 0xffff) listLength++;
  return (void *)((ui8 *)result+12);
#else
  return result;
#endif
}


void *UI_realloc(void *buf, i32 size, ui32 id)
{
  void *result;
#ifdef _DEBUG
  void **next, **prev;
  if (buf == NULL)
  {
    return UI_malloc(size, id);
  }
  else
  {
    buf = (void *)((ui8 *)buf - 12);
    size += 12;
    result = realloc(buf, size);
    if (result == NULL) AllocationError();
    prev = ((void ***)result)[1];
    if (prev == NULL)
    {
      allocatedMemoryList = (ui8 *)result;
    }
    else
    {
      prev[0] = result; //Fix forward link of previous.
    };
    next = ((void ***)result)[0];
    if (next != NULL)
    {
      next[1] = result;//Fix back link of next.
    };
  };
  return (void *)((char *)result + 12);
#else
  if (buf == NULL)
  {
    result = realloc(NULL, size);
  }
  else
  {
    result = realloc((char *)buf, size);
  };
  if (result == NULL) AllocationError();
  return (void *)((char *)result);
#endif
}

void UI_free(void *buf)
{
#ifdef _DEBUG
  void **next, **prev;
  buf = (void *)((pnt)buf-12);
  prev = ((void ***)buf)[1];
  next = ((void ***)buf)[0];
  if (prev == NULL)
  {
    allocatedMemoryList = (ui8 *)next;
  }
  else
  {
    prev[0] = next;
  };
  if (next != NULL)
  {
    next[1] = prev;
  };
  if ( ((ui32 *)buf)[2] != 0xffff) listLength--;
  free(buf);
#else
  free(buf);
#endif
}

LISTING::LISTING(void)
{
  m_listing = (char *)malloc(1);
  *m_listing = 0;
}

LISTING::~LISTING(void)
{
  free(m_listing);
  m_listing = NULL;
}

void LISTING::AddLine(char *line)
{
  i32 len;
  len = strlen(m_listing);
  m_listing = (char *)realloc(m_listing, len + strlen(line) + 5);
  if (len != 0)
  {
    m_listing[len] = '\015';
    m_listing[len+1] = '\012';
    len +=2;
  };
  strcpy(m_listing+len, line);
}

void LISTING::DisplayList(char * title)
{
  EditDialog dlg;
  dlg.m_initialText = m_listing;
  dlg.DoModal();
}


#ifdef MIDI

void MIDI::PlayMIDI(void)
{
  OpenMediaFile();
  PlayMediaFile();
}

#endif

