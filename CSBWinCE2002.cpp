// CSBwinCE2002.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "UI.h"
#include "CSBwinCE2002.h"
#include "dispatch.h"
//#include "Objects.h"
#include "CSB.h"
#include "data.h"
#include <commctrl.h>
#include <aygshell.h>
#include <sipapi.h>


#define MAX_LOADSTRING 100

void ReadConfigFile(void);

i32 trace = -1;
CSB_UI_MESSAGE csbMessage;
RECT defaultWindowRect;
bool hidingTaskBar = false;
bool hidingMenu = false;
bool hidingTime = false;
bool overlappingText = false;
extern i32 screenSize;
extern bool BeginRecordOK;
extern bool ItemsRemainingOK;
extern bool PlayfileIsOpen(void);
extern bool RecordfileIsOpen(void);
extern bool TimerTraceActive;
extern bool AttackTraceActive;
extern bool AITraceActive;
extern bool RecordMenuOption;
extern i32 NoSpeedLimit;
extern i32 MostRecentlyAdjustedSkills[2];
extern i32 LatestSkillValues[2];
extern XLATETYPE latestScanType;
extern XLATETYPE latestCharType;
extern i32 latestCharp1;
extern i32 latestScanp1;
extern i32 latestScanp2;
extern i32 latestScanXlate;
extern i32 latestCharXlate;
extern unsigned char *encipheredDataFile;
extern bool simpleEncipher;

char *parentFolder(char *folderName, char *endName);

char *helpMessage = "CSBWin looks in three places for files:\n"
                    " 1) The default directory\n"
                    " 2) The parent of the default directory\n"
                    " 3) The directory of CSBWin.exe\n"
                    "      Searched in order 1, 2, 3\n\n."
                    "The default directory can be spcified in two ways:\n"
                    " 1) The command line   directory=\"c:\\My Games\\DM\\\"\n"
                    " 2) The config.txt file   directory \"c:\\My Games\\DM\\\"\n\n"
                    "      The quotes are necessary if the name contains spaces.\n"
                    "      The config.txt file overrides the command line\n"
                    "      except (of course) when searching for config.txt\n\n"
                    "If no default directory is specified then the only\n"
                    "directory searched will be the directory containing CSBWin.exe.\n\n"
                    "Whenever a file is created it will be created in the first of\n"
                    "the three directories that is specified.  So new files will go\n"
                    "into the default directory unless none is specified, in which\n"
                    "case they will go into the CSBWin.exe directory";

i32 WindowWidth = 652;
i32 WindowHeight = 460;
i32 WindowX = 0;
i32 WindowY = 0;
i32 windowPos[4] = {0,319, 0, 239}; //top, bottom, left, right

extern ui32 VBLInterruptCount;
extern ui32 CheckVBLCount;
extern ui32 STBLTCount;
extern i32 GameTime;

bool fullscreenRequested = false;
bool virtualFullscreen = false;
bool cursorIsShowing = true;
HDC hdc;

i32 line = 0;

// Global Variables:
HINSTANCE hInst;                // current instance
HWND hWnd; // Global
HWND g_hwndCB;          // The command bar handle
TCHAR szTitle[MAX_LOADSTRING];                // The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];                // The title bar text
TCHAR T_szCSBVersion[MAX_LOADSTRING];
char szCSBVersion[MAX_LOADSTRING];


// Global Variables:
//HINSTANCE     g_hInst;        // The current instance

static SHACTIVATEINFO s_sai;

// Forward declarations of functions included in this code module:
ATOM        MyRegisterClass (HINSTANCE, LPTSTR);
BOOL        InitInstance  (HINSTANCE, int);
LRESULT CALLBACK  WndProc     (HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK  About     (HWND, UINT, WPARAM, LPARAM);
HWND        CreateRpCommandBar(HWND);


void info(char *, unsigned int);
int WINAPI WinMain( HINSTANCE hInstance,
          HINSTANCE hPrevInstance,
          LPTSTR    lpCmdLine,
          int       nCmdShow)
{
  MSG msg;
  HACCEL hAccelTable;
  if (encipheredDataFile != NULL)
      memcpy(encipheredDataFile, WinMain, 64);

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_CSBWINCE2002, szWindowClass, MAX_LOADSTRING);
	LoadString(hInstance, IDS_VERSION, T_szCSBVersion, MAX_LOADSTRING);
	//MyRegisterClass(hInstance);

 // Perform application initialization:
  if (!InitInstance (hInstance, nCmdShow)) 
  {
    return FALSE;
  };

  speedTable[SPEED_GLACIAL].vblPerTick = 1000;
  speedTable[SPEED_MOLASSES].vblPerTick = 55;
  speedTable[SPEED_VERYSLOW].vblPerTick = 33;
  speedTable[SPEED_SLOW].vblPerTick = 22;
  speedTable[SPEED_NORMAL].vblPerTick = 15;
  speedTable[SPEED_FAST].vblPerTick = 11;
  speedTable[SPEED_QUICK].vblPerTick = 7;

  volumeTable[VOLUME_FULL].attenuation = 0;
  volumeTable[VOLUME_HALF].attenuation = 6;
  volumeTable[VOLUME_QUARTER].attenuation = 12;
  volumeTable[VOLUME_EIGHTH].attenuation = 18;
  volumeTable[VOLUME_OFF].attenuation = 100;

  volumeTable[VOLUME_FULL].divisor = 1;
  volumeTable[VOLUME_HALF].divisor = 2;
  volumeTable[VOLUME_QUARTER].divisor = 4;
  volumeTable[VOLUME_EIGHTH].divisor = 8;
  volumeTable[VOLUME_OFF].divisor = 65535;



  hAccelTable = LoadAccelerators(hInstance, (LPCTSTR)IDC_CSBWINCE2002);

  // Main message loop:
  while (GetMessage(&msg, NULL, 0, 0)) 
  {
    if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) 
    {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    }
  }

  return msg.wParam;
}


