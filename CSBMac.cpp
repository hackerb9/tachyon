#pragma pack(1)

/*

TAG002336() -> ScheduleSounds

33314 = 0x8000 | 546

546 = punch, 962 = length

#include <cstring>
#include <TextUtils.h>
#include <LowMem.h>
#include <DateTimeUtils.h>
#include <Timer.h>
#include <Dialogs.h>
#include <MacWindows.h>
#include <QuickDraw.h>
#include <Fonts.h>
#include <Menus.h>
#include <DrawSprocket.h>
#include <Sound.h>*/

#include <string.h>
#include "Transition.h"

bool fullscreenRequested = true;
bool fullscreenActive = false;
char* folderSavedGame="../Resources";

#if TARGET_RT_MAC_MACH_O
	#include <QD/QD.h>
	#include <DrawSprocket/DrawSprocket.h>
	#include <Carbon/Carbon.h>
	#include <mach/vm_map.h>
#else
	#include <DrawSprocket.h>
	#include <Timer.h>
	#include <Power.h>
	
	Rect* GetPortBounds( CGrafPtr a, Rect* b)
	{
		BlockMoveData( &a->portRect, b, sizeof( Rect ) );
		return b;
	}
#endif

#define CHAOSWINDOW_Y	40
#define FIRERATE	millisecondsPerVBL
#define LOWORD(x)	((x)&0xffff)
#define HIWORD(x) ((((long)(x))>>16L)&0xffff)

// CSBwin.cpp : Defines the entry point for the application.
//
char szCSBVersion[]="CSBVersion 9.0?";
#if TARGET_OS_MAC
	#if __MWERKS__
		#pragma reverse_bitfields	on
	#else
		#error All bitfields must be reversed in order to run!
	#endif
#endif
 
#include "stdafx.h"
#include "resource.h"
#include <stdio.h>
#include "UI.h"
#include "Objects.h"
#include "Dispatch.h"
#include "CSB.H"
#include "Data.h"

#define MAX_LOADSTRING 100

extern DBank d;
FILE *trace=NULL;
CSB_UI_MESSAGE csbMessage;
extern i32 screenSize;
extern bool BeginRecordOK;
extern bool PlayfileIsOpen(void);
extern bool RecordfileIsOpen(void);
bool TraceFileActive;
//extern int clockSpeed;
extern long GameMode;
extern long MostRecentlyAdjustedSkill;
//extern int LatestSkillValue;
extern i32 millisecondsPerVBL;
extern long NoSpeedLimit;
extern bool TimerTraceActive;
//extern FILE *TraceFile;


char *helpMessage = "CSBWin looks in three places for files:\n"
                    " 1) The default directory (Resources folder inside package)\n"
                    " 2) The parent of the default directory\n"
                    " 3) The directory of Chaos Strikes Back.app\n"
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

typedef struct {
	TMTask 	task;
	long		magic;
	Boolean 	fire;
} TimeTask;

/////////  pure mac global variables  ////////////
//

//	i32 				gDSpScreenWidth = 640;
	i32 				gDSpScreenWidth = 1024;
	bool 				gScreenIsInvalid;
	TimeTask			gTimer;
	Boolean				gRunning = true;
	DSpContextReference gContextRef;
	                                            //
//////////////////////////////////////////////////



HWND 		hWnd = NULL;

int 		line = 0;


static void initwackyoptions()
{
	NoSpeedLimit		=	0;
	speedTable[SPEED_GLACIAL].vblPerTick = 1000;
	speedTable[SPEED_MOLASSES].vblPerTick = 55;
	speedTable[SPEED_VERYSLOW].vblPerTick = 33;
	speedTable[SPEED_SLOW].vblPerTick = 22;
	speedTable[SPEED_NORMAL].vblPerTick = 15;
	speedTable[SPEED_FAST].vblPerTick = 11;
	speedTable[SPEED_QUICK].vblPerTick = 7;
}


bool InitInstance();
void DoChaosEventLoop();
void ExitInstance();
void WndProc(void*, unsigned short message, unsigned short wParam, unsigned long lParam);
static OSErr InitMacSound();
static void	 ExitMacSound();

extern 	void swoosh();
extern char* SmartDiscards[];
extern i32 AddSmartDiscard(char *buf);

