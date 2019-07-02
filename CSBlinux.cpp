// CSBlinux.cpp : Linux-specific file instead of windows-specific CSBwin.cpp.
// Note: CSBTypes.h needs to be included after stdafx.h
// Check LinScreen.cpp for comments.
#include "stdafx.h"
#include "UI.h"
#include "resource.h"
#include <stdio.h>

#include "CSBTypes.h"

#include "Dispatch.h"
#include "Objects.h"
#include "CSB.h"
#include "Data.h"

#ifdef USE_PARAGUI
# include <pgapplication.h>
# include <pgbutton.h>
  const int WIDTH  = 640;
  const int HEIGHT = 480;
#endif//USE_PARAGUI

#define APPTITLE  "CSBwin"
#define APPVERSION  "15.7"
#define APPVERMINOR "v0" /* linux-only update*/

//extern gboolean noDirtyRect;
//extern SDL_Rect DirtyRect;

extern bool RecordMenuOption;
extern bool DMRulesDesignOption;
extern bool RecordDesignOption;
extern bool extendedPortraits;
bool fullscreenActive;
extern bool simpleEncipher; //not used in Linux
extern unsigned char *encipheredDataFile; //not used in Linux

static SDL_Event evert;
bool sdlQuitPending = false;
bool timerInQueue = false;

int absMouseX;  // screen resolution; you must divide by screenSize
int absMouseY;  //    to get position on ST screen.

#if defined SDL12
int screenWidth, screenHeight;
#endif


#if defined SDL20
SDL_Window *sdlWindow = NULL;
SDL_Texture *sdlTexture = NULL;
SDL_Renderer *sdlRenderer = NULL;
int SDL_VIDEOEXPOSE = 0;
#endif


#ifdef MAEMO_NOKIA_770
# include "hildon-widgets/hildon-program.h"
# include "hildon-widgets/hildon-window.h"

  HildonProgram *program = NULL;
  HildonWindow *hildonmainwindow = NULL;
#endif//MAEMO_NOKIA_770

void PostQuitMessage( int a ) {
    SDL_Event event;
//    FILE *f;
//    f = fopen("debug","a");
//    fprintf(f,"PostQuitMessage(0x%x)\n", a);
//    fclose(f);
    while (SDL_PollEvent(&event) != 0){};
    event.type = SDL_QUIT;
    SDL_PushEvent(&event);
    sdlQuitPending = true;
//    SDL_Quit ();
#ifdef USE_OLD_GTK
    gtk_exit (0);
#endif //USE_OLD_GTK
//    exit(0);
}

#define IDC_Timer 1

/*
static void DumpEvents(void)
{
  FILE *f;
  SDL_Event ev;
  f = fopen("debug","a");
  fprintf(f, "%s\n", SDL_GetError());
  while (SDL_PollEvent(&ev) != 0)
  {
    if (ev.type = SDL_USEREVENT)
    {
      fprintf(f, "SDL_USEREVENT type = %d\n", ev.user.code);
    }
    else
    {
      fprintf(f, "Event type= %d\n", ev.type);
    };
  };
  fclose(f);
}
*/

bool videoExposeWaiting = false;

static guint32 __Timer_Callback(guint32 interval, void *param) 
{
  int code;
  code = GPOINTER_TO_INT(param);
  if (code == IDC_Timer)
  {
    if(timerInQueue)
    {
      // We will add an IDC_Timer only when there is nothing
      // else to do.
      return interval;
    }
    else
    {
      // The queue is empty.  We can add an IDC_Timer and
      // (perhaps) an IDC_VIDEOEXPOSE.
      SDL_Event ev;
      if (videoExposeWaiting)
      {
        ev.type = SDL_USEREVENT;
        ev.user.code = IDC_VIDEOEXPOSE;
        if (SDL_PushEvent(&ev) < 0)
        {
//           DumpEvents();
          die(0x66a9);
        };
        videoExposeWaiting = false;
      };
      {
        ev.type = SDL_USEREVENT;
        ((SDL_UserEvent*) &ev)->code = IDC_Timer;
        if (SDL_PushEvent(&ev) < 0)
        {
          char line[80];
          sprintf(line," code = %d", GPOINTER_TO_INT(param));
          UI_MessageBox(SDL_GetError(), "PushEvent Failed",MESSAGE_OK);
          die(0x66aa);
        };
        timerInQueue = true;
        videoExposeWaiting = false;
      };
      return interval;
    };
  }
  else
  {
    PostQuitMessage(0xbab3);
  };
}