char *GetField(char *pCol, char *field, char term)
{
  i32 col;
  while (*pCol == ' ') pCol++;
  col = 0;
  field[col] = 0;
  while ( col<80 )
  {
    if (*pCol == 0) return pCol;
    if (*pCol == term) return pCol;
    if ( (term == '=') && (*pCol == ' ') ) return pCol;
    field[col] = *pCol;
    pCol++;
    col++;
    field[col] = 0;
  };
  return pCol;
}

char *ParseOption(char *pCol, char *key, char *value)
{
  pCol = GetField(pCol, key, '=');
  _strupr(key);
  value[0] = 0;
  if (*pCol == '=') 
  {
    if (pCol[1] == '"')
    {
      pCol = GetField(pCol+2, value, '"');
    }
    else
    {
      pCol = GetField(pCol+1, value, ' ');
    };
  }
  return pCol+1;
}


void ProcessCommandLine(void)
{
  //char *lpCmdLine;
  int len;
  char *pCol, *command, *lastSlash;
  char term;
  char key[81], value[81];
  unsigned short modName[500];
  TEMPORARY_CHAR commandLine;
  TEMPORARY_CHAR moduleName;
  commandLine = GetCommandLine();
  GetModuleFileName(NULL, modName, 499);
  moduleName = modName;
  term = ' ';
  pCol = moduleName.Text();
  // We want to find the last backslash.  Everything up to there
  // will become our root directory.
  lastSlash = NULL;
  while (*pCol != 0)
  {
    if (*pCol == '\\') lastSlash = pCol;
    pCol++;
  };
  len = lastSlash-moduleName.Text()+1;
  root = (char *)UI_realloc(root,len, 0xffff);
  memcpy(root, moduleName.Text()+1, len-1);
  root[len-1] = 0;
  //UI_MessageBox(root,"root",MESSAGE_OK);
  //strcpy(root,"Windows\\Start Menu\\Programs\\");
  command = pCol;
  while (*pCol != 0)
  {
    if (*pCol == term) break;
    pCol++;
  };
  //root = parentFolder(command, pCol);
  pCol = commandLine.Text();
  if (pCol[0] == '"')
  {
    pCol = GetField(commandLine.Text()+1, key, '"')+1;
  }
  else
  {
   pCol = GetField(commandLine.Text(), key, ' ');
  };
  while (*pCol != 0)
  {
    pCol = ParseOption(pCol, key, value);
    if (key[0] != 0) UI_ProcessOption(key, value);
  };
}


//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
//    It is important to call this function so that the application 
//    will get 'well formed' small icons associated with it.
//
ATOM MyRegisterClass(HINSTANCE hInstance, LPTSTR szWindowClass)
{
  WNDCLASS  wc;
  ProcessCommandLine();
    wc.style      = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc    = (WNDPROC) WndProc;
    wc.cbClsExtra   = 0;
    wc.cbWndExtra   = 0;
    wc.hInstance    = hInstance;
    wc.hIcon      = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_CSBWINCE2002));
    wc.hCursor      = 0;
    wc.hbrBackground  = (HBRUSH) GetStockObject(BLACK_BRUSH);
    wc.lpszMenuName   = 0;
    wc.lpszClassName  = szWindowClass;

  return RegisterClass(&wc);
}

//
//  FUNCTION: InitInstance(HANDLE, int)
//
//  PURPOSE: Saves instance handle and creates main window
//
//  COMMENTS:
//
//    In this function, we save the instance handle in a global variable and
//    create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
  TCHAR szTitle[MAX_LOADSTRING];      // The title bar text
  TCHAR szWindowClass[MAX_LOADSTRING];    // The window class name

  hInst = hInstance;    // Store instance handle in our global variable
  // Initialize global strings
  LoadString(hInstance, IDC_CSBWINCE2002, szWindowClass, MAX_LOADSTRING);
  LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);

  //If it is already running, then focus on the window
  hWnd = FindWindow(szWindowClass, szTitle);  
  if (hWnd) 
  {
    // set focus to foremost child window
    // The "| 0x01" is used to bring any owned windows to the foreground and
    // activate them.
    SetForegroundWindow((HWND)((ULONG) hWnd | 0x00000001));
    return 0;
  } 

  MyRegisterClass(hInstance, szWindowClass);
  
  //result = GetClientRect(hWnd, &rect);
  
  ReadConfigFile();
  hWnd = CreateWindow(szWindowClass, szTitle, WS_VISIBLE,
    CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, hInstance, NULL);
  if (!hWnd)
  { 
    return FALSE;
  }
  //When the main window is created using CW_USEDEFAULT the height of the menubar (if one
  // is created is not taken into account). So we resize the window after creating it
  // if a menubar is present
  {
    RECT rc;
    GetWindowRect(hWnd, &rc);
    rc.top    = windowPos[0];
    rc.bottom = windowPos[1];
    rc.left   = windowPos[2];
    rc.right  = windowPos[3];
	defaultWindowRect = rc;
    if (g_hwndCB)
      MoveWindow(hWnd, rc.left, rc.top, rc.right-rc.left+1, rc.bottom-rc.top+1, FALSE);
  }

  ShowWindow(hWnd, nCmdShow);
  UpdateWindow(hWnd);
  SetTimer(hWnd, 1, 50, NULL);

  return TRUE;
}

void MTRACE(char *msg)
{
  if (trace < 0) return;
  FILE *f = GETFILE(trace);
  fprintf(f, msg);
  fflush(f);
}