char *foo[]={"hello","world",NULL};

int main(int argc, char** argv)
{

 	if (!InitInstance ()) 
	{
		return 0;
	}
	
	

	initwackyoptions();
	HideCursor();
	HideMenuBar();
	DoChaosEventLoop();
	MacShowCursor();
	ShowMenuBar();
	ExitInstance();
	
	return 0;
}



//


bool interrupt = 0;

pascal void TimerTask(TMTask* task)
{	
	TimeTask* tim = (TimeTask*)task;
	
	tim -> fire = 1;
	//interrupt = 1;
	//vblInterrupt();
	//interrupt = 0;
	PrimeTime((QElemPtr)task, millisecondsPerVBL);
}


bool InitInstance()
{
   Rect r;
   OSStatus err;
	DSpContextAttributes	a = {0,640,480,0,0,kDSpColorNeeds_Request,0,
											 0,
											 kDSpDepthMask_32,
											 kDSpDepthMask_32,
											 32,
											 32,
											 2,{0,0,0},false,{0,0,0,0}};
   
    vm_size_t smDSize = 1024;

    screenSize = 2;
    
   //  make SmartDiscards writeable
   vm_protect(mach_task_self(), (unsigned int)(*SmartDiscards), smDSize, 0, VM_PROT_READ|VM_PROT_WRITE);

   
   InitCursor();
   SetRect(&r,0,0,320,200);
   DSpStartup();
   
   memset(&d,0,sizeof(d));
   memset(&a,0,sizeof(a));
   a.displayWidth = gDSpScreenWidth;
   a.displayHeight = gDSpScreenWidth/4*3;
   a.colorNeeds = 0;
   a.backBufferDepthMask = 32;
   a.displayDepthMask = 32;
   a.backBufferBestDepth = 32;
   a.displayBestDepth = 32; 
   a.pageCount = 1;
   err = DSpFindBestContext(&a,&gContextRef);
   if(err)
   {
   	DSpShutdown();
   	ExitToShell();
   }
   
   err = InitMacSound();
   if(err)
   {
   	DSpShutdown();
   	ExitToShell();
   }
   
   
   	err = DSpSetDebugMode(true);
   	err = DSpContext_Reserve(gContextRef, &a);
	//err = DSpContext_FadeGammaOut(gContextRef, NULL);
#pragma message("Uncomment this")

if( gDSpScreenWidth == 1024)
{
    Rect r = {40,0,440,640};
    WindowRef w;
    CreateNewWindow(kPlainWindowClass,0,&r,&w);
    ShowWindow(w);
}
else
{
	err = DSpContext_SetState(gContextRef, kDSpContextState_Active);
}
   	//err = DSpContext_FadeGammaIn(gContextRef, NULL);
	
	//macwindows.h
   /*gCSBWindow = NewCWindow(NULL,&r,"\pChaos Strikes Back",false,noGrowDocProc,(WindowPtr)-1L,true,NULL);
   
   if (!gCSBWindow)
      return false;
	
	MoveWindow(gCSBWindow,50,50,true);
	ShowWindow(gCSBWindow);
    SetPort(gCSBWindow);
	*/
	
	WndProc(0,WM_CREATE,0,0);
  return true;
}

void ExitInstance()
{
	OSStatus err;
	
	RemoveTimeTask((QElemPtr)&gTimer);
	//DisposeWindow(gCSBWindow);
	err = DSpContext_FadeGammaOut(gContextRef, NULL);
	err = DSpContext_SetState(gContextRef, kDSpContextState_Inactive);
    err = DSpContext_FadeGammaIn(gContextRef, NULL);
	err = DSpContext_Release(gContextRef);
	DSpShutdown();
	ExitMacSound();
}

int delta[60];
int prevms = -1;
int slot;
void display(void);
extern unsigned int VBLInterruptCount;
extern unsigned int CheckVBLCount;
extern unsigned int STBLTCount;
extern i32 GameTime;

bool cursorIsShowing = true;

void PostQuitMessage(short)
{
	gRunning = false;
}
extern void swoosh();