static void PushEvent(void *param) 
{
  SDL_Event ev;
  {
    int code;
    code = GPOINTER_TO_INT(param);
#if defined  SDL12
    if ((code == SDL_VIDEOEXPOSE) || (code == IDC_VIDEOEXPOSE))
    {
      if (SDL_PollEvent(NULL) > 0)
#elif defined SDL20
    if (code == IDC_VIDEOEXPOSE)
    {
      if (SDL_PeepEvents(&ev,1,SDL_PEEKEVENT,
                     SDL_FIRSTEVENT,SDL_LASTEVENT) >0)
#else
      xxerror
#endif
      {
        // We will add a IDC_VIDEOEXPOSE only when there is
        // nothing else to do.
        videoExposeWaiting = true;
        return;
      };
    };
  };
  ev.type = SDL_USEREVENT;
  ((SDL_UserEvent*) &ev)->code = GPOINTER_TO_INT(param);
  if (SDL_PushEvent(&ev) < 0)
  {
    char line[80];
    sprintf(line," code = %d", GPOINTER_TO_INT(param));
    UI_MessageBox(SDL_GetError(), "PushEvent Failed",MESSAGE_OK);
    UI_Die(0x66ab);
  };
  return;
}


static void __resize_screen( ui32 w, i32 h ) {
  SDL_Event ev;
#if defined SDL12
  //ev.type = SDL_VIDEORESIZE;
  //((SDL_ResizeEvent*)&ev) -> w = w;
  //((SDL_ResizeEvent*)&ev) -> h = h;
#elif defined SDL20
  ev.type = IDC_VIDEORESIZE;
  ev.user.data1 = GINT_TO_POINTER(w);
  ev.user.data2 = GINT_TO_POINTER(h);
#else
  xxxError
#endif
  //SDL_PushEvent(&ev);
}
/*
* There IS a videoexpose event in linux, but apparently SDL
* wants to handle those for itself... so we cannot use 'invalidate'
* the obvious way. UI_Invalidate just marks a request to
* update the screen... LIN_Invalidate is called from vbl and pretends
* to be the OS that grants the update.
*/

bool GetVideoRectangle(i32, RECT *);
static bool pending_update = true;

void UI_Invalidate(bool erase/*=false*/) {
  pending_update = true;
}
void LIN_Invalidate()
{// *screen, x, y, w, h
  if( pending_update ) {
    PushEvent(GINT_TO_POINTER(IDC_VIDEOEXPOSE) );
  }
}




#define MAX_LOADSTRING 100



i32 trace=-1;
CSB_UI_MESSAGE csbMessage;
#ifdef _DYN_WINSIZE
//i32 screenSize=2;
#else
//i32 screenSize=2;
#endif

extern bool BeginRecordOK;
extern bool ItemsRemain;
extern bool PlayfileIsOpen(void);
extern bool RecordfileIsOpen(void);
extern bool TimerTraceActive;
extern bool AttackTraceActive;
extern bool AITraceActive;
extern bool PlaybackCommandOption;
extern unsigned char *encipheredDataFile;// Oh?

ui32 TImER=0;
extern i32 NoSpeedLimit;
extern i32 GameMode;
extern i32 MostRecentlyAdjustedSkills[2];
extern i32 LatestSkillValues[2];
extern XLATETYPE latestScanType;
extern XLATETYPE latestCharType;
extern i32 latestCharp1;
extern i32 latestScanp1;
extern i32 latestScanp2;
extern i32 latestScanXlate;
extern i32 latestCharXlate;
const char *helpMessage = "CSBlinux will need the orginal datafiles "
      "for either Chaos Strikes Back,\n"
      "or Dungeon Master in your current working directory.\n"
      "If the files are located somewhere else try\n --help for some usefull comandlinde arguments.\n"
      "\nMake sure that you have these files\n"
      "(and that they are spelled with lower case)\n"
      "   dungeon.dat\n"
      "   hcsb.dat\n"
      "   hcsb.hct\n"
      "   mini.dat\n"
      "   graphics.dat\n"
      "   config.linux";

/* This is a bad place for these id's */

void MTRACE(const char *msg)
{
  if (trace < 0) return;
  FILE *f = GETFILE(trace);
  fprintf(f, "%s", msg);
  fflush(f);
}

void UI_GetCursorPos(i32 *_x, i32 *_y)
{
  *_x = X_TO_CSB(absMouseX,screenSize);
  *_y = Y_TO_CSB(absMouseY,screenSize);
}



char szCSBVersion[] = "CSB for Windows/Linux Version " APPVERSION;
int WindowWidth = 640;
int WindowHeight = 400;
int WindowX = 0;
int WindowY = 0;
float st_X = 320.0 / 640.0;
float st_Y = 200.0 / 400.0;
bool fullscreenRequested = false;


i32 line = 0;

// Global Variables:
SDL_Surface *WND = NULL; //defined in UI.h
SDL_TimerID timer;

#ifdef _DYN_WINSIZE
HermesHandle from_palette;
HermesHandle to_palette;
HermesHandle converter;
HermesFormat* from_format;
#else
SDL_Surface *SCRAP = NULL;
#endif


#ifdef USE_OLD_GTK
GtkWidget *appGlobal;   /* Main Application Handle (menu et al) */
GSList    *radio_size_grp, *radio_speed_grp;       /* a list of  radio-'buttons' in the menu*/

static void
appAbout (GtkWidget *w, gpointer data)
{
 GtkWidget *dlg;
 const char *authors[] = {"It was orginally developed for the Atari ST by FTL Software:",
        "Director ~ ~ ~ ~ Doug Bell",
        "asst.director ~ Dennis Walker",
        "2nd unit director ~ ~ Mike Newton",
        "graphics ~ ~ Andy Jaros",
        "graphics ~ ~ David Simon",
        "producer ~ ~ Wayne Holder", NULL};
 const char *comment= "Port from the ST to windows by\n  Paul R. Stevens\n"
        " \nAdditional port from Windows to Linux by\n  Erik Svanberg"
        " and Rebecka Svanberg.\n"
        ""
        " \nCheck 'AUTHORS' and 'COPYING' files for details.";//comments

        GString *S = g_string_new( "This game is not GPL and not freeware.\n\n" );
        int i = 0; while(authors[i]) {g_string_append(S, authors[i]); g_string_append(S, "\n"); i++;}
        g_string_append(S, "\n\n");
        g_string_append(S,comment);
        UI_MessageBox( S->str, APPTITLE " " APPVERSION APPVERMINOR, MESSAGE_OK );
        g_string_free(S,TRUE);

}
#endif //USE_OLD_GTK

/* Free the global allocated data and finish the main loop */
static void cbAppDestroy(void)
{
  /* Close SDL (prevents the ticks to continue running) */
  //printf("\nQuitting...\n");
  SDL_Quit ();
#ifdef USE_OLD_GTK
  /* End with the GTK*/
  gtk_exit (0);
#endif //USE_OLD_GTK
    exit(0);
}


#ifdef USE_OLD_GTK
/********************** MENU DEFINITIONS **********************/

static GtkItemFactoryEntry menubar[] = {
  { "/_File",   NULL,   NULL,   NULL,   "<Branch>" },
  { "/File/tear",   NULL,   NULL,   NULL,   "<Tearoff>" },
  { "/File/Quit", "<control>Q", GTK_SIGNAL_FUNC(cbAppDestroy),    NULL,   "<Item>" },

  { "/_Misc",   NULL,   NULL,   NULL,   "<Branch>" },
  { "/Misc/tear",   NULL,   NULL,   NULL,   "<Tearoff>" },
  {"/Misc/Size x _1", NULL, GTK_SIGNAL_FUNC(__timer_callback),  ( IDC_Normal ), "<RadioItem>"},
  {"/Misc/Size x _2", NULL, GTK_SIGNAL_FUNC(__timer_callback),  ( IDC_Double ), "/Misc/Size x 1"},
  {"/Misc/Size x _3", NULL, GTK_SIGNAL_FUNC(__timer_callback),  ( IDC_Triple ),   "/Misc/Size x 1"},
  {"/Misc/Size x _4", NULL, GTK_SIGNAL_FUNC(__timer_callback),  ( IDC_Quadruple ),  "/Misc/Size x 1"},
  { "/Misc/sep",  NULL, NULL,   NULL,   "<Separator>" },
  {"/Misc/_Record", NULL, GTK_SIGNAL_FUNC(__timer_callback),  ( IDC_Record ), "<Item>"},//10
  {"/Misc/_Playback", NULL, GTK_SIGNAL_FUNC(__timer_callback),  ( IDC_Playback ), "<Item>"},
  {"/Misc/_QuickPlay",NULL, GTK_SIGNAL_FUNC(__timer_callback),  ( IDC_QuickPlay ),  "<Item>"},
  {"/Misc/_DM rules",NULL,  GTK_SIGNAL_FUNC(__timer_callback),  ( IDM_DMRULES ),  "<CheckItem>"},
  {"/Misc/_Items Remaining",NULL, GTK_SIGNAL_FUNC(__timer_callback),  ( IDC_ItemsRemaining ),"<Item>"},
  {"/Misc/_Non-CSB Items",NULL, GTK_SIGNAL_FUNC(__timer_callback),  ( IDC_NonCSBItemsRemaining ),"<Item>"},

  { "/_Speed",        NULL,   NULL,   NULL,          "<Branch>" },
  { "/Speed/tear",      NULL,   NULL,   NULL,         "<Tearoff>" },
  {"/Speed/_Glacial",     NULL, GTK_SIGNAL_FUNC(__timer_callback),  ( IDM_Glacial ),  "<RadioItem>"},
  {"/Speed/_Molasses",    NULL, GTK_SIGNAL_FUNC(__timer_callback), ( IDM_Molasses ),  "/Speed/Glacial"},
  {"/Speed/_Very Slow",   NULL, GTK_SIGNAL_FUNC(__timer_callback), ( IDM_VerySlow ),  "/Speed/Glacial"},
  {"/Speed/_Slow",      NULL, GTK_SIGNAL_FUNC(__timer_callback), ( IDM_Slow ),    "/Speed/Glacial"},
  {"/Speed/_Normal",      NULL, GTK_SIGNAL_FUNC(__timer_callback), ( IDM_Normal ),  "/Speed/Glacial"},
  {"/Speed/_Fast",      NULL, GTK_SIGNAL_FUNC(__timer_callback), ( IDM_Fast ),    "/Speed/Glacial"},
  {"/Speed/_Quick as a Bunny",  NULL, GTK_SIGNAL_FUNC(__timer_callback), ( IDM_Quick ),   "/Speed/Glacial"},
{ "/Speed/sep", NULL, NULL,   NULL,   "<Separator>" },
  {"/Speed/_Extra Ticks",   NULL, GTK_SIGNAL_FUNC(__timer_callback), ( IDM_ExtraTicks ),  "<CheckItem>"},
  {"/Speed/_Player Clock",    NULL, GTK_SIGNAL_FUNC(__timer_callback), ( IDM_PlayerClock ), "<CheckItem>"},

  { "/_Trace",        NULL,   NULL,   NULL,          "<Branch>" },
  { "/Trace/tear",      NULL,   NULL,   NULL,         "<Tearoff>" },
  {"/Trace/_TimerTrace",NULL, GTK_SIGNAL_FUNC(__timer_callback),  ( IDC_TimerTrace ), "<CheckItem>"},
  {"/Trace/_AttackTrace",NULL,  GTK_SIGNAL_FUNC(__timer_callback),  ( IDC_AttackTrace ),  "<CheckItem>"},
  {"/Trace/AI Trac_e",NULL, GTK_SIGNAL_FUNC(__timer_callback),  ( IDC_AITrace ),  "<CheckItem>"},
  {"/Trace/Viewport Trace",NULL,  GTK_SIGNAL_FUNC(__timer_callback),  ( IDM_GraphicTrace ), "<CheckItem>"},
  {"/Trace/_DSA Trace",NULL,  GTK_SIGNAL_FUNC(__timer_callback),  ( IDC_DSATrace ), "<CheckItem>"},
  {"/Trace/Dispatch Trace",NULL,  GTK_SIGNAL_FUNC(__timer_callback),  ( IDC_DispatchTrace ),  "<CheckItem>"},


  { "/_Help",   NULL,   NULL,   NULL,  "<LastBranch>" },
  { "/Help/tear",   NULL,   NULL,   NULL,  "<Tearoff>" },
  { "/Help/_About", NULL, GTK_SIGNAL_FUNC(appAbout),    NULL,  "<Item>" },
};
static void __before_misc_menu_is_showed(GtkMenuItem* bush, gpointer saddam) {
  GtkItemFactory *votes = gtk_item_factory_from_widget(GTK_WIDGET(bush));
  gtk_widget_set_sensitive(gtk_item_factory_get_widget( votes, "/Misc/Record"),
    BeginRecordOK ); //Record
  gtk_widget_set_sensitive(gtk_item_factory_get_widget( votes, "/Misc/Playback"),
    BeginRecordOK ); //Playback
  gtk_widget_set_sensitive(gtk_item_factory_get_widget( votes, "/Misc/Items Remaining"),
    ItemsRemainingOK ); //Items remaining
  gtk_widget_set_sensitive(gtk_item_factory_get_widget( votes, "/Misc/Non-CSB Items"),
    ItemsRemainingOK ); //Non-CSB Items
  gtk_widget_set_sensitive(gtk_item_factory_get_widget( votes, "/Misc/QuickPlay"),
          PlayfileIsOpen() ); //Quickplay
}
static void __before_trace_menu_is_showed(GtkMenuItem* bush, gpointer saddam) {
  GtkItemFactory *votes = gtk_item_factory_from_widget(GTK_WIDGET(bush));
  gtk_widget_set_sensitive(gtk_item_factory_get_widget( votes, "/Trace/TimerTrace"),
    (encipheredDataFile == NULL)); //TimerTrace
  gtk_widget_set_sensitive(gtk_item_factory_get_widget( votes, "/Trace/Dispatch Trace"),
    true ); //DispatchTrace
  gtk_widget_set_sensitive(gtk_item_factory_get_widget( votes, "/Trace/AI Trace"),
    true ); //AI Trace
  gtk_widget_set_sensitive(gtk_item_factory_get_widget( votes, "/Trace/AttackTrace"),
    true ); //AttackTrace
  gtk_widget_set_sensitive(gtk_item_factory_get_widget( votes, "/Trace/DSA Trace"),
    TimerTraceActive);//DSA Trace
//  gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(gtk_item_factory_get_widget( votes, "/Misc/AI Trace")), (AITraceActive)); //AI Trace
//  gtk_check_menu_item_set_active  (GTK_CHECK_MENU_ITEM(gtk_item_factory_get_widget( votes, "/Misc/DM rules")), DM_rules);//DM rules
}
#endif //USE_OLD_GTK

#if 0
// In order to draw SDL in a GTK window.
// Doesn't work, since gtk swallows every event.
#include <gdk/gdkx.h>
#endif

void g_log(const gchar *, GLogLevelFlags, const gchar *, ...)
{
    die(0x3512);
}

void DiscardAdditionalTimerEvents(void)
{
  SDL_Event evert;
//  {
//    FILE *f;
//    f = fopen("debug", "a");
//    fprintf(f, "Enter DiscardAdditionalTimerEvents");
//    fclose(f);
//  };
  while (1)
  {
#if defined SDL12
    if(SDL_PeepEvents(&evert,1,SDL_PEEKEVENT,~0) > 0)
#elif defined SDL20
    if(SDL_PeepEvents(&evert,1,SDL_PEEKEVENT,
                 SDL_FIRSTEVENT, SDL_LASTEVENT) > 0)
#else
    xxxError
#endif
    {
      if(evert.type == SDL_USEREVENT)
      {
        if(evert.user.code == IDC_Timer)
        {
          SDL_WaitEvent(&evert);
          continue;
        };
      };
    };
    break;
  };
//  {
//    FILE *f;
//    f = fopen("debug", "a");
//    fprintf(f, " and return\n");
//    fclose(f);
//  };
}

void Process_SDL_MOUSEMOTION(
           bool& cursorIsShowing)
{
  MTRACE("SDL_MOUSEMOTION\n");
  SDL_MouseMotionEvent *e = (SDL_MouseMotionEvent*) &evert;
  int x, y;
  bool warp = false;
//  {
//    FILE *f;
//    f = fopen("debug","a");
//    fprintf(f, "%d %d\n", e->x, e->y);
//    fclose(f);
//  };
  x = e->x;
  y = e->y;
  if (x >= 316*screenSize) {x = 316*screenSize-1; warp=true;};
  if (y >= 196*screenSize) {y = 196*screenSize-1; warp=true;}
  if (x <    1*screenSize) {x =   1*screenSize;   warp=true;}
  if (y <    1*screenSize) {y =   1*screenSize;   warp=true;}
  if (warp)
  {
#ifdef SDL12
    SDL_WarpMouse(x, y);
#elif defined SDL20
//    SDL_WarpMouseGlobal(x, y);
#else
    xxError
#endif
    return;
  };
  absMouseX = x; 
  absMouseY = y;
  i32 st_mouseX = X_TO_CSB(absMouseX,screenSize);
  i32 st_mouseY = Y_TO_CSB(absMouseY,screenSize);
  if (GameMode == 1)
  {
    if (cursorIsShowing)
    {
      cursorIsShowing = false;
      SDL_ShowCursor(SDL_DISABLE);
    };
  }
  else
  {
    if (!cursorIsShowing)
    {
      cursorIsShowing = true;
      SDL_ShowCursor(SDL_ENABLE);
    };
  };
//  {
//    FILE *f;
//    static int prevX, prevY;
//    if ((e->x != prevX) || (e->y != prevY))
//    {
//      f = fopen("/run/shm/debug","a");
//      fprintf(f, "Movement e->x=%d(%d), e->y=%d(%d); set mouseX=%d mouseY=%d showing=%d\n",
//                  e->x, e->xrel, e->y, e->yrel, absMouseX, absMouseY, cursorIsShowing);
//      fclose(f);
//    };
//  };
}

void Process_ecode_IDC_Normal(void)
{
  MTRACE("IDC_Normal\n");
  __resize_screen( 320, 240 );
  csbMessage.type = UIM_SETOPTION;
  csbMessage.p1 = OPT_NORMAL;
  csbMessage.p2 = 2; //2-(screenSize==1); //new value
  if (CSBUI(&csbMessage) != UI_STATUS_NORMAL)
  {
    PostQuitMessage(1);
  };
}

void Process_ecode_IDC_Timer(void)
{
  MTRACE("IDC_Timer\n");
  if(GameMode != 1)
  {
    LIN_Invalidate();
  };
  DiscardAdditionalTimerEvents();
  timerInQueue = false;
       //One-at-a-time, please
       //The problem was that under certain circumstances too
       //many timer events get queued and the SDL queue gets
       //full and keystrokes get discarded!
  csbMessage.type=UIM_TIMER;
  if (CSBUI(&csbMessage) != UI_STATUS_NORMAL)
  {
    PostQuitMessage(2);
  };
}

void Process_ecode_IDC_VIDEOEXPOSE(void)
{
  /*
   * BEGIN the evil VIDEOEXPOSE
   */
  MTRACE("IDC_VIDEOEXPOSE\n");
  //printf("IDC_VIDEOEXPOSE\n");
  line++;
  line = line%10;
  //        memset( SCR, line, 40 );
  //        SDL_BlitSurface(SCR, NULL, WND, NULL );
  switch(line)
  {
    case 0:
      //        g_print("vblint = %u",VBLInterruptCount);
      break;
    case 1:
      //        g_print("chkvbl = %u",CheckVBLCount);
      break;
    case 2:
    //        g_print("STblt = %u",STBLTCount);
      break;
    case 3:
    //  g_print("Time = %u",GameTime);
      break;
    case 4:
    //  g_print(
    //         "Skill[%d]=0x%08x      ",
    //          MostRecentlyAdjustedSkills[0],
    //          LatestSkillValues[0]);
      break;
    case 5:
    //  g_print(
    //          "Skill[%d]=0x%08x      ",
    //          MostRecentlyAdjustedSkills[1],
    //          LatestSkillValues[1]);
      break;
    case 6:
      switch (latestCharType)
      {
        case TYPEIGNORED:
    //      g_print( "%04x key --> Ignored                         ", latestCharp1);
          break;
        case TYPEKEY:
    //      g_print( "%04x key --> Translated %08x", latestCharp1, latestCharXlate);
          break;
      };
      break;
    case 7:
      switch (latestScanType)
      {
        case TYPESCAN:
        case TYPEIGNORED:
    //      g_print( "%08x mscan --> Ignored                        ",latestScanp1);
          break;
        case TYPEMSCANL:
    //      g_print( "%08x mscan --> Translated to %08x L",latestScanp1,latestScanXlate);
          break;
        case TYPEMSCANR:
    //      g_print( "%08x mscan --> Translated to %08x R",latestScanp1,latestScanXlate);
          break;
        default:
    //      g_print("                                              ");
          break;
      };
      break;
    case 8:
      switch (latestScanType)
      {
        case TYPEMSCANL:
        case TYPEIGNORED:
        case TYPEMSCANR:
    //      g_print( "%08x scan --> Ignored                        ",latestScanp2);
          break;
        case TYPESCAN:
    //      g_print( "%08x scan --> Translated to %08x", latestScanp2,latestScanXlate);
          break;
        default:
    //      g_print("                                              ");
          break;
      };
      break;
    case 9:
    //  g_print("Total Moves = %d",totalMoveCount);
      break;
  };
  //TextOut(hdc,325,25+15*line,msg,strlen(msg));
  //        g_warning(msg);
  /*
   * This message should be sent when the screen has been erased?
   *
   */
  /*
    csbMessage.type=UIM_REDRAW_ENTIRE_SCREEN;
    if (CSBUI(&csbMessage) != UI_STATUS_NORMAL)
    {
      PostQuitMessage(0);
      break;
    };
  */

  /*
   * Finally, write to the screen!
   */
  csbMessage.type=UIM_PAINT;
  if (CSBUI(&csbMessage) != UI_STATUS_NORMAL)
  {
    PostQuitMessage(3);
  };
}

void Process_ecode_IDC_Double(void)
{
  MTRACE("IDC_Double\n");
  __resize_screen( 320*2, 200*2);
  csbMessage.type = UIM_SETOPTION;
  csbMessage.p1 = OPT_DOUBLE;
  csbMessage.p2 = 2; //2-(screenSize==2); //new value
  if (CSBUI(&csbMessage) != UI_STATUS_NORMAL)
  {
    PostQuitMessage(4);
  };
}

void Process_ecode_IDC_Triple(void)
{
  MTRACE("IDC_Triple\n");
  __resize_screen( 320*3, 200*3 );
  csbMessage.type = UIM_SETOPTION;
  csbMessage.p1 = OPT_TRIPLE;
  csbMessage.p2 = 2; //2-(screenSize==3); //new value
  if (CSBUI(&csbMessage) != UI_STATUS_NORMAL)
  {
    PostQuitMessage(5);
  };
}

void Process_ecode_IDC_Quadruple(void)
{
  MTRACE("IDC_Quadruple\n");
  __resize_screen( 320*4, 200*4 );
  csbMessage.type = UIM_SETOPTION;
  csbMessage.p1 = OPT_QUADRUPLE;
  csbMessage.p2 = 2; //2-(screenSize==4); //new value
  if (CSBUI(&csbMessage) != UI_STATUS_NORMAL)
  {
    PostQuitMessage(6);
  };
}

void Process_ecode_IDC_Quintuple(void)
{
  MTRACE("IDC_Quintuple\n");
  __resize_screen( 320*5, 200*5 );
  csbMessage.type = UIM_SETOPTION;
  csbMessage.p1 = OPT_QUINTUPLE;
  csbMessage.p2 = 2; //2-(screenSize==4); //new value
  if (CSBUI(&csbMessage) != UI_STATUS_NORMAL)
  {
    PostQuitMessage(7);
  };
}

void Process_ecode_IDC_Sextuple(void)
{
  MTRACE("IDC_Sextuple\n");
  __resize_screen( 320*6, 200*6 );
  csbMessage.type = UIM_SETOPTION;
  csbMessage.p1 = OPT_SEXTUPLE;
  csbMessage.p2 = 2; //2-(screenSize==4); //new value
  if (CSBUI(&csbMessage) != UI_STATUS_NORMAL)
  {
    PostQuitMessage(8);
  };
}

void Process_ecode_IDC_QuickPlay(void)
{
  MTRACE("IDC_QuickPlay\n");
  if (!PlayfileIsOpen()) return;
  csbMessage.type = UIM_SETOPTION;
  csbMessage.p1 = OPT_QUICKPLAY;
  csbMessage.p2 = (NoSpeedLimit!=0) ? 0 : 1;
  if (CSBUI(&csbMessage) != UI_STATUS_NORMAL)
  {
    PostQuitMessage(9);
  };
}

void Process_ecode_IDM_Glacial(void)
{
  MTRACE("IDM_Glacial\n");
  csbMessage.type = UIM_SETOPTION;
  csbMessage.p1 = OPT_CLOCK;
  csbMessage.p2 = SPEED_GLACIAL;
  if (CSBUI(&csbMessage) != UI_STATUS_NORMAL)
  {
    PostQuitMessage(0xa);
  };
}

void Process_ecode_IDM_Molasses(void)
{
  MTRACE("IDM_Molasses\n");
  csbMessage.type = UIM_SETOPTION;
  csbMessage.p1 = OPT_CLOCK;
  csbMessage.p2 = SPEED_MOLASSES;
  if (CSBUI(&csbMessage) != UI_STATUS_NORMAL)
  {
    PostQuitMessage(0xb);
  };
}

void Process_ecode_IDM_VerySlow(void)
{
  MTRACE("IDM_VerySlow\n");
  csbMessage.type = UIM_SETOPTION;
  csbMessage.p1 = OPT_CLOCK;
  csbMessage.p2 = SPEED_VERYSLOW;
  if (CSBUI(&csbMessage) != UI_STATUS_NORMAL)
  {
    PostQuitMessage(0xc);
  };
}

void Process_ecode_IDM_Slow(void)
{
  MTRACE("IDM_Slow\n");
  csbMessage.type = UIM_SETOPTION;
  csbMessage.p1 = OPT_CLOCK;
  csbMessage.p2 = SPEED_SLOW;
  if (CSBUI(&csbMessage) != UI_STATUS_NORMAL)
  {
    PostQuitMessage(0xd);
  };
}

void Process_ecode_IDM_Normal(void)
{
  MTRACE("IDM_Normal\n");
  csbMessage.type = UIM_SETOPTION;
  csbMessage.p1 = OPT_CLOCK;
  csbMessage.p2 = SPEED_NORMAL;
  if (CSBUI(&csbMessage) != UI_STATUS_NORMAL)
  {
    PostQuitMessage(0xe);
  };
}

void Process_ecode_IDM_Fast(void)
{
  MTRACE("IDM_Fast\n");
  csbMessage.type = UIM_SETOPTION;
  csbMessage.p1 = OPT_CLOCK;
  csbMessage.p2 = SPEED_FAST;
  if (CSBUI(&csbMessage) != UI_STATUS_NORMAL)
  {
    PostQuitMessage(0xf);
  };
}

void Process_ecode_IDM_Quick(void)
{
  MTRACE("IDM_Quick\n");
  csbMessage.type = UIM_SETOPTION;
  csbMessage.p1 = OPT_CLOCK;
  csbMessage.p2 = SPEED_QUICK;
  if (CSBUI(&csbMessage) != UI_STATUS_NORMAL)
  {
    PostQuitMessage(0x10);
  };
}

void Process_ecode_IDM_PlayerClock(void)
{
  MTRACE("IDM_PlayerClock\n");
  csbMessage.type = UIM_SETOPTION;
  csbMessage.p1 = OPT_PLAYERCLOCK;
  if (CSBUI(&csbMessage) != UI_STATUS_NORMAL)
  {
    PostQuitMessage(0x11);
  };
}

void Process_ecode_IDM_ExtraTicks(void)
{
  MTRACE("IDM_ExtraTicks\n");
  csbMessage.type = UIM_SETOPTION;
  csbMessage.p1 = OPT_EXTRATICKS;
  if (CSBUI(&csbMessage) != UI_STATUS_NORMAL)
  {
    PostQuitMessage(0x12);
  };
}

void Process_ecode_IDC_Record(void)
{
  MTRACE("IDC_Record\n");
  csbMessage.type = UIM_SETOPTION;
  csbMessage.p1 = OPT_RECORD;
  csbMessage.p2 = 1;
  if (RecordMenuOption) csbMessage.p2 = 0;
  if (CSBUI(&csbMessage) != UI_STATUS_NORMAL)
  {
    PostQuitMessage(0x13);
  };
}

void Process_ecode_IDC_TimerTrace(void)
{
  MTRACE("IDC_TimerTrace\n");
  csbMessage.type = UIM_SETOPTION;
  csbMessage.p1 = OPT_TIMERTRACE;
  csbMessage.p2 = 1;
  if (TimerTraceActive) csbMessage.p2 = 0;
  if (CSBUI(&csbMessage) != UI_STATUS_NORMAL)
  {
    PostQuitMessage(0x14);
  };
}

void Process_ecode_IDM_DMRules(void)
{
   MTRACE("IDM_DMRULES\n");
   csbMessage.type = UIM_SETOPTION;
   csbMessage.p1 = OPT_DMRULES;
   if (CSBUI(&csbMessage) != UI_STATUS_NORMAL)
   {
     PostQuitMessage(0x15);
   };
}

void Process_ecode_IDC_AttackTrace(void)
{
  MTRACE("IDC_AttackTrace\n");
  csbMessage.type = UIM_SETOPTION;
  csbMessage.p1 = OPT_ATTACKTRACE;
  csbMessage.p2 = 1;
  if (AttackTraceActive) csbMessage.p2 = 0;
  if (CSBUI(&csbMessage) != UI_STATUS_NORMAL)
  {
    PostQuitMessage(0x16);
  };
}

void Process_ecode_IDC_AITrace(void)
{
  MTRACE("IDC_AITrace\n");
  csbMessage.type = UIM_SETOPTION;
  csbMessage.p1 = OPT_AITRACE;
  csbMessage.p2 = 1;
  if (AITraceActive) csbMessage.p2 = 0;
  if (CSBUI(&csbMessage) != UI_STATUS_NORMAL)
  {
    PostQuitMessage(0x17);
  };
}

void Process_ecode_IDC_ItemsRemaining(void)
{
  MTRACE("IDC_ItemsRemaining\n");
  csbMessage.type = UIM_SETOPTION;
  csbMessage.p1 = OPT_ITEMSREMAINING;
  if (CSBUI(&csbMessage) != UI_STATUS_NORMAL)
  {
    PostQuitMessage(0x18);
  };
}

void Process_ecode_IDC_NonCSBItemsRemaining(void)
{
  MTRACE("IDC_NonCSBItemsRemaining\n");
  csbMessage.type = UIM_SETOPTION;
  csbMessage.p1 = OPT_NONCSBITEMSREMAINING;
  if (CSBUI(&csbMessage) != UI_STATUS_NORMAL)
  {
    PostQuitMessage(0x19);
  };
}

void Process_ecode_IDC_Playback(void)
{
  MTRACE("IDC_Playback\n");
  csbMessage.type = UIM_SETOPTION;
  csbMessage.p1 = OPT_PLAYBACK;
  csbMessage.p2 = 1;
  if (PlayfileIsOpen()) csbMessage.p2 = 0;
  if (CSBUI(&csbMessage) != UI_STATUS_NORMAL)
  {
    PostQuitMessage(0x1a);
  };
}

void Process_ecode_IDC_DispatchTrace(void)
{
  MTRACE("IDC_DispatchTrace\n");
  if (trace  >= 0 )
  {
    CLOSE(trace);
    trace = -1;
  }
  else
  {
    trace = CREATE("trace.log","w", true);
  };
}

void Process_ecode_IDM_GraphicTrace(void)
{
  MTRACE("IDC_GraphicTrace\n");
  csbMessage.type = UIM_SETOPTION;
  csbMessage.p1 = OPT_GRAPHICTRACE;
  csbMessage.p2 = 1;
  if (TimerTraceActive) csbMessage.p2 = 0;
  if (CSBUI(&csbMessage) != UI_STATUS_NORMAL)
  {
    PostQuitMessage(0x1b);
  };
}

void Process_ecode_IDC_DSATrace(void)
{
  MTRACE("IDC_DSATrace\n");
  csbMessage.type = UIM_SETOPTION;
  csbMessage.p1 = OPT_DSATRACE;
  csbMessage.p2 = 1;
  if (DSATraceActive) csbMessage.p2 = 0;
  if (CSBUI(&csbMessage) != UI_STATUS_NORMAL)
  {
    PostQuitMessage(0x1c);
  };
}


void Process_SDL_USEREVENT(void)
{
  MTRACE("sdl_userevent->");
  SDL_UserEvent *e = (SDL_UserEvent*) &evert;
  switch( e->code )
  {// the menu (and timer)
    case IDC_Normal:        Process_ecode_IDC_Normal();        break;
    case IDC_Timer:         Process_ecode_IDC_Timer();         break;
    case IDC_VIDEOEXPOSE:   Process_ecode_IDC_VIDEOEXPOSE();   break;
    case IDC_Double:        Process_ecode_IDC_Double();        break;
    case IDC_Triple:        Process_ecode_IDC_Triple();        break;
    case IDC_Quadruple:     Process_ecode_IDC_Quadruple();     break;
    case IDC_Quintuple:     Process_ecode_IDC_Quintuple();     break;
    case IDC_Sextuple:      Process_ecode_IDC_Sextuple();      break;
    case IDC_QuickPlay:     Process_ecode_IDC_QuickPlay();     break;
    case IDM_Glacial:       Process_ecode_IDM_Glacial();       break;
    case IDM_Molasses:      Process_ecode_IDM_Molasses();      break;
    case IDM_VerySlow:      Process_ecode_IDM_VerySlow();      break;
    case IDM_Slow:          Process_ecode_IDM_Slow();          break;
    case IDM_Normal:        Process_ecode_IDM_Normal();        break;
    case IDM_Fast:          Process_ecode_IDM_Fast();          break;
    case IDM_Quick:         Process_ecode_IDM_Quick();         break;
    case IDM_PlayerClock:   Process_ecode_IDM_PlayerClock();   break;
    case IDM_ExtraTicks:    Process_ecode_IDM_ExtraTicks();    break;
    case IDC_Record:        Process_ecode_IDC_Record();        break;
    case IDC_TimerTrace:    Process_ecode_IDC_TimerTrace();    break;
    case IDM_DMRULES:       Process_ecode_IDM_DMRules();       break;
    case IDC_AttackTrace:   Process_ecode_IDC_AttackTrace();   break;
    case IDC_AITrace:       Process_ecode_IDC_AITrace();       break;
    case IDC_ItemsRemaining:Process_ecode_IDC_ItemsRemaining();break;
    case IDC_Playback:      Process_ecode_IDC_Playback();      break;
    case IDC_DispatchTrace: Process_ecode_IDC_DispatchTrace(); break;
    case IDM_GraphicTrace:  Process_ecode_IDM_GraphicTrace();  break;
    case IDC_DSATrace:      Process_ecode_IDC_DSATrace();      break;
    case IDC_NonCSBItemsRemaining:
                            Process_ecode_IDC_NonCSBItemsRemaining();
                                                               break;
    default:
      printf("Unknown SDL_USEREVENT\n");
      MTRACE("Unknown SDL_USEREVENT\n");
      //return DefWindowProc(hWnd, message, wParam, lParam);
      break;
  };
}

void Process_SDL_MOUSEBUTTONDOWN(void)
{
  MTRACE("SDL_MOUSEBUTTONDOWN->");
  {
    SDL_MouseButtonEvent  *e = (SDL_MouseButtonEvent*) &evert;
    switch(e->button)
    {// btn down, left or right
      case SDL_BUTTON_LEFT:
        MTRACE("SDL_BUTTON_LEFT\n");
        //printf("left button @%d ", (ui32)UI_GetSystemTime());
        csbMessage.type=UIM_LEFT_BUTTON_DOWN;
        csbMessage.p1 = absMouseX; // = e->x;  // horizontal position of cursor
        csbMessage.p2 = absMouseY; // = e->y;  // vertical position of cursor
        if (CSBUI(&csbMessage) != UI_STATUS_NORMAL)
        {
          PostQuitMessage(0x1d);
        };
        break;
      case SDL_BUTTON_RIGHT:
        MTRACE("SDL_BUTTON_RIGHT\n");
        //printf("right ");
        csbMessage.type=UIM_RIGHT_BUTTON_DOWN;
        csbMessage.p1 = absMouseX; // = e->x;  // horizontal position of cursor
        csbMessage.p2 = absMouseY; // = e->y;    // vertical position of cursor
        if (CSBUI(&csbMessage) != UI_STATUS_NORMAL)
        {
          PostQuitMessage(0x1e);
        };
        break;
    };
  };
  //printf("at (%x, %x)\n", X_TO_CSB(mouseX,screenSize),Y_TO_CSB(mouseY,screenSize));
}

void Process_SDL_MOUSEBUTTONUP(void)
{
  MTRACE("SDL_MOUSEBUTTONUP->");
  {// btn up, left or right
    SDL_MouseButtonEvent  *e = (SDL_MouseButtonEvent*) &evert;
    switch(e->button)
    {
      case SDL_BUTTON_RIGHT:
        MTRACE("SDL_BUTTON_RIGHT\n");
        csbMessage.type=UIM_RIGHT_BUTTON_UP;
        csbMessage.p1 = absMouseX; // = e->x;  // horizontal position of cursor
        csbMessage.p2 = absMouseY; // = e->y; // vertical position of cursor
        if (CSBUI(&csbMessage) != UI_STATUS_NORMAL)
        {
          PostQuitMessage(0x1f);
        };
        break;
      case SDL_BUTTON_LEFT:
        MTRACE("SDL_BUTTON_LEFT\n");
        csbMessage.type=UIM_LEFT_BUTTON_UP;
        csbMessage.p1 = absMouseX; // = e->x;   // horizontal position of cursor
        csbMessage.p2 = absMouseY; // = e->y;  // vertical position of cursor
        if (CSBUI(&csbMessage) != UI_STATUS_NORMAL)
        {
          PostQuitMessage(0x20);
        };
        break;
    };
  };
}

void Process_SDL_KEYDOWN(void)
{
//  static int prevp1 = -1;
  MTRACE("SDL_KEYDOWN\n");
#if defined SDL12
  {
    FILE *f;
    f = fopen("/run/shm/debug", "a");
    fprintf(f,"mod=%d,sym=0x%02x,scancode=0x%04x\n",
              evert.key.keysym.mod,
              evert.key.keysym.sym,
              evert.key.keysym.scancode);
    fclose(f);
  };
  if ((evert.key.keysym.mod & (KMOD_RALT | KMOD_LALT)) != 0)
  {
    if (evert.key.keysym.sym == SDLK_RETURN)
    {
      if (fullscreenActive)
      {
        if (WindowWidth > screenWidth) return;
        if (WindowHeight > screenHeight) return;
        WND = SDL_SetVideoMode(WindowWidth,  WindowHeight, 16, SDL_SWSURFACE);
        fullscreenActive = false;
        csbMessage.type=UIM_REDRAW_ENTIRE_SCREEN;
        if (CSBUI(&csbMessage) != UI_STATUS_NORMAL)
        {
          PostQuitMessage(0x20);
        };
        return;
      }
      else
      {
        WND = SDL_SetVideoMode(WindowWidth,  WindowHeight, 16, SDL_SWSURFACE|SDL_FULLSCREEN);
        fullscreenActive = true;
        csbMessage.type=UIM_REDRAW_ENTIRE_SCREEN;
        if (CSBUI(&csbMessage) != UI_STATUS_NORMAL)
        {
          PostQuitMessage(0x20);
        };
        return;
      };
    };
  };
#endif
  {
    /*SDL_KeyboardEvent *e = (SDL_KeyboardEvent*) &evert;
    if(e->keysym->sym==SDLK_LEFT)
    printf("left ");
    if else(e->keysym->sym==SDLK_RIGHT)
    printf("right ");
    */
    csbMessage.type=UIM_KEYDOWN;
    csbMessage.p1 =  evert.key.keysym.sym; //virtual key
    csbMessage.p2 = (evert.key.keysym.scancode>>16)&0xff; //scancode
    //printf("Key 0x%x pressed @%d\n",(int)csbMessage.p1, (ui32)UI_GetSystemTime());
//    if ((csbMessage.p1 == 3) && (prevp1 == 3))
//    {
//      PostQuitMessage(0);
//      return;
//    };
//    prevp1 = csbMessage.p1;
    if (CSBUI(&csbMessage) != UI_STATUS_NORMAL)
    {
      PostQuitMessage(0x21);
    };
  };
  {
    static int prevKeyUp = -1;
    csbMessage.type=UIM_CHAR;
    csbMessage.p1 = evert.key.keysym.sym;
    if (evert.key.keysym.sym == SDLK_LCTRL) return;
    if (evert.key.keysym.sym == SDLK_RCTRL) return;
    //{
    //  char line[80];
    //  sprintf(line, "p1=%d; mod=%d", csbMessage.p1, evert.key.keysym.mod);
    //  UI_MessageBox(line, "KEYUP", MESSAGE_OK);
    //};
    if (evert.key.keysym.mod &  (KMOD_LCTRL | KMOD_RCTRL))
    {
      if (csbMessage.p1 == SDLK_c)
      {
        if (prevKeyUp == 3)
        {
          PostQuitMessage(0x22);
          return;
        };
        csbMessage.p1 = 3; //Control-c
      };
    };
    prevKeyUp = csbMessage.p1;
    if (CSBUI(&csbMessage) != UI_STATUS_NORMAL)
    {
      PostQuitMessage(0x23);
    };
  };
}

void Process_SDL_KEYUP(void)
{
  MTRACE("SDL_KEYUP\n");
}

void Process_SDL_VIDEORESIZE(void)
{
//  {
//    FILE *f;
//    f = fopen("/run/shm/debug", "a");
//    fprintf(f, "Resizing Window\n");
//    fclose(f);
//    die(0x661c);
//  };
  MTRACE("SDL_VIDEORESIZE\n");
#if defined  SDL12
  SDL_ResizeEvent *e = (SDL_ResizeEvent*) &evert;
  WindowWidth = e->w;
  WindowHeight = e->h;
#elif defined SDL20
  NotImplemented(0x55a0);
#endif
  st_X = 320.0 / ((float)WindowWidth);
  st_Y = 200.0 / ((float)WindowHeight);
#if defined SDL12
  SDL_FreeSurface(WND);
  WND = SDL_SetVideoMode(WindowWidth,  WindowHeight, 16, SDL_SWSURFACE|SDL_FULLSCREEN);
#elif defined SDL20
  NotImplemented(0x3aab);
#endif 
#ifndef _DYN_WINSIZE
        switch(screenSize) {
          default:
          case 1: SDL_FreeSurface(SCRAP); SCRAP = SDL_CreateRGBSurface(SDL_SWSURFACE,320*1,200*1,8,0,0,0,0); break;
          case 2: SDL_FreeSurface(SCRAP); SCRAP = SDL_CreateRGBSurface(SDL_SWSURFACE,320*2,200*2,8,0,0,0,0); break;
          case 3: SDL_FreeSurface(SCRAP); SCRAP = SDL_CreateRGBSurface(SDL_SWSURFACE,320*3,200*3,8,0,0,0,0); break;
          case 4: SDL_FreeSurface(SCRAP); SCRAP = SDL_CreateRGBSurface(SDL_SWSURFACE,320*4,200*4,8,0,0,0,0); break;
        }
#endif //! _DYN_WINSIZE
  csbMessage.type=UIM_REDRAW_ENTIRE_SCREEN;
  if (CSBUI(&csbMessage) != UI_STATUS_NORMAL)
  {
    PostQuitMessage(0x24);
  };
  UI_Invalidate();
}

#ifdef SDL20

void Process_SDL_WINDOWEVENT(void)
{
  switch(evert.window.event)
  {
    case SDL_WINDOWEVENT_SHOWN:
      break;
    case SDL_WINDOWEVENT_HIDDEN:
      break;
    case SDL_WINDOWEVENT_LEAVE:
      break;
    case SDL_WINDOWEVENT_MOVED:
      break;
    case SDL_WINDOWEVENT_SIZE_CHANGED:
      break;
    case SDL_WINDOWEVENT_RESIZED:
      break;
    case SDL_WINDOWEVENT_EXPOSED:
      break;
    case SDL_WINDOWEVENT_ENTER:
      break;
    case SDL_WINDOWEVENT_FOCUS_GAINED:
      break;
    case SDL_WINDOWEVENT_FOCUS_LOST:
      break;
    case SDL_WINDOWEVENT_MINIMIZED:
      break;
    case SDL_WINDOWEVENT_MAXIMIZED:
      break;
    case SDL_WINDOWEVENT_RESTORED:
      break;
    case SDL_WINDOWEVENT_TAKE_FOCUS:
      break;
    default:
      {
        char line[80];
        sprintf(line,
                "evert.window.event = %d\n",
                evert.window.event);
        UI_MessageBox(line,
                      "Process_SDL_WINDOWEVENT",
                      MESSAGE_OK);
        NotImplemented(0x66); break;
        // check for RESIZE
    };
  };
}

#endif

/********************** MAIN **********************/
int main (int argc, char* argv[])
{
  static bool cursorIsShowing;
#ifdef MAEMO_NOKIA_770
  HildonProgram* program;
#endif//MAEMO_NOKIA_770
  /* Set default 'root' directory. */
  //folderSavedGame=(char*)".";
//  {
//    FILE *f;
//    f = fopen("/run/shm/debug", "w");
//    fprintf(f,"Debugging info\n");
//    fclose(f);
//  };
  folderParentName=(char*)".";
  folderName=(char*)".";
  root = (char*)".";
  /* Parse commandline arguments.
   * Eat all arguments I recognize. Otherwise Gnome kills me.
   */
  printf("%s\n", APPTITLE  " " APPVERSION APPVERMINOR );
        versionSignature = Signature(szCSBVersion);
  if( UI_ProcessOption(argv,argc) == false )
  {
      exit(0);
  }
  if( !(folderParentName&&folderName&&root) )
  {
    root=(char*)"./";
  }

  //    ***** Init the Aplication enviroment ******

#ifdef USE_OLD_GTK
  gtk_init (&argc, &argv);
  appGlobal = gtk_window_new (GTK_WINDOW_TOPLEVEL);

#ifdef MAEMO_NOKIA_770
  /* Create the hildon application and setup the title */
  program = HILDON_PROGRAM ( hildon_program_get_instance () );
  hildonmainwindow = HILDON_WINDOW(hildon_window_new());
  g_set_application_name ( "CSB" );
  //    g_signal_connect(G_OBJECT(hildonmainwindow), "delete_event", gtk_main_quit, NULL);
  hildon_program_add_window(program, hildonmainwindow);

  appGlobal = GTK_WIDGET(hildonmainwindow); //gtk_window_new (GTK_WINDOW_TOPLEVEL);
#endif//MAEMO_NOKIA_770

  if (appGlobal == NULL)
  {
    g_error("main: GTK application is a nullpointer.");
  }
  gtk_window_set_title (GTK_WINDOW (appGlobal), APPTITLE);
  //gtk_signal_connect (GTK_OBJECT (appGlobal), "delete_event",GTK_SIGNAL_FUNC (cbAppDestroy), NULL);
  gtk_signal_connect (GTK_OBJECT (appGlobal), "destroy",GTK_SIGNAL_FUNC (cbAppDestroy), NULL);
  gtk_signal_connect (GTK_OBJECT (appGlobal), "expose_event",GTK_SIGNAL_FUNC (__timer_callback),GINT_TO_POINTER( IDC_VIDEOEXPOSE));

  //gtk_create_menu
  {
    GtkItemFactory *item_factory;
    GtkAccelGroup *accel_group;
    GtkWidget *menuW;
    GtkWidget *main_vbox;
    static const gint nmenu_items = sizeof (menubar) / sizeof (menubar[0]);

    accel_group = gtk_accel_group_new ();
    item_factory = gtk_item_factory_new (GTK_TYPE_MENU_BAR, "<main>",
                                       accel_group);
    gtk_item_factory_create_items (item_factory, nmenu_items, menubar, NULL);
    gtk_window_add_accel_group (GTK_WINDOW (appGlobal), accel_group);
    gtk_signal_connect(GTK_OBJECT(gtk_item_factory_get_widget(item_factory,"/Misc")),
                       "expose_event",
    GTK_SIGNAL_FUNC(__before_misc_menu_is_showed),
                     (gpointer) item_factory);
    gtk_signal_connect(GTK_OBJECT(gtk_item_factory_get_widget(item_factory,"/Trace")),
                      "expose_event",
    GTK_SIGNAL_FUNC(__before_trace_menu_is_showed),
                    (gpointer) item_factory);
    menuW = gtk_item_factory_get_widget (item_factory, "<main>");
    main_vbox = gtk_vbox_new (FALSE, 1);
    gtk_container_set_border_width (GTK_CONTAINER (main_vbox), 1);
    gtk_container_add (GTK_CONTAINER (appGlobal), main_vbox);
    gtk_box_pack_start (GTK_BOX (main_vbox), menuW, FALSE, TRUE, 0);

    gtk_check_menu_item_set_active  (GTK_CHECK_MENU_ITEM(gtk_item_factory_get_widget(item_factory,"/Misc/Size x 2")), true);// Default is '2 x ScreenSize'
    gtk_check_menu_item_set_active  (GTK_CHECK_MENU_ITEM(gtk_item_factory_get_widget(item_factory,"/Speed/Normal")), true); // Default is 'Normal' speed
    if(!PlaybackCommandOption)
    {
      gtk_check_menu_item_set_active  (GTK_CHECK_MENU_ITEM(gtk_item_factory_get_widget(item_factory,"/Speed/Extra Ticks")), true);extraTicks=true;// Default is 'Extra Ticks'
    };
  };


  gtk_widget_show_all (appGlobal);
#ifdef MAEMO_NOKIA_770
  /* Hack to get SDL to use GTK window */
  /* Doesn't work with gtk-1.2 */
  {
    char SDL_windowhack[32];
    sprintf( SDL_windowhack,"SDL_WINDOWID=%ld",
            (GTK_WINDOW(hildonmainwindow)));
            (GTK_WINDOW(appGlobal)));

    //      GDK_WINDOW_XWINDOW(appGlobal->window));
    putenv(SDL_windowhack);
    printf("Done SDL window hack. ID should be %ld\n%s", GDK_WINDOW_XWINDOW(appGlobal->window), SDL_windowhack);
  };
#endif //MAEMO_NOKIA_770
#endif //USE_OLD_GTK

//    ***** Initialize defaults, Video and Audio *****
  if ( SDL_Init ( SDL_INIT_VIDEO)<0)//|SDL_INIT_AUDIO|SDL_INIT_TIMER) < 0)
  {
    g_critical("Unable to init SDL: %s", SDL_GetError() );
    //g_error("Unable to init SDL: %s", SDL_GetError() );
  };
  printf("SDL initialized.\n");
#if defined SDL12
  SDL_WM_SetCaption(APPTITLE, NULL);
#elif defined SDL20
  // NotImplemented(0x87ea);
#else
  xxxError
#endif


// ****************************** the DISPLAY ****************************************************
  {
#if defined SDL12
    const SDL_VideoInfo *info;
    info = SDL_GetVideoInfo();
    screenWidth = info->current_w;
    screenHeight = info->current_h;
    printf("Window Size = %d (%d x %d)\n", screenSize, WindowWidth, WindowHeight);
    printf("Screen Size = %d x %d\n", screenWidth, screenHeight);
    if ((WindowWidth > screenWidth) || (WindowHeight > screenHeight))
    {
      UI_MessageBox("Specified window size too large for screen",
                    "Failed to create window",
                    MESSAGE_OK);
      die(0x731b);
    };
    WND = SDL_SetVideoMode(WindowWidth,  WindowHeight, 16, SDL_SWSURFACE|SDL_FULLSCREEN);
    {
      FILE *f;
      f = fopen("/run/shm/debug","a");
      fprintf(f, "Setting window to %d x %d\n", screenWidth, screenHeight);
      fclose(f);
    };
    fullscreenActive = true;
#elif defined SDL20
    int flags;
    screenSize = 1;
    flags = 0;
    if (fullscreenRequested)
    {
      flags =  SDL_WINDOW_FULLSCREEN_DESKTOP;
    };
    if ((sdlWindow = SDL_CreateWindow(
                   APPTITLE,
                   SDL_WINDOWPOS_CENTERED,
                   SDL_WINDOWPOS_CENTERED,
                   WindowWidth,WindowHeight,
                   flags)) == NULL)
    {
      UI_MessageBox(SDL_GetError(),
                    "Failed to create window",
                    MESSAGE_OK);
      die(0xe9a3);
    };
    SDL_VIDEOEXPOSE = SDL_RegisterEvents(1);
    if ((sdlRenderer = SDL_CreateRenderer(
                   sdlWindow,
                   -1,
                   0)) == NULL)
    {
      UI_MessageBox(SDL_GetError(),
                    "Failed to create Renderer",
                    MESSAGE_OK);
      die(0x519b);
    };
    SDL_RenderSetLogicalSize(sdlRenderer,320,200);
    if ((sdlTexture = SDL_CreateTexture(
                   sdlRenderer,
                   SDL_PIXELFORMAT_RGB565,
                   SDL_TEXTUREACCESS_STREAMING,
                   320,200)) == NULL)
    {
      UI_MessageBox(SDL_GetError(),
                    "Failed to create texture",
                    MESSAGE_OK);
      die(0xff53);
    };
#else
    xxxError
#endif
#ifdef _DYN_WINSIZE
    if(!Hermes_Init()) g_error("No hermes...");
    from_palette = Hermes_PaletteInstance();
    to_palette = Hermes_PaletteInstance();
    converter = Hermes_ConverterInstance( HERMES_CONVERT_NORMAL );
    from_format = Hermes_FormatNew(8,0,0,0,0,TRUE);
#else
#ifdef SDL12
    SCRAP = SDL_CreateRGBSurface(SDL_SWSURFACE,screenWidth,screenHeight,8,0,0,0,0);
    {
      FILE *f;
      f = fopen("/run/shm/debug", "a");
      fprintf(f, "Creating Surface %d %d\n", screenWidth, screenHeight);
      fclose(f);
    };
    if (SCRAP == NULL)
    {
      g_error("Unable to get SDL surface: %s",SDL_GetError());
    };
#endif
#endif
  };
  if ( SDL_InitSubSystem (SDL_INIT_TIMER) < 0)
  {
    g_error("Unable to init Timer: %s", SDL_GetError() );
  }

#if defined SDL12
  SDL_UpdateRect(WND, 0, 0, 0, 0);
#elif defined SDL20
  // Nothing needed here???
  // NotImplemented(0x85ea);
#else
  xxxError
#endif
  SDL_ShowCursor(SDL_ENABLE);
  cursorIsShowing = true;
  if(fullscreenRequested)
  {
    fullscreenActive = true;
#if defined SDL12
    //SDL_WM_ToggleFullScreen(WND);
#elif defined SDL20
    //NotImplemented(0x84ea);
#else
    xxxError
#endif
  }
  UI_Initialize_sounds();


#ifdef USE_PARAGUI
  PG_Application app;
  app.SetScreen(WND);
  app.LoadTheme("default");
  PG_Button btnOK(NULL,0, PG_Rect((WIDTH-100)/2,(HEIGHT-20)/2,100,20),"Push me !");
  btnOK.Show();
  //app.Run();
#endif//USE_PARAGUI
  /*
   * Initialize the display in a 640x480 8-bit palettized mode,
   * requesting a software surface, or anything else....
   */

  /* Do the window-cha-cha. */
  speedTable[SPEED_GLACIAL].vblPerTick = 1000;
  speedTable[SPEED_MOLASSES].vblPerTick = 55;
  speedTable[SPEED_VERYSLOW].vblPerTick = 33;
  speedTable[SPEED_SLOW].vblPerTick = 22;
  speedTable[SPEED_NORMAL].vblPerTick = 15;
  speedTable[SPEED_FAST].vblPerTick = 11;
  speedTable[SPEED_QUICK].vblPerTick = 7;

  volumeTable[VOLUME_FULL].divisor = 1;
  volumeTable[VOLUME_HALF].divisor = 2;
  volumeTable[VOLUME_QUARTER].divisor = 4;
  volumeTable[VOLUME_EIGHTH].divisor = 8;
  volumeTable[VOLUME_OFF].divisor = 65536;

  MTRACE("*** First initialization ***\n");
  csbMessage.type=UIM_INITIALIZE;
  /* Parse config.linux */
  if (CSBUI(&csbMessage) != UI_STATUS_NORMAL)
  {
    PostQuitMessage(0x25);
  };

  switch(screenSize){
    case 1:
      __resize_screen( 320, 200 );
      csbMessage.type = UIM_SETOPTION;
      csbMessage.p1 = OPT_NORMAL;
      csbMessage.p2 = 2;
      break;
    case 3:
      __resize_screen( 320*3, 200 *3);
      csbMessage.type = UIM_SETOPTION;
      csbMessage.p1 = OPT_TRIPLE;
      csbMessage.p2 = 2;
      break;
    case 4:
      __resize_screen( 320*4, 200 *4);
      csbMessage.type = UIM_SETOPTION;
      csbMessage.p1 = OPT_QUADRUPLE;
      csbMessage.p2 = 2;
      break;
    case 5:
      __resize_screen( 320*5, 200 *5);
      csbMessage.type = UIM_SETOPTION;
      csbMessage.p1 = OPT_QUINTUPLE;
      csbMessage.p2 = 2;
      break;
    case 6:
      __resize_screen( 320*6, 200 *6);
      csbMessage.type = UIM_SETOPTION;
      csbMessage.p1 = OPT_SEXTUPLE;
      csbMessage.p2 = 2;
      break;
    case 2:
    //  case 0:
    default:
      __resize_screen(WindowWidth,WindowHeight);
      screenSize=2;
      csbMessage.type = UIM_SETOPTION;
      csbMessage.p1 = OPT_DOUBLE;
      csbMessage.p2 = 2;
      break;
  };// End of switch(screenSize)
  if (CSBUI(&csbMessage) != UI_STATUS_NORMAL)
  {
    PostQuitMessage(0x26);
  };

  /* Setup a 50ms timer. */
  timer = SDL_AddTimer(TImER?TImER:10, __Timer_Callback, GINT_TO_POINTER(IDC_Timer));
  printf("Timer: %d ms established...\n", TImER?TImER:10);

  //SDL_WM_GrabInput(SDL_GRAB_ON);

  /********************************************
   *
   *       Main Execution Loop
   *
   ********************************************
   */
  while (TRUE)
  {

#ifdef USE_OLD_GTK
    gtk_main_iteration_do(FALSE); // This solution will always cost CPU instead of gtk_main()
#endif //USE_OLD_GTK
    // evert = app.WaitEvent();
    if (SDL_PollEvent(&evert) == 0)
    {
      timerInQueue=false;
      if(SDL_WaitEvent(&evert) == 0) continue;
    }
    if (sdlQuitPending)
    {
//      FILE *f;
//      f = fopen("debug","a");
//      fprintf(f, "MainLook sdlQuitPending type= %d\n", evert.type);
//      fclose(f);
      if (SDL_QuitRequested())
      {
        if (evert.type != SDL_QUIT) continue;
      }
      else
      {
        sdlQuitPending = false;
      }; 
    };
    {
//      FILE *f;
//      f = fopen("debug","a");
//      if (evert.type == SDL_USEREVENT)
//      {
//        fprintf(f,"SDL_USEREVENT  type = %d\n", evert.user.code);
//      }
//      else
//      { 
//        fprintf(f, "MessageLoop  = %d\n", evert.type);
//      };
//      fclose(f);
    };
    /* Listen for 'quick buttons' here. */
    /* Hail to the Great Message Struct! */
    MTRACE("msg=");
    switch( evert.type )
    {
      case SDL_QUIT:
        MTRACE("SDL_QUIT\n");
        SDL_RemoveTimer(timer);
        SDL_Quit();
        cbAppDestroy();
        exit(0);
        break;
      default:
        MTRACE("was ignored\n");
        break;
      case SDL_MOUSEMOTION:     Process_SDL_MOUSEMOTION(
                                         cursorIsShowing);
                                break;
      case SDL_USEREVENT:       Process_SDL_USEREVENT();       break;// __timer_loopback
      case SDL_MOUSEBUTTONDOWN: Process_SDL_MOUSEBUTTONDOWN(); break;
      case SDL_MOUSEBUTTONUP:   Process_SDL_MOUSEBUTTONUP();   break;
      case SDL_KEYDOWN:         Process_SDL_KEYDOWN();         break;
      case SDL_KEYUP:           Process_SDL_KEYUP();           break;
#if defined SDL12
      case SDL_VIDEORESIZE:     Process_SDL_VIDEORESIZE();     break;
#elif defined SDL20
      case SDL_WINDOWEVENT:     Process_SDL_WINDOWEVENT();    break;
#else
      xxxError
#endif
      /*
      case SDL_ACTIVEEVENT:
        {
          if ( evert.active.state & SDL_APPACTIVE )
          {
            if ( evert.active.gain )
            {
              printfalll("App activated\n");
            }
            else
            {
              printf("App iconified\n");
            };
          };
        };
      */

    }; /* Eof Great Event Switch */
  } /* Eof Lord Message Loop */

  printf("Quiting SDL.\n");

  /* Shutdown all subsystems */
  SDL_Quit();
  printf("Quiting....\n");
  exit(0);
  return (0);
}