//
//  FUNCTION: WndProc(HWND, unsigned, WORD, LONG)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT  - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
  int wmId, wmEvent;
  PAINTSTRUCT ps;
  static i32 x,y;
  static bool inWindow;
  i32 size;
  //TCHAR szHello[MAX_LOADSTRING];

  switch (message) 
  {
    /*
    case WM_WINDOWPOSCHANGED:
      {
        WINDOWPOS *pPos;
        int i = SWP_HIDEWINDOW;
        int j = SWP_SHOWWINDOW;
        int m = (int)HWND_TOP;
        int n = (int)HWND_BOTTOM;
        int o = (int)HWND_NOTOPMOST;
        int p = (int)HWND_TOPMOST;
        pPos = (WINDOWPOS *)lParam;
        MTRACE("WM_WINDOWPOSCHANGED\n");
//        if ((pPos->hwndInsertAfter == HWND_TOP) && !gameFrozen) break;
//        if ((pPos->hwndInsertAfter != HWND_TOP) && gameFrozen) break;
//        csbMessage.p1 = 0x1b; //virtual key
        ////
        if (pPos->hwndInsertAfter == HWND_TOP)
        {
          csbMessage.p1 = 0x1c; // We are now at the top
        }
        else
        {
          csbMessage.p1 = 0x1d; // We are no longer topmost
        };
        ////
        csbMessage.type=UIM_CHAR;
        if (CSBUI(&csbMessage) != UI_STATUS_NORMAL)
        {
          PostQuitMessage(0);
          break;
        };
        break;
      };
      */
    case WM_KILLFOCUS:
      csbMessage.p1 = 0x1d; // We are no longer topmost
        ////
      csbMessage.type=UIM_CHAR;
      if (CSBUI(&csbMessage) != UI_STATUS_NORMAL)
      {
        PostQuitMessage(0);
        break;
      };
      break;
    case WM_SETFOCUS:
      csbMessage.p1 = 0x1c; // We are now topmost
        ////
      csbMessage.type=UIM_CHAR;
      if (CSBUI(&csbMessage) != UI_STATUS_NORMAL)
      {
        PostQuitMessage(0);
        break;
      };
      break;
    case WM_COMMAND:
      wmId    = LOWORD(wParam); 
      wmEvent = HIWORD(wParam); 
      // Parse the menu selections:
      switch (wmId)
      { 
        case 0:
            MTRACE("0\n");
            break;
    		case IDM_HideTaskBar:
		    	hidingTaskBar = !hidingTaskBar;
			    break;
    		case IDM_HideMenu:
		    	hidingMenu = !hidingMenu;
			    break;
//    		case IDM_OverlapText:
//		    	overlappingText = !overlappingText;
//          ForceScreenDraw();
//			    break;
        case IDM_HideTime:
          hidingTime = !hidingTime;
			    break;
        case IDM_HELP_ABOUT:
          DialogBox(hInst, (LPCTSTR)IDD_ABOUTBOX, hWnd, (DLGPROC)About);
            break;
      /*
        case IDM_Statistics:
            MTRACE("IDM_Statistics\n");
            csbMessage.type = UIM_Statistics;
            csbMessage.p1 = 0;
            csbMessage.p2 = 0;
            if (CSBUI(&csbMessage) != UI_STATUS_NORMAL)
            {
              PostQuitMessage(0);
              break;
            }; 
            break; 
        */
        case IDM_GameVersion:
            MTRACE("IDM_GameVersion\n");
            csbMessage.type = UIM_EditGameInfo;
            csbMessage.p1 = 0;
            csbMessage.p2 = 2; //2-(screenSize==2); //new value
            if (CSBUI(&csbMessage) != UI_STATUS_NORMAL)
            {
              PostQuitMessage(0);
              break;
            }; 
            break; 
        case IDOK:
          SendMessage(hWnd, WM_ACTIVATE, MAKEWPARAM(WA_INACTIVE, 0), (LPARAM)hWnd);
          SendMessage (hWnd, WM_CLOSE, 0, 0);
          break;
        case IDC_QuickPlay:
            MTRACE("IDC_QuickPlay\n");
            if (!PlayfileIsOpen()) break;
            csbMessage.type = UIM_SETOPTION;
            csbMessage.p1 = OPT_QUICKPLAY;
            csbMessage.p2 = (NoSpeedLimit!=0) ? 0 : 1;
            if (CSBUI(&csbMessage) != UI_STATUS_NORMAL)
            {
              PostQuitMessage(0);
              break;
            };
            break;
        case IDM_Glacial:
            MTRACE("IDM_Glacial\n");
            csbMessage.type = UIM_SETOPTION;
            csbMessage.p1 = OPT_CLOCK;
            csbMessage.p2 = SPEED_GLACIAL;
            if (CSBUI(&csbMessage) != UI_STATUS_NORMAL)
            {
              PostQuitMessage(0);
              break;
            };
            break;
        case IDM_Molasses:
            MTRACE("IDM_Molasses\n");
            csbMessage.type = UIM_SETOPTION;
            csbMessage.p1 = OPT_CLOCK;
            csbMessage.p2 = SPEED_MOLASSES;
            if (CSBUI(&csbMessage) != UI_STATUS_NORMAL)
            {
              PostQuitMessage(0);
              break;
            };
            break;
        case IDM_VerySlow:
            MTRACE("IDM_VerySlow\n");
            csbMessage.type = UIM_SETOPTION;
            csbMessage.p1 = OPT_CLOCK;
            csbMessage.p2 = SPEED_VERYSLOW;
            if (CSBUI(&csbMessage) != UI_STATUS_NORMAL)
            {
              PostQuitMessage(0);
              break;
            };
            break;
        case IDM_Slow:
            MTRACE("IDM_Slow\n");
            csbMessage.type = UIM_SETOPTION;
            csbMessage.p1 = OPT_CLOCK;
            csbMessage.p2 = SPEED_SLOW;
            if (CSBUI(&csbMessage) != UI_STATUS_NORMAL)
            {
              PostQuitMessage(0);
              break;
            };
            break;
        case IDM_Normal:
            MTRACE("IDM_Normal\n");
            csbMessage.type = UIM_SETOPTION;
            csbMessage.p1 = OPT_CLOCK;
            csbMessage.p2 = SPEED_NORMAL;
            if (CSBUI(&csbMessage) != UI_STATUS_NORMAL)
            {
              PostQuitMessage(0);
              break;
            };
            break;
        case IDM_Fast:
            MTRACE("IDM_Fast\n");
            csbMessage.type = UIM_SETOPTION;
            csbMessage.p1 = OPT_CLOCK;
            csbMessage.p2 = SPEED_FAST;
            if (CSBUI(&csbMessage) != UI_STATUS_NORMAL)
            {
              PostQuitMessage(0);
              break;
            };
            break;
            
        case IDM_Quick:
            MTRACE("IDM_Quick\n");
            csbMessage.type = UIM_SETOPTION;
            csbMessage.p1 = OPT_CLOCK;
            csbMessage.p2 = SPEED_QUICK;
            if (CSBUI(&csbMessage) != UI_STATUS_NORMAL)
            {
              PostQuitMessage(0);
              break;
            };
            break;
            
        case IDM_PlayerClock:
            MTRACE("IDM_PlayerClock\n");
            csbMessage.type = UIM_SETOPTION;
            csbMessage.p1 = OPT_PLAYERCLOCK;
            if (CSBUI(&csbMessage) != UI_STATUS_NORMAL)
            {
              PostQuitMessage(0);
              break;
            };
            break;
        case IDM_DMRULES:
            MTRACE("IDM_DMRULES\n");
            csbMessage.type = UIM_SETOPTION;
            csbMessage.p1 = OPT_DMRULES;
            if (CSBUI(&csbMessage) != UI_STATUS_NORMAL)
            {
              PostQuitMessage(0);
              break;
            };
            break;
        case IDM_ExtraTicks:
            MTRACE("IDM_ExtraTicks\n");
            csbMessage.type = UIM_SETOPTION;
            csbMessage.p1 = OPT_EXTRATICKS;
            if (CSBUI(&csbMessage) != UI_STATUS_NORMAL)
            {
              PostQuitMessage(0);
              break;
            };
            break;
        case IDC_Record:
            MTRACE("IDC_Record\n");
            csbMessage.type = UIM_SETOPTION;
            csbMessage.p1 = OPT_RECORD;
            csbMessage.p2 = 1;
            if (RecordMenuOption) csbMessage.p2 = 0;
            if (CSBUI(&csbMessage) != UI_STATUS_NORMAL)
            {
              PostQuitMessage(0);
              break;
            };
            break;
            /*
        case IDC_TimerTrace:
            MTRACE("IDC_TimerTrace\n");
            csbMessage.type = UIM_SETOPTION;
            csbMessage.p1 = OPT_TIMERTRACE;
            csbMessage.p2 = 1;
            if (TimerTraceActive) csbMessage.p2 = 0;
            if (CSBUI(&csbMessage) != UI_STATUS_NORMAL)
            {
              PostQuitMessage(0);
              break;
            };
            break;
        case IDC_AttackTrace:
            MTRACE("IDC_AttackTrace\n");
            csbMessage.type = UIM_SETOPTION;
            csbMessage.p1 = OPT_ATTACKTRACE;
            csbMessage.p2 = 1;
            if (AttackTraceActive) csbMessage.p2 = 0;
            if (CSBUI(&csbMessage) != UI_STATUS_NORMAL)
            {
              PostQuitMessage(0);
              break;
            };
            break;
        case IDC_AITrace:
            MTRACE("IDC_AITrace\n");
            csbMessage.type = UIM_SETOPTION;
            csbMessage.p1 = OPT_AITRACE;
            csbMessage.p2 = 1;
            if (AITraceActive) csbMessage.p2 = 0;
            if (CSBUI(&csbMessage) != UI_STATUS_NORMAL)
            {
              PostQuitMessage(0);
              break;
            };
            break;
            */
        case IDC_ItemsRemaining:
            MTRACE("IDC_ItemsRemaining\n");
            csbMessage.type = UIM_SETOPTION;
            csbMessage.p1 = OPT_ITEMSREMAINING;
            if (CSBUI(&csbMessage) != UI_STATUS_NORMAL)
            {
              PostQuitMessage(0);
              break;
            };
            break;
        case IDC_NonCSBItemsRemaining:
            MTRACE("IDC_NonCSBItemsRemaining\n");
            csbMessage.type = UIM_SETOPTION;
            csbMessage.p1 = OPT_NONCSBITEMSREMAINING;
            if (CSBUI(&csbMessage) != UI_STATUS_NORMAL)
            {
              PostQuitMessage(0);
              break;
            };
            break;
        case IDC_Playback:
            MTRACE("IDC_Playback\n");
            csbMessage.type = UIM_SETOPTION;
            csbMessage.p1 = OPT_PLAYBACK;
            csbMessage.p2 = 1;
            if (PlayfileIsOpen()) csbMessage.p2 = 0;
            if (CSBUI(&csbMessage) != UI_STATUS_NORMAL)
            {
              PostQuitMessage(0);
              break;
            };
            break;
            /*
        case IDC_DispatchTrace:
            MTRACE("IDC_DispatchTrace\n");
            if (trace >= 0) 
            {
              CLOSE(trace);
              trace = -1;
              break;
            };
            trace = CREATE("trace.log","w", true);
            break;
            */
        case IDM_VOLUME_OFF:
            MTRACE("IDM_VOLUME_OFF\n");
            csbMessage.type = UIM_SETOPTION;
            csbMessage.p1 = OPT_VOLUME;
            csbMessage.p2 = VOLUME_OFF;
            if (CSBUI(&csbMessage) != UI_STATUS_NORMAL)
            {
              PostQuitMessage(0);
              break;
            };
            break;
        case IDM_VOLUME_QUARTER:
            MTRACE("IDM_VOLUME_QUARTER\n");
            csbMessage.type = UIM_SETOPTION;
            csbMessage.p1 = OPT_VOLUME;
            csbMessage.p2 = VOLUME_QUARTER;
            if (CSBUI(&csbMessage) != UI_STATUS_NORMAL)
            {
              PostQuitMessage(0);
              break;
            };
            break;
        case IDM_VOLUME_EIGHTH:
            MTRACE("IDM_VOLUME_EIGHTH\n");
            csbMessage.type = UIM_SETOPTION;
            csbMessage.p1 = OPT_VOLUME;
            csbMessage.p2 = VOLUME_EIGHTH;
            if (CSBUI(&csbMessage) != UI_STATUS_NORMAL)
            {
              PostQuitMessage(0);
              break;
            };
            break;
        case IDM_VOLUME_HALF:
            MTRACE("IDM_VOLUME_HALF\n");
            csbMessage.type = UIM_SETOPTION;
            csbMessage.p1 = OPT_VOLUME;
            csbMessage.p2 = VOLUME_HALF;
            if (CSBUI(&csbMessage) != UI_STATUS_NORMAL)
            {
              PostQuitMessage(0);
              break;
            };
            break;
        case IDM_VOLUME_FULL:
            MTRACE("IDM_VOLUME_FULL\n");
            csbMessage.type = UIM_SETOPTION;
            csbMessage.p1 = OPT_VOLUME;
            csbMessage.p2 = VOLUME_FULL;
            if (CSBUI(&csbMessage) != UI_STATUS_NORMAL)
            {
              PostQuitMessage(0);
              break;
            };
            break;
        case IDM_Exit:
          PostQuitMessage(0);
          break;
				default:
            MTRACE("Unknown WM_COMMAND\n");
				    return DefWindowProc(hWnd, message, wParam, lParam);
      }
      break;
    case WM_CREATE:
      MTRACE("WM_CREATE\n"); 
      g_hwndCB = CreateRpCommandBar(hWnd);
            // Initialize the shell activate info structure
            memset (&s_sai, 0, sizeof (s_sai));
            s_sai.cbSize = sizeof (s_sai);
      csbMessage.type=UIM_INITIALIZE;
      if (CSBUI(&csbMessage) != UI_STATUS_NORMAL)
      {
        PostQuitMessage(0);
        break;
      };
			return DefWindowProc(hWnd, message, wParam, lParam);
      break;
		case WM_PAINT:
      MTRACE("WM_PAINT\n");
		  hdc = BeginPaint(hWnd, &ps);
      /*
      if (  (!virtualFullscreen) && (screenSize == 1)
          ||(virtualFullscreen && (videoSegSize[4]!=0)) )
      {
        line++;
        line = line%10;
        switch(line)
        {
        case 0:
          sprintf(msg,"vblint = %u",VBLInterruptCount);
          break;
        case 1:
          sprintf(msg,"chkvbl = %u",CheckVBLCount);
          break;
        case 2:
          sprintf(msg,"STblt = %u",STBLTCount);
          break;
        case 3:
          sprintf(msg,"Time = %u",GameTime);
          break;
        case 4:
          sprintf(msg,
                  "Skill[%d]=0x%08x      ",
                  MostRecentlyAdjustedSkills[0],
                  LatestSkillValues[0]);
          break;
        case 5:
          sprintf(msg,
                  "Skill[%d]=0x%08x      ",
                  MostRecentlyAdjustedSkills[1],
                  LatestSkillValues[1]);
          break;
        case 6:
          switch (latestCharType)
          {
          case TYPEIGNORED: 
            sprintf(msg, "%04x key --> Ignored                         ", latestCharp1);
            break;
          case TYPEKEY:
            sprintf(msg, "%04x key --> Translated %08x", latestCharp1, latestCharXlate);
            break;
          };
          break;
        case 7:
          switch (latestScanType)
          {
          case TYPESCAN:
          case TYPEIGNORED:
            sprintf(msg, "%08x mscan --> Ignored                        ",latestScanp1);
            break;
          case TYPEMSCANL:
            sprintf(msg, "%08x mscan --> Translated to %08x L",latestScanp1,latestScanXlate);
            break;
          case TYPEMSCANR:
            sprintf(msg, "%08x mscan --> Translated to %08x R",latestScanp1,latestScanXlate);
            break;
          default:
            sprintf(msg,"                                              ");
          };
          break;    
        case 8:
          switch (latestScanType)
          {
          case TYPEMSCANL:
          case TYPEIGNORED:
          case TYPEMSCANR:
            sprintf(msg, "%08x scan --> Ignored                        ",latestScanp2);
            break;
          case TYPESCAN:
            sprintf(msg, "%08x scan --> Translated to %08x", latestScanp2,latestScanXlate);
            break;
          default:
            sprintf(msg,"                                              ");
          };
          break;    
        case 9:
          sprintf(msg,"Total Moves = %d",totalMoveCount);
          break;
        };
        {
          i32 x, y;
          x = 325;
          y = 25;
          if (virtualFullscreen)
          {
            x = videoSegX[4];
            y = videoSegY[4];
          };
          y += 15 *line;
          {
            TEMPORARY_WIDE twide;
            twide = msg;
            ExtTextOut(hdc,x,y,ETO_OPAQUE,NULL,twide.Text(),strlen(msg),NULL);
          };
        };
      };
      */
      //BitmapDemo(hWnd);
      csbMessage.type=UIM_PAINT;
      if (CSBUI(&csbMessage) != UI_STATUS_NORMAL)
      {
        PostQuitMessage(0);
        break;
      };
			EndPaint(hWnd, &ps);
      hdc = 0;
			break;
    case WM_DESTROY:
      CommandBar_Destroy(g_hwndCB);
      PostQuitMessage(0);
      break;
    case WM_TIMER:
      MTRACE("WM_TIMER\n");
      csbMessage.type=UIM_TIMER;
      if (CSBUI(&csbMessage) != UI_STATUS_NORMAL)
      {
        PostQuitMessage(0);
        break;
      };
      MTRACE("   Returned\n");
      break;
    case WM_ERASEBKGND:
      MTRACE("WM_ERASEBKGND\n"); 
      csbMessage.type=UIM_REDRAW_ENTIRE_SCREEN;
      if (CSBUI(&csbMessage) != UI_STATUS_NORMAL)
      {
        PostQuitMessage(0);
        break;
      };
			return DefWindowProc(hWnd, message, wParam, lParam);
    case WM_MOUSEMOVE:
      MTRACE("WM_MOUSEMOVE\n");
      x=LOWORD(lParam);
      y=HIWORD(lParam);
      if (virtualFullscreen)
      {
        i32 x1=x, y1=y;
        TranslateFullscreen(x1,y1,x,y);
      }
      else
      {
        x = (x+screenSize/2)/screenSize;
        y = (y+screenSize/2)/screenSize;
      };
      inWindow = true;
      size = screenSize;
      if (y > 200) inWindow = false;
      if (y < 1)   inWindow = false;
      if (x > 320) inWindow = false;
      if (x < 1)   inWindow = false;
      if (GameMode == 1)
      {
        if (inWindow)
        {
          if (cursorIsShowing)
          {
            ShowCursor(false);
            cursorIsShowing = false;
          }
          else
          {
          };
        }
        else
        {
          if (cursorIsShowing)
          {
          }
          else
          {
            ShowCursor(true);
            cursorIsShowing = true;
          };
        };
      };
      if (GameMode != 1)
      {
        if (cursorIsShowing)
        {
        }
        else
        {
          ShowCursor(true);
          cursorIsShowing = true;
        };
      };
			return DefWindowProc(hWnd, message, wParam, lParam);
    case WM_KEYDOWN:
      MTRACE("WM_KEYDOWN\n"); 
      csbMessage.type=UIM_KEYDOWN;
      csbMessage.p1 = wParam; //virtual key
      csbMessage.p2 = (lParam>>16)&0xff; //scancode
      if (CSBUI(&csbMessage) != UI_STATUS_NORMAL)
      {
        PostQuitMessage(0);
        break;
      };
			return DefWindowProc(hWnd, message, wParam, lParam);
    case WM_KEYUP:
      MTRACE("WM_KEYUP\n"); 
			return DefWindowProc(hWnd, message, wParam, lParam);
    case WM_CHAR:
      MTRACE("WM_CHAR\n"); 
      csbMessage.type=UIM_CHAR;
      csbMessage.p1 = wParam;
      if (CSBUI(&csbMessage) != UI_STATUS_NORMAL)
      {
        PostQuitMessage(0);
        break;
      };
			return 0;// DefWindowProc(hWnd, message, wParam, lParam);
    case WM_INITMENUPOPUP:
      MTRACE("WM_INITMMENUPOPUP\n"); 
      switch (LOWORD(lParam))
      {
      case IDC_File: //File menu
        break;
	  case IDM_TOOLS_MENU:
		{
		  i32 flag;
      if (hidingTaskBar)flag = MF_CHECKED;
      else flag = MF_UNCHECKED;
      CheckMenuItem ((HMENU)wParam,
                     IDM_HideTaskBar,
                     MF_BYCOMMAND|flag);

      if (hidingTime)flag = MF_CHECKED;
      else flag = MF_UNCHECKED;
      CheckMenuItem ((HMENU)wParam,
                     IDM_HideTime,
                     MF_BYCOMMAND|flag);

//      if (overlappingText)flag = MF_CHECKED;
//      else flag = MF_UNCHECKED;
//      CheckMenuItem ((HMENU)wParam,
//                     IDM_OverlapText,
//                     MF_BYCOMMAND|flag);

		}
    case IDM_Misc: //Misc menu
    {
      i32 flag;


//          if (TimerTraceActive)flag = MF_CHECKED;
//          else flag = MF_UNCHECKED;
//          CheckMenuItem ((HMENU)wParam,
//                         IDC_TimerTrace,
//                         MF_BYCOMMAND|flag);

//          if (encipheredDataFile == NULL) flag = MF_ENABLED;
//          else flag = MF_GRAYED;
//          EnableMenuItem((HMENU)wParam,
//                         IDC_TimerTrace,
//                         MF_BYCOMMAND|flag);

      if (DM_rules)flag = MF_CHECKED;
      else flag = MF_UNCHECKED;
      CheckMenuItem ((HMENU)wParam,
                         IDM_DMRULES,
                         MF_BYCOMMAND|flag);

//          if (AttackTraceActive)flag = MF_CHECKED;
//          else flag = MF_UNCHECKED;
//          CheckMenuItem ((HMENU)wParam,
//                         IDC_AttackTrace,
//                         MF_BYCOMMAND|flag);

//          if (AITraceActive)flag = MF_CHECKED;
//          else flag = MF_UNCHECKED;
//          CheckMenuItem ((HMENU)wParam,
//                         IDC_AITrace,
//                         MF_BYCOMMAND|flag);

      flag = MF_GRAYED;
      if (BeginRecordOK) flag = MF_ENABLED;
      EnableMenuItem((HMENU)wParam,
                     IDC_Playback,
                     MF_BYCOMMAND | flag);

      flag = MF_GRAYED;
      if (ItemsRemainingOK
          && (encipheredDataFile==NULL)) flag = MF_ENABLED;
      EnableMenuItem((HMENU)wParam,
                     IDC_ItemsRemaining,
                     MF_BYCOMMAND | flag);

      flag = MF_GRAYED;
      if (ItemsRemainingOK
          && (encipheredDataFile==NULL)) flag = MF_ENABLED;
      EnableMenuItem((HMENU)wParam,
                     IDC_NonCSBItemsRemaining,
                     MF_BYCOMMAND | flag);

      flag = MF_UNCHECKED;
      if (PlayfileIsOpen()) flag = MF_CHECKED;
      CheckMenuItem((HMENU)wParam,
                    IDC_Playback,
                    MF_BYCOMMAND | flag);

      flag = MF_GRAYED;
      if (BeginRecordOK) flag = MF_ENABLED;
      EnableMenuItem((HMENU)wParam,
                     IDC_Record,
                     MF_BYCOMMAND | flag);
      flag = MF_UNCHECKED;
      if (RecordMenuOption) flag = MF_CHECKED;
      CheckMenuItem((HMENU)wParam,
                    IDC_Record,
                    MF_BYCOMMAND | flag);

      flag = MF_GRAYED;
      if (PlayfileIsOpen()) flag = MF_ENABLED;
      EnableMenuItem((HMENU)wParam,
                     IDC_QuickPlay,
                     MF_BYCOMMAND | flag);
      flag = MF_UNCHECKED;
      if (NoSpeedLimit!=0) flag = MF_CHECKED;
      CheckMenuItem((HMENU)wParam,
                    IDC_QuickPlay,
                    MF_BYCOMMAND | flag);

    };
    return 0;
      case IDM_Speed: //Speed Menu
        {
          i32 flag;
          if (gameSpeed==SPEED_GLACIAL)flag = MF_CHECKED;
          else flag = MF_UNCHECKED;
          CheckMenuItem ((HMENU)wParam,
                         IDM_Glacial,
                         MF_BYCOMMAND|flag);

          if (gameSpeed==SPEED_MOLASSES)flag = MF_CHECKED;
          else flag = MF_UNCHECKED;
          CheckMenuItem ((HMENU)wParam,
                         IDM_Molasses,
                         MF_BYCOMMAND|flag);

          if (gameSpeed==SPEED_VERYSLOW)flag = MF_CHECKED;
          else flag = MF_UNCHECKED;
          CheckMenuItem ((HMENU)wParam,
                         IDM_VerySlow,
                         MF_BYCOMMAND|flag);

          if (gameSpeed==SPEED_SLOW)flag = MF_CHECKED;
          else flag = MF_UNCHECKED;
          CheckMenuItem ((HMENU)wParam,
                         IDM_Slow,
                         MF_BYCOMMAND|flag);

          if (gameSpeed==SPEED_NORMAL)flag = MF_CHECKED;
          else flag = MF_UNCHECKED;
          CheckMenuItem ((HMENU)wParam,
                         IDM_Normal,
                         MF_BYCOMMAND|flag);

          if (gameSpeed==SPEED_QUICK)flag = MF_CHECKED;
          else flag = MF_UNCHECKED;
          CheckMenuItem ((HMENU)wParam,
                         IDM_Quick,
                         MF_BYCOMMAND|flag);

          if (gameSpeed==SPEED_FAST)flag = MF_CHECKED;
          else flag = MF_UNCHECKED;
          CheckMenuItem ((HMENU)wParam,
                         IDM_Fast,
                         MF_BYCOMMAND|flag);

          if (playerClock)flag = MF_CHECKED;
          else flag = MF_UNCHECKED;
          CheckMenuItem ((HMENU)wParam,
                         IDM_PlayerClock,
                         MF_BYCOMMAND|flag);
          if (extraTicks)flag = MF_CHECKED;
          else flag = MF_UNCHECKED;
          CheckMenuItem ((HMENU)wParam,
                         IDM_ExtraTicks,
                         MF_BYCOMMAND|flag);
        };
      case IDM_Volume:
          i32 flag;
          if (gameVolume==VOLUME_FULL)flag = MF_CHECKED;
          else flag = MF_UNCHECKED;
          CheckMenuItem ((HMENU)wParam,
                         IDM_VOLUME_FULL,
                         MF_BYCOMMAND|flag);

          if (gameVolume==VOLUME_HALF)flag = MF_CHECKED;
          else flag = MF_UNCHECKED;
          CheckMenuItem ((HMENU)wParam,
                         IDM_VOLUME_HALF,
                         MF_BYCOMMAND|flag);

          if (gameVolume==VOLUME_QUARTER)flag = MF_CHECKED;
          else flag = MF_UNCHECKED;
          CheckMenuItem ((HMENU)wParam,
                         IDM_VOLUME_QUARTER,
                         MF_BYCOMMAND|flag);

          if (gameVolume==VOLUME_EIGHTH)flag = MF_CHECKED;
          else flag = MF_UNCHECKED;
          CheckMenuItem ((HMENU)wParam,
                         IDM_VOLUME_EIGHTH,
                         MF_BYCOMMAND|flag);

          if (gameVolume==VOLUME_OFF)flag = MF_CHECKED;
          else flag = MF_UNCHECKED;
          CheckMenuItem ((HMENU)wParam,
                         IDM_VOLUME_OFF,
                         MF_BYCOMMAND|flag);

        break;
      case 4: 
        break;//Help menu
      };
			return DefWindowProc(hWnd, message, wParam, lParam);
    case WM_LBUTTONDOWN:
      MTRACE("WM_LBUTTONDOWN\n");
      csbMessage.type=UIM_LEFT_BUTTON_DOWN;
      csbMessage.p1 = LOWORD(lParam);  // horizontal position of cursor 
      csbMessage.p2 = HIWORD(lParam);  // vertical position of cursor 
      if (CSBUI(&csbMessage) != UI_STATUS_NORMAL)
      {
        PostQuitMessage(0);
        break;
      };
			return DefWindowProc(hWnd, message, wParam, lParam);
    case WM_LBUTTONUP:
      MTRACE("WM_LBUTTONUP\n");
      csbMessage.type=UIM_LEFT_BUTTON_UP;
      csbMessage.p1 = LOWORD(lParam);  // horizontal position of cursor 
      csbMessage.p2 = HIWORD(lParam);  // vertical position of cursor 
      if (CSBUI(&csbMessage) != UI_STATUS_NORMAL)
      {
        PostQuitMessage(0);
        break;
      };
			return DefWindowProc(hWnd, message, wParam, lParam);
    case WM_RBUTTONDOWN:
      MTRACE("WM_RBUTTONDOWN\n");
      csbMessage.type=UIM_RIGHT_BUTTON_DOWN;
      csbMessage.p1 = LOWORD(lParam);  // horizontal position of cursor 
      csbMessage.p2 = HIWORD(lParam);  // vertical position of cursor 
      if (CSBUI(&csbMessage) != UI_STATUS_NORMAL)
      {
        PostQuitMessage(0);
        break;
      };
			return DefWindowProc(hWnd, message, wParam, lParam);
    case WM_RBUTTONUP:
      MTRACE("WM_RBUTTONUP\n");
      csbMessage.type=UIM_RIGHT_BUTTON_UP;
      csbMessage.p1 = LOWORD(lParam);  // horizontal position of cursor 
      csbMessage.p2 = HIWORD(lParam);  // vertical position of cursor 
      if (CSBUI(&csbMessage) != UI_STATUS_NORMAL)
      {
        PostQuitMessage(0);
        break;
      };
			return DefWindowProc(hWnd, message, wParam, lParam);
    case WM_ACTIVATE:
            // Notify shell of our activate message
      SHHandleWMActivate(hWnd, wParam, lParam, &s_sai, FALSE);
        break;
    case WM_SETTINGCHANGE:
      SHHandleWMSettingChange(hWnd, wParam, lParam, &s_sai);
        break;
    default:
      return DefWindowProc(hWnd, message, wParam, lParam);
   }
   return 0;
}