/*static void MyShowHideMenuBar()
{
	Point		pt;
	static 	i32 hidden = 1;
	
	DSpGetMouse(&pt);
	if(pt.v < 20)
	{
		if(hidden) 
		{
			DSpContext_SetState(gContextRef, kDSpContextState_Paused);
			ShowMenuBar();
			MacDrawMenuBar();
			WndProc(hWnd, WM_ERASEBKGND, 0, 0);
			hidden = 0;
		}
		gScreenIsInvalid = true;
	}
	else
	{
		if(!hidden)
		{
   			HideMenuBar();
			DSpContext_SetState(gContextRef, kDSpContextState_Active);
			WndProc(hWnd, WM_ERASEBKGND, 0, 0);
			hidden = 1;
		}
	}
}
*/

extern bool INTERLACED;
		
void DoChaosEventLoop()
{
	EventRecord 	theEvent;
	Boolean			gotEvent;
	CGrafPtr		chaosPort;
	Rect			bounds;
	OSStatus		err = noErr;
	Boolean			lastMouseDownEventWasLeftMouse, hasIdle;
	
/*	RgnHandle		chaosWindowRegion, restOfScreenRegion;
	Rect			mainScreenRect, chaosWindowRect;
		
	restOfScreenRegion = NewRgn();
	err = QDError();
	
	if(!err)
	{
		chaosWindowRegion = NewRgn();
		err = QDError();
	}
	
	if(!err)
	{
		RectRgn(restOfScreenRegion,GetPixBounds((*LMGetMainDevice())->gdPMap, &mainScreenRect));
		err = QDError();
	}  
	
	if(!err)
	{
		MacSetRect(&chaosWindowRect, 0, CHAOSWINDOW_Y, 640, CHAOSWINDOW_Y+400);  
		RectRgn(chaosWindowRegion, &chaosWindowRect);
		err = QDError();
	}
	
	if(!err)
	{
		DiffRgn(restOfScreenRegion, chaosWindowRegion, restOfScreenRegion);
		err = QDError();
	}
*/
	
	if(!err)
	{
		err = DSpContext_GetBackBuffer(gContextRef,kDSpBufferKind_Normal,&chaosPort);
	}
	
	if(!err)
	{
		MacSetPort((GrafPtr)chaosPort);
		BackColor(blackColor);
		ForeColor(whiteColor);
		EraseRect(GetPortBounds(chaosPort, &bounds));
		err = DSpContext_SwapBuffers(gContextRef,NULL,NULL);
	}
	
	if(!err)
	{
		long attr;
		if(Gestalt(gestaltPowerMgrAttr,&attr)==0)
			hasIdle = (attr & ( (1L << gestaltPMgrExists) | (1L << gestaltPMgrCPUIdle))) == ( (1L << gestaltPMgrExists) | (1L << gestaltPMgrCPUIdle));
		
	}
	
	SetEventMask(mDownMask|mUpMask|autoKeyMask|keyDownMask|keyUpMask|updateMask);
	
	swoosh();
		
	//WndProc(0,IDC_DispatchTrace,0,0);
	//WndProc(0,IDC_TimerTrace,0,0);


	gTimer.fire = 0;
	gTimer.magic = 'tomz';   
	gTimer.task.tmAddr = NewTimerUPP(TimerTask);
	gTimer.task.tmReserved = NULL;
	InstallXTimeTask((QElemPtr)&gTimer);
    PrimeTime((QElemPtr)&gTimer,millisecondsPerVBL);
 
 
	while(gRunning)
	{
		EventRecord oldEvent;
		
		gotEvent = WaitNextEvent(everyEvent, &theEvent, 0, NULL);
		//MyShowHideMenuBar();
		
		if(hasIdle)
		{
			//  we don't want to cycle and slow down
		
			IdleUpdate();
		}
		
		if(gTimer.fire)
		{
			gTimer.fire = 0;
			WndProc(NULL,WM_TIMER,0,0);
		}
		
		if(gScreenIsInvalid)
		{
			WndProc(NULL,WM_PAINT,0,0);
			gScreenIsInvalid = 0;
		}
		
		theEvent.where.v -= 40;
		
		if(theEvent.where.h != oldEvent.where.h || theEvent.where.v != oldEvent.where.v)
		{
			WndProc(NULL,WM_MOUSEMOVE,0,(((unsigned long)theEvent.where.v)<<16L)|theEvent.where.h);
		}
		
		oldEvent = theEvent;
		if(!gotEvent) continue;
		
		//if(TraceFile) flush(TraceFile);
    
		
		switch(theEvent.what)
		{
			case updateEvt:
				WndProc(NULL,WM_ERASEBKGND,0,0);				
				break;
				
			case autoKey:
			case keyDown:
				
				switch((theEvent.message>>8)&0xFF)
				{
				    case    0x7A:
				        theEvent.message = 0x70;
				        break; 
				
				    case    0x78:
				        theEvent.message = 0x71;
				        break;

				    case    0x63:
				        theEvent.message = 0x72;
				        break;

				    case    0x76:
				        theEvent.message = 0x73;
				        break;
				        
				    default:
				        break;
				}
				
				WndProc(NULL,WM_KEYDOWN,(theEvent.message)&0xff, (theEvent.message&0xffff) | ((long)theEvent.modifiers)<<16L);				
				
				
				if(((theEvent.message & 0xff) == 'q') && (theEvent.modifiers & cmdKey))
				{
					WndProc(NULL,WM_TIMER,0,0);				
					WndProc(NULL,WM_DESTROY,0,(((unsigned long)theEvent.where.v)<<16L)|theEvent.where.h);				
					PostQuitMessage(0);
				}
				if(((theEvent.message & 0xff) == 'i') && (theEvent.modifiers & cmdKey))
				{
					INTERLACED ^= 1;
					WndProc(hWnd, WM_ERASEBKGND, 0, 0);
				}
				
				break;
			
			case mouseDown:
				lastMouseDownEventWasLeftMouse = (theEvent.modifiers & controlKey) != controlKey;  
				
				if(lastMouseDownEventWasLeftMouse)
					WndProc(NULL,WM_LBUTTONDOWN,0,(((unsigned long)theEvent.where.v)<<16L)|theEvent.where.h);				
				else
					WndProc(NULL,WM_RBUTTONDOWN,0,(((unsigned long)theEvent.where.v)<<16L)|theEvent.where.h);				
				
				break;
		
			case mouseUp:
				if(lastMouseDownEventWasLeftMouse)
					WndProc(NULL,WM_LBUTTONUP,0,(((unsigned long)theEvent.where.v)<<16L)|theEvent.where.h);				
				else	
					WndProc(NULL,WM_RBUTTONUP,0,(((unsigned long)theEvent.where.v)<<16L)|theEvent.where.h);				
				
				break;
		
			case app4Evt:
				if((theEvent.message>>24L)!=0xFA) break;
				//GlobalToLocal(&theEvent.where);
				WndProc(NULL,WM_NCMOUSEMOVE,0,(((unsigned long)theEvent.where.v)<<16L)|theEvent.where.h);				
				break;
		
		}
		
	}
	
	//if(chaosWindowRegion) 	DisposeRgn(chaosWindowRegion);
	//if(restOfScreenRegion) 	DisposeRgn(restOfScreenRegion);
}