HWND CreateRpCommandBar(HWND hwnd)
{
  SHMENUBARINFO mbi;

  memset(&mbi, 0, sizeof(SHMENUBARINFO));
  mbi.cbSize     = sizeof(SHMENUBARINFO);
  mbi.hwndParent = hwnd;
  mbi.nToolBarId = IDM_MENU;
  mbi.hInstRes   = hInst;
  mbi.nBmpId     = 0;
  mbi.cBmpImages = 0;

  if (!SHCreateMenuBar(&mbi)) 
    return NULL;

  return mbi.hwndMB;
}

// Mesage handler for the About box.
LRESULT CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
  SHINITDLGINFO shidi;

  switch (message)
  {
    case WM_INITDIALOG:
      // Create a Done button and size it.  
      shidi.dwMask = SHIDIM_FLAGS;
      shidi.dwFlags = SHIDIF_DONEBUTTON | SHIDIF_SIPDOWN | SHIDIF_SIZEDLGFULLSCREEN;
      shidi.hDlg = hDlg;
      SHInitDialog(&shidi);
      return TRUE; 


    case WM_LBUTTONDOWN:
      EndDialog(hDlg, LOWORD(wParam));
      return TRUE;

    case WM_COMMAND:
      if ((LOWORD(wParam) == IDOK) || (LOWORD(wParam) == IDC_OK))
      {
        EndDialog(hDlg, LOWORD(wParam));
        return TRUE;
      }
      break;

/*
    case WM_SETCURSOR:
    case WM_STYLECHANGED:
    case WM_WINDOWPOSCHANGED:
    case WM_MOVE:
    case WM_SIZE:
    case 0x0086:
    case WM_QUERYNEWPALETTE:
    case WM_ACTIVATE:
    case WM_PAINT:
    case WM_ERASEBKGND:
    case WM_CTLCOLORDLG:
    case WM_CTLCOLORSTATIC:
    case WM_CTLCOLORBTN:
    case WM_IME_SETCONTEXT:
    case WM_IME_NOTIFY:
    case WM_SETFOCUS:
    case WM_GETDLGCODE:
    case WM_MOUSEFIRST:
      break;
*/
    default:
      csbMessage.type = UIM_NOOP; //Do nothing
      if (CSBUI(&csbMessage) != UI_STATUS_NORMAL)
      {
        PostQuitMessage(0);
      };
      break;
  }
    return FALSE;
}


TEMPORARY_WIDE::TEMPORARY_WIDE(void)
{
  this->text = NULL;
}

TEMPORARY_WIDE::~TEMPORARY_WIDE(void)
{
  if (text != NULL) UI_free (text);
  text = NULL;
}

unsigned short *TEMPORARY_WIDE::Text(void)
{
  return text;
}

void TEMPORARY_WIDE::operator =(const char *atext)
{
  int i, len;
  len = strlen(atext);
  if (text != NULL) UI_free(text);
  text = (unsigned short *)UI_malloc(2*(len+1), MALLOC021);
  for (i=0; i<=len; i++)
  {
    text[i] = (unsigned)atext[i];
  };
}

TEMPORARY_CHAR::TEMPORARY_CHAR(void)
{
  this->text = NULL;
}

TEMPORARY_CHAR::~TEMPORARY_CHAR(void)
{
  if (text != NULL) UI_free (text);
  text = NULL;
}

char *TEMPORARY_CHAR::Text(void)
{
  return text;
}

void TEMPORARY_CHAR::operator =(unsigned short *wtext)
{
  int i, len;
  len = wcslen(wtext);
  if (text != NULL) UI_free(text);
  text = (char *)UI_malloc(len+1, MALLOC022);
  for (i=0; i<=len; i++)
  {
    text[i] = (unsigned)wtext[i];
  };
}