//
//  FUNCTION: WndProc(HWND, unsigned, WORD, LONG)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//

void WndProc(void*, unsigned short message, unsigned short wParam, unsigned long lParam)
{
  char msg[100];

  static int x,y;
  static bool inWindow;


  if (trace!=NULL) fprintf(trace,"msg=");
  if (trace!=NULL) fflush(trace);

  switch (message) 
	{
		/*case WM_COMMAND:
      if (trace!=NULL) fprintf(trace,"WM_COMMAND\n");
			wmId    = LOWORD(wParam); 
			wmEvent = HIWORD(wParam); 
			// Parse the menu selections:
/*			switch (wmId)
			{
				case IDM_ABOUT:
				    DialogBox(hInst, (LPCTSTR)IDD_ABOUTBOX, hWnd, (DLGPROC)About);
				    break;
				case IDM_EXIT:
            if (trace!=NULL) fclose(trace);
				    DestroyWindow(hWnd);
				    break;
*/
        case IDC_Double:
            csbMessage.type = UIM_SETOPTION;
            csbMessage.p1 = OPT_DOUBLE;
            csbMessage.p2 = 3-screenSize; //new value
            if (CSBUI(&csbMessage) != UI_STATUS_NORMAL)
            {
              PostQuitMessage(0);
              break;
            };
            break;

/*        case IDC_Clock:
            csbMessage.type = UIM_SETOPTION;
            csbMessage.p1 = OPT_CLOCK;
            csbMessage.p2 = 10000/clockSpeed; //new value
            if (CSBUI(&csbMessage) != UI_STATUS_NORMAL)
            {
              PostQuitMessage(0);
              break;
            };
            break;*/
            
        case IDC_Record:
            csbMessage.type = UIM_SETOPTION;
            csbMessage.p1 = OPT_RECORD;
            csbMessage.p2 = 1;
            if (RecordfileIsOpen()) csbMessage.p2 = 0;
            if (CSBUI(&csbMessage) != UI_STATUS_NORMAL)
            {
              PostQuitMessage(0);
              break;
            };
            break;
        case IDC_TimerTrace:
            csbMessage.type = UIM_SETOPTION;
            csbMessage.p1 = OPT_TIMERTRACE;
            csbMessage.p2 = 1;
            if (TraceFileActive) csbMessage.p2 = 0;
            if (CSBUI(&csbMessage) != UI_STATUS_NORMAL)
            {
              PostQuitMessage(0);
              break;
            };
            break;
        case IDC_Playback:
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
        
        case IDC_DispatchTrace:
            if (trace != NULL) 
            {
              fclose(trace);
              break;
            };
            trace = UI_fopen("trace.log","w");
            break;	
		

		case WM_PAINT:
		      if (trace!=NULL) fprintf(trace,"WM_PAINT\n");
		      if (trace!=NULL) fflush(trace);
		      if (screenSize == 1)
		      {
		        line++;
		        line = line%5;
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
		          /*sprintf(msg,
		                  "Skill[%d]=0x%08x      ",
		                  MostRecentlyAdjustedSkill,
		                  LatestSkillValue);
		          */
		          break;
		        };
		//        TextOut(hdc,325,25+15*line,msg,strlen(msg));
		      };
		      csbMessage.type=UIM_PAINT;
		      if (CSBUI(&csbMessage) != UI_STATUS_NORMAL)
		      {
		        PostQuitMessage(0);
		        break;
		      };
			break;
    case WM_TIMER:
      if (trace!=NULL) fprintf(trace,"WM_TIMER");
      if (trace!=NULL) fflush(trace);
      csbMessage.type=UIM_TIMER;
      if (CSBUI(&csbMessage) != UI_STATUS_NORMAL)
      {
        PostQuitMessage(0);
        break;
      };
      if (trace!=NULL) fprintf(trace,"   Returned\n");
      if (trace!=NULL) fflush(trace);
      break;

        
    case WM_DESTROY:
      if (trace!=NULL) fprintf(trace,"WM_DESTROY\n"); 
      csbMessage.type=UIM_TERMINATE;
      if (CSBUI(&csbMessage) != UI_STATUS_NORMAL)
      {
        PostQuitMessage(0);
        break;
      };
	
	case WM_CREATE:
      if (trace!=NULL) fprintf(trace,"WM_CREATE\n"); 
      csbMessage.type=UIM_INITIALIZE;
      if (CSBUI(&csbMessage) != UI_STATUS_NORMAL)
      {
        PostQuitMessage(0);
        break;
      };
	
    case WM_ERASEBKGND:
      if (trace!=NULL) fprintf(trace,"WM_ERASEBKGND\n"); 
      csbMessage.type=UIM_REDRAW_ENTIRE_SCREEN;
      if (CSBUI(&csbMessage) != UI_STATUS_NORMAL)
      {
        PostQuitMessage(0);
        break;
      };

    case WM_MOUSEMOVE:
      if (trace!=NULL) fprintf(trace,"WM_MOUSEMOVE\n");
      x=LOWORD(lParam);
      y=HIWORD(lParam);
      inWindow = true;
      if (y > 200*screenSize) inWindow = false;
      if (y < 1)              inWindow = false;
      if (x > 320*screenSize) inWindow = false;
      if (x < 1)              inWindow = false;
      /*if (GameMode == 1)
      {
        if (inWindow)
        {
          if (cursorIsShowing)
          {
            MacShowCursor();
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
            MacShowCursor();
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
         MacShowCursor();
          cursorIsShowing = true;
        };
      };*/
      
//			return DefWindowProc(hWnd, message, wParam, lParam);
  
  	   break;
    case WM_KEYDOWN:
      if (trace!=NULL) fprintf(trace,"WM_KEYDOWN\n"); 
      csbMessage.type=UIM_KEYDOWN;
      csbMessage.p1 = wParam; //virtual key
      csbMessage.p2 = (lParam>>8)&0xff; //scancode
      csbMessage.p3 = lParam>>16; //modifiers
      if (CSBUI(&csbMessage) != UI_STATUS_NORMAL)
      {
        PostQuitMessage(0);
        break;
      };
		//	return DefWindowProc(hWnd, message, wParam, lParam);
    	break;
    	
    case WM_CHAR:
      if (trace!=NULL) fprintf(trace,"WM_CHAR\n"); 
      csbMessage.type=UIM_CHAR;
      csbMessage.p1 = wParam;
      if (CSBUI(&csbMessage) != UI_STATUS_NORMAL)
      {
        PostQuitMessage(0);
        break;
      };

    case WM_NCMOUSEMOVE:
      if (trace!=NULL) fprintf(trace,"WM_NCMOUSEMOVE\n"); 
      if (cursorIsShowing)
      {
      }
      else
      {
        STShowCursor();
        cursorIsShowing = true;
      };
  		break;

/*    case WM_INITMENUPOPUP:
      if (trace!=NULL) fprintf(trace,"WM_INITMMENUPOPUP\n"); 
      switch (LOWORD(lParam))
      {
      case 0: //File menu
        break;
      case 1: //Misc menu
        {
          int flag;
          if (clockSpeed==1000)flag = MF_CHECKED;
          else flag = MF_UNCHECKED;
          CheckMenuItem ((HMENU)wParam,
                         IDC_Clock,
                         MF_BYCOMMAND|flag);

          if (screenSize==2)flag = MF_CHECKED;
          else flag = MF_UNCHECKED;
          CheckMenuItem ((HMENU)wParam,
                         IDC_Double,
                         MF_BYCOMMAND|flag);

          if (TraceFileActive)flag = MF_CHECKED;
          else flag = MF_UNCHECKED;
          CheckMenuItem ((HMENU)wParam,
                         IDC_TimerTrace,
                         MF_BYCOMMAND|flag);

          flag = MF_GRAYED;
          if (BeginRecordOK) flag = MF_ENABLED;
          EnableMenuItem((HMENU)wParam,
                         IDC_Playback,
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
          if (RecordfileIsOpen()) flag = MF_CHECKED;
          CheckMenuItem((HMENU)wParam,
                        IDC_Record,
                        MF_BYCOMMAND | flag);
        };
        return 0;
      case 2: 
        break;//Help menu
      };
			return DefWindowProc(hWnd, message, wParam, lParam);
    case WM_ENTERIDLE:
      if (trace!=NULL) fprintf(trace,"WM_ENTERIDLE\n"); 
			return DefWindowProc(hWnd, message, wParam, lParam);
    case WM_CAPTURECHANGED:
      if (trace!=NULL) fprintf(trace,"WM_CAPTURECHANGED\n"); 
			return DefWindowProc(hWnd, message, wParam, lParam);
    case WM_EXITMENULOOP:
      if (trace!=NULL) fprintf(trace,"WM_EXITMENULOOP\n"); 
			return DefWindowProc(hWnd, message, wParam, lParam);
    case WM_NCLBUTTONDOWN:
      if (trace!=NULL) fprintf(trace,"WM_NCLBUTTONDOWN\n"); 
			return DefWindowProc(hWnd, message, wParam, lParam);
    case WM_CLOSE:
      if (trace!=NULL) fprintf(trace,"WM_CLOSE\n"); 
			return DefWindowProc(hWnd, message, wParam, lParam);
    case WM_SIZING:
      if (trace!=NULL) fprintf(trace,"WM_SIZING\n");
			return DefWindowProc(hWnd, message, wParam, lParam);
    case WM_ENTERSIZEMOVE:
      if (trace!=NULL) fprintf(trace,"WM_ENTERSIZEMOVE\n");
			return DefWindowProc(hWnd, message, wParam, lParam);
*/
    case WM_LBUTTONDOWN:
      if (trace!=NULL) fprintf(trace,"WM_LBUTTONDOWN\n");
      csbMessage.type=UIM_LEFT_BUTTON_DOWN;
      csbMessage.p1 = LOWORD(lParam);  // horizontal position of cursor 
      csbMessage.p2 = HIWORD(lParam);  // vertical position of cursor 
      if (CSBUI(&csbMessage) != UI_STATUS_NORMAL)
      {
        PostQuitMessage(0);
        break;
      };
		break;
    
    case WM_LBUTTONUP:
      if (trace!=NULL) fprintf(trace,"WM_LBUTTONUP\n");
      csbMessage.type=UIM_LEFT_BUTTON_UP;
      csbMessage.p1 = LOWORD(lParam);  // horizontal position of cursor 
      csbMessage.p2 = HIWORD(lParam);  // vertical position of cursor 
      if (CSBUI(&csbMessage) != UI_STATUS_NORMAL)
      {
        PostQuitMessage(0);
        break;
      };
		break;
		
    case WM_RBUTTONDOWN:
      if (trace!=NULL) fprintf(trace,"WM_RBUTTONDOWN\n");
      csbMessage.type=UIM_RIGHT_BUTTON_DOWN;
      csbMessage.p1 = LOWORD(lParam);  // horizontal position of cursor 
      csbMessage.p2 = HIWORD(lParam);  // vertical position of cursor 
      if (CSBUI(&csbMessage) != UI_STATUS_NORMAL)
      {
        PostQuitMessage(0);
        break;
      };
		break;
		
    case WM_RBUTTONUP:
      if (trace!=NULL) fprintf(trace,"WM_RBUTTONUP\n");
      csbMessage.type=UIM_RIGHT_BUTTON_UP;
      csbMessage.p1 = LOWORD(lParam);  // horizontal position of cursor 
      csbMessage.p2 = HIWORD(lParam);  // vertical position of cursor 
      if (CSBUI(&csbMessage) != UI_STATUS_NORMAL)
      {
        PostQuitMessage(0);
        break;
      };
		break;
		
		default:
      if (trace!=NULL) fprintf(trace,"0x%02x\n",message);
			break;
   };
   if (trace!=NULL) fflush(trace);
   return ;
}



//LMGetScrnBase
/*
char* strupr(char* str)
{
	UpperText(str,strlen(str));
	return str;
}



bool UI_DeleteFile(const char* fname)
{
	short err;
	err = remove(fname);
	return err == noErr;
}

void STSleep(long sleep)
{
	unsigned long ggg;
	Delay(sleep,&ggg);
}

void UI_GetCursorPos(int *x, int *y)
{
  Point pt;
  DSpGetMouse(&pt);
  *x = pt.h/screenSize;
  *y = pt.v/screenSize;
}

void UI_Invalidate()
{
	WndProc(0,WM_PAINT,0,0);
}*/

SndChannelPtr	gSoundChannel = nil;
SndCallBackUPP	gSoundCompletion;

static pascal void MacSoundProc(SndChannelPtr chan, SndCommand *cmd)
{
	#pragma unused(chan)
	Debugger();
	((Boolean*)(cmd -> param2))[0] = true;
}

static OSErr InitMacSound()
{

	OSErr	err = noErr;
	
	gSoundCompletion = NewSndCallBackUPP(MacSoundProc);
	if(!gSoundCompletion) return memFullErr;
	
	err = SndNewChannel(&gSoundChannel, sampledSynth, initMono, gSoundCompletion);
	if(err != noErr)
	{
		DisposeSndCallBackUPP(gSoundCompletion);
		gSoundCompletion = nil;
	}
	
	return err;		
}


static void ExitMacSound()
{
	if(gSoundChannel)
	{
		UI_PlaySound(NULL,SND_SYNC);
		
		SndDisposeChannel(gSoundChannel,true);
		gSoundChannel = nil;
	}
	
	if(gSoundCompletion)
	{
		DisposeSndCallBackUPP(gSoundCompletion);
		gSoundCompletion = nil;
	}
}