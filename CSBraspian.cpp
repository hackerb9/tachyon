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

#define APPTITLE	"CSBwin"
#define APPVERSION 	"12.0"
#define APPVERMINOR	"v0" /* linux-only update*/

extern gboolean noDirtyRect;
extern SDL_Rect DirtyRect;

extern bool RecordMenuOption;
extern bool DMRulesDesignOption;
extern bool RecordDesignOption;
extern bool extendedPortraits;
extern bool fullscreenActive;
extern bool simpleEncipher; //not used in Linux
extern unsigned char *encipheredDataFile; //not used in Linux

static SDL_Event evert;
int systemXsize;
int systemYsize;
int systemBPP;

#ifdef PITRACE
class PI_TRACE
{
public:
  FILE *f;
  PI_TRACE(void){f = fopen("piTrace.txt","w");};
  ~PI_TRACE(void){if(f !=NULL)fclose(f);};
};
PI_TRACE pitrace;

#endif

#ifdef MAEMO_NOKIA_770
# include "hildon-widgets/hildon-program.h"
# include "hildon-widgets/hildon-window.h"

  HildonProgram *program = NULL;
  HildonWindow *hildonmainwindow = NULL;
#endif//MAEMO_NOKIA_770

void PostQuitMessage( int a ) {
    SDL_Event event;
    event.type = SDL_QUIT;
    SDL_PushEvent(&event);
    SDL_Quit ();
#ifdef USE_OLD_GTK
    gtk_exit (0);
#endif //USE_OLD_GTK
    exit(0);
}
static guint32 __timer_callback(guint32 interval, void *param) {
    SDL_Event ev;
    ev.type = SDL_USEREVENT;
    ((SDL_UserEvent*) &ev)->code = GPOINTER_TO_INT(param);
    SDL_PushEvent(&ev);
    return interval;
}
static void __resize_screen( ui32 w, i32 h ) {
	SDL_Event ev;
	ev.type = SDL_VIDEORESIZE;
	((SDL_ResizeEvent*)&ev) -> w = w;
	((SDL_ResizeEvent*)&ev) -> h = h;
	SDL_PushEvent(&ev);
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
		__timer_callback(0, GINT_TO_POINTER(SDL_VIDEOEXPOSE) );
	}
}




#define MAX_LOADSTRING 100



i32 trace=-1;
CSB_UI_MESSAGE csbMessage;
#ifdef _DYN_WINSIZE
i32 screenSize=2;
#else
i32 screenSize=2;
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
#define IDC_Timer 1
static float mouseX = 0.0;
static float mouseY = 0.0;

void MTRACE(const char *msg)
{
  if (trace < 0) return;
  FILE *f = GETFILE(trace);
  fprintf(f, "%s", msg);
  fflush(f);
}

#ifdef PITRACE
void PiTrace(const char *msg)
{
  static int repeat = 0;
  static char line[999] = "";
  if (strcmp(line, msg) == 0)
  {
    repeat++;
    return;
  };
  if (repeat > 0)
  {
    fprintf(pitrace.f, "Repeated %d times\n", repeat);
    repeat = 0;
  };
  strcpy(line, msg);
  fprintf(pitrace.f, "%s", msg);
  fflush(pitrace.f);
}
#endif

void UI_GetCursorPos(i32 *_x, i32 *_y)
{
  if (fullscreenActive)
  {
	i32 x=mouseX, y=mouseY;
    RECT rect;
    i32 distance2 = 1<<30;
    i32 d2, dx, dy;
    i32 i;
    POINT test, closestPoint = {0,0};
    for (i=0; (distance2 != 0) && GetVideoRectangle(i, &rect); i++)
    {
      if (x < rect.left) test.x = rect.left;
      else if (x >= rect.right) test.x = rect.right-1;
      else test.x = x;
      if (y < rect.top) test.y = rect.top;
      else if (y >= rect.bottom) test.y = rect.bottom-1;
      else test.y = y;
      dx = test.x - x;
      dy = test.y - y;
      d2 = dx*dx + dy*dy;
      if (d2 < distance2)
      {
        distance2 = d2;
        closestPoint.x = test.x;
        closestPoint.y = test.y;
      };
    };
    //if (set) SetCursorPos(point.x, point.y);
    TranslateFullscreen(closestPoint.x,closestPoint.y,*_x,*_y);
  } else {
	*_x = X_TO_CSB(mouseX,screenSize);
	*_y = Y_TO_CSB(mouseY,screenSize);
  }
}


char szCSBVersion[] = "Portable DM/CSB Version" APPVERSION;
int WindowWidth = 640;
int WindowHeight = 400;
int WindowX = 0;
int WindowY = 0;
float st_X = 320.0 / 640.0;
float st_Y = 200.0 / 480.0;
bool fullscreenRequested = true;
bool fullscreenActive = true;


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
GtkWidget *appGlobal;		/* Main Application Handle (menu et al) */
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
 const char *comment=	"Port from the ST to windows by\n  Paul R. Stevens\n"
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
	printf("\nQuitting...\n");
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
  { "/_File",		NULL,		NULL,		NULL,		"<Branch>" },
  { "/File/tear",		NULL,		NULL,		NULL,	 	"<Tearoff>" },
  { "/File/Quit",	"<control>Q",	GTK_SIGNAL_FUNC(cbAppDestroy),		NULL,		"<Item>" },
  
  { "/_Misc",		NULL,		NULL,		NULL,	 	"<Branch>" },
  { "/Misc/tear",		NULL,		NULL,		NULL,	 	"<Tearoff>" },
  {"/Misc/Size x _1",	NULL,	GTK_SIGNAL_FUNC(__timer_callback),	( IDC_Normal ),	"<RadioItem>"},
  {"/Misc/Size x _2",	NULL,	GTK_SIGNAL_FUNC(__timer_callback),	( IDC_Double ),	"/Misc/Size x 1"},
  {"/Misc/Size x _3",	NULL,	GTK_SIGNAL_FUNC(__timer_callback),	( IDC_Triple ),		"/Misc/Size x 1"},
  {"/Misc/Size x _4",	NULL,	GTK_SIGNAL_FUNC(__timer_callback),	( IDC_Quadruple ),	"/Misc/Size x 1"},
{ "/Misc/sep",	NULL,	NULL,		NULL,		"<Separator>" },
  {"/Misc/_Record",	NULL,	GTK_SIGNAL_FUNC(__timer_callback),	( IDC_Record ),	"<Item>"},//10
  {"/Misc/_Playback",	NULL,	GTK_SIGNAL_FUNC(__timer_callback),	( IDC_Playback ),	"<Item>"},
  {"/Misc/_QuickPlay",NULL,	GTK_SIGNAL_FUNC(__timer_callback),	( IDC_QuickPlay ),	"<Item>"},
  {"/Misc/_DM rules",NULL,	GTK_SIGNAL_FUNC(__timer_callback),	( IDM_DMRULES ),	"<CheckItem>"},
  {"/Misc/_Items Remaining",NULL,	GTK_SIGNAL_FUNC(__timer_callback),	( IDC_ItemsRemaining ),"<Item>"},
  {"/Misc/_Non-CSB Items",NULL,	GTK_SIGNAL_FUNC(__timer_callback),	( IDC_NonCSBItemsRemaining ),"<Item>"},
  
  { "/_Speed",				NULL,		NULL,		NULL,					 "<Branch>" },
  { "/Speed/tear",			NULL,		NULL,		NULL,	 				"<Tearoff>" },
  {"/Speed/_Glacial",			NULL,	GTK_SIGNAL_FUNC(__timer_callback),	( IDM_Glacial ),	"<RadioItem>"},
  {"/Speed/_Molasses",		NULL,	GTK_SIGNAL_FUNC(__timer_callback), ( IDM_Molasses ),	"/Speed/Glacial"},  
  {"/Speed/_Very Slow",		NULL,	GTK_SIGNAL_FUNC(__timer_callback), ( IDM_VerySlow ),	"/Speed/Glacial"},
  {"/Speed/_Slow",			NULL,	GTK_SIGNAL_FUNC(__timer_callback), ( IDM_Slow ),		"/Speed/Glacial"},
  {"/Speed/_Normal",			NULL,	GTK_SIGNAL_FUNC(__timer_callback), ( IDM_Normal ),	"/Speed/Glacial"},
  {"/Speed/_Fast",			NULL,	GTK_SIGNAL_FUNC(__timer_callback), ( IDM_Fast ),		"/Speed/Glacial"},
  {"/Speed/_Quick as a Bunny",	NULL,	GTK_SIGNAL_FUNC(__timer_callback), ( IDM_Quick ),		"/Speed/Glacial"},
{ "/Speed/sep", NULL,	NULL,		NULL,		"<Separator>" },
  {"/Speed/_Extra Ticks",		NULL,	GTK_SIGNAL_FUNC(__timer_callback), ( IDM_ExtraTicks ),	"<CheckItem>"},
  {"/Speed/_Player Clock",		NULL,	GTK_SIGNAL_FUNC(__timer_callback), ( IDM_PlayerClock ),	"<CheckItem>"},

  { "/_Trace",				NULL,		NULL,		NULL,					 "<Branch>" },
  { "/Trace/tear",			NULL,		NULL,		NULL,	 				"<Tearoff>" },
  {"/Trace/_TimerTrace",NULL,	GTK_SIGNAL_FUNC(__timer_callback),	( IDC_TimerTrace ),	"<CheckItem>"},
  {"/Trace/_AttackTrace",NULL,	GTK_SIGNAL_FUNC(__timer_callback),	( IDC_AttackTrace ),	"<CheckItem>"},
  {"/Trace/AI Trac_e",NULL,	GTK_SIGNAL_FUNC(__timer_callback),	( IDC_AITrace ),	"<CheckItem>"},
  {"/Trace/Viewport Trace",NULL,	GTK_SIGNAL_FUNC(__timer_callback),	( IDM_GraphicTrace ),	"<CheckItem>"},
  {"/Trace/_DSA Trace",NULL,	GTK_SIGNAL_FUNC(__timer_callback),	( IDC_DSATrace ),	"<CheckItem>"},
  {"/Trace/Dispatch Trace",NULL,	GTK_SIGNAL_FUNC(__timer_callback),	( IDC_DispatchTrace ),	"<CheckItem>"},


  { "/_Help",		NULL,		NULL,		NULL,	 "<LastBranch>" },
  { "/Help/tear",		NULL,		NULL,		NULL,	 "<Tearoff>" },
  { "/Help/_About",	NULL,	GTK_SIGNAL_FUNC(appAbout),		NULL,	 "<Item>" },
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
//	gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(gtk_item_factory_get_widget( votes, "/Misc/AI Trace")), (AITraceActive)); //AI Trace
//	gtk_check_menu_item_set_active  (GTK_CHECK_MENU_ITEM(gtk_item_factory_get_widget( votes, "/Misc/DM rules")), DM_rules);//DM rules
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



/********************** MAIN **********************/
int main (int argc, char* argv[])
{
#ifdef MAEMO_NOKIA_770
	HildonProgram* program;
#endif//MAEMO_NOKIA_770

	/* Set default 'root' directory. */
    	folderSavedGame=(char*)".";
	folderParentName=(char*)".";
	folderName=(char*)".";
	root = (char*)".";
	        /* Parse commandline arguments.
		* Eat all arguments I recognize. Otherwise Gnome kills me.
		*/
	printf("%s\n", APPTITLE	 " " APPVERSION APPVERMINOR );
	if( UI_ProcessOption(argv,argc) == false ){
	    exit(0);
	}
	if( !(folderParentName&&folderName&&root) ){
	  root=(char*)"./";
	}

//		***** Init the Aplication enviroment ******

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

	if (appGlobal == NULL){
	    g_error("main: GTK application is a nullpointer.");}
    gtk_window_set_title (GTK_WINDOW (appGlobal), APPTITLE);

	//gtk_signal_connect (GTK_OBJECT (appGlobal), "delete_event",GTK_SIGNAL_FUNC (cbAppDestroy), NULL);
	gtk_signal_connect (GTK_OBJECT (appGlobal), "destroy",GTK_SIGNAL_FUNC (cbAppDestroy), NULL);
	gtk_signal_connect (GTK_OBJECT (appGlobal), "expose_event",GTK_SIGNAL_FUNC (__timer_callback),GINT_TO_POINTER( SDL_VIDEOEXPOSE));

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
		if(!PlaybackCommandOption){
			gtk_check_menu_item_set_active  (GTK_CHECK_MENU_ITEM(gtk_item_factory_get_widget(item_factory,"/Speed/Extra Ticks")), true);extraTicks=true;// Default is 'Extra Ticks'
		}
	}


	gtk_widget_show_all (appGlobal);
#ifdef MAEMO_NOKIA_770
	/* Hack to get SDL to use GTK window */
	/* Doesn't work with gtk-1.2 */
	{ 
	  char SDL_windowhack[32];
  	  sprintf(SDL_windowhack,"SDL_WINDOWID=%ld",

			   (GTK_WINDOW(hildonmainwindow)));
			   (GTK_WINDOW(appGlobal)));

//	    GDK_WINDOW_XWINDOW(appGlobal->window));
	  putenv(SDL_windowhack);
	  printf("Done SDL window hack. ID should be %ld\n%s", GDK_WINDOW_XWINDOW(appGlobal->window), SDL_windowhack);
	}
#endif //MAEMO_NOKIA_770
#endif //USE_OLD_GTK
	
//		***** Initialize defaults, Video and Audio *****
 	if ( SDL_Init ( SDL_INIT_VIDEO)<0)//|SDL_INIT_AUDIO|SDL_INIT_TIMER) < 0)
	{
		g_critical("Unable to init SDL: %s", SDL_GetError() );
		//g_error("Unable to init SDL: %s", SDL_GetError() );
	}
	printf("SDL initialized.\n");
	SDL_WM_SetCaption(APPTITLE, NULL);

	
// ****************************** the DISPLAY ****************************************************
#ifdef _DYN_WINSIZE
	if(!Hermes_Init()) g_error("No hermes...");
	from_palette = Hermes_PaletteInstance();
	to_palette = Hermes_PaletteInstance();
	converter = Hermes_ConverterInstance( HERMES_CONVERT_NORMAL );
	from_format = Hermes_FormatNew(8,0,0,0,0,TRUE);
#else
	SCRAP = SDL_CreateRGBSurface(SDL_SWSURFACE,320,200,8,0,0,0,0);
	if (SCRAP == NULL)
	{
		g_error("Unable to get SDL surface: %s",SDL_GetError());
	}
#endif
        {
          const SDL_VideoInfo *videoInfo=SDL_GetVideoInfo();
          systemXsize = videoInfo->current_w;
          systemYsize = videoInfo->current_h;
          systemBPP   = videoInfo->vfmt->BitsPerPixel;
	  WND = SDL_SetVideoMode(320, //systemXsize, 
                                 200, //systemYsize, 
                                 systemBPP,
                                 SDL_SWSURFACE|SDL_RESIZABLE);
        };
	if ( SDL_InitSubSystem (SDL_INIT_TIMER) < 0)
	{
		g_error("Unable to init Timer: %s", SDL_GetError() );
	}
	SDL_UpdateRect(WND, 0, 0, 0, 0);
	SDL_ShowCursor(SDL_ENABLE);
	if(fullscreenRequested)
	{
		fullscreenActive = true;
		SDL_WM_ToggleFullScreen(WND);
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
	PostQuitMessage(0);
    };

    switch(screenSize){
	case 1:
#ifdef PI_RESIZEABLE
	    			    __resize_screen( 320, 240 );
#endif
			    csbMessage.type = UIM_SETOPTION;
			    csbMessage.p1 = OPT_NORMAL;
			    csbMessage.p2 = 2;
			    break;
	case 3:
	    			    __resize_screen( 320*3, 240 *3);
			    csbMessage.type = UIM_SETOPTION;
			    csbMessage.p1 = OPT_TRIPLE;
			    csbMessage.p2 = 2;
			    break;
	case 4:
	    	    			    __resize_screen( 320*4, 240 *4);
			    csbMessage.type = UIM_SETOPTION;
			    csbMessage.p1 = OPT_QUADRUPLE;
			    csbMessage.p2 = 2;
			    break;
	case 5:
	    	    			    __resize_screen( 320*5, 240 *5);
			    csbMessage.type = UIM_SETOPTION;
			    csbMessage.p1 = OPT_QUADRUPLE;
			    csbMessage.p2 = 2;
			    break;
	case 6:
	    	    			    __resize_screen( 320*6, 240 *6);
			    csbMessage.type = UIM_SETOPTION;
			    csbMessage.p1 = OPT_QUADRUPLE;
			    csbMessage.p2 = 2;
			    break;
	case 2:
//	case 0:
	default:
			    __resize_screen(WindowWidth,WindowHeight);
			    screenSize=2;
			    csbMessage.type = UIM_SETOPTION;
			    csbMessage.p1 = OPT_DOUBLE;
			    csbMessage.p2 = 2;
			    break;
    }// End of switch(screenSize)
    if (CSBUI(&csbMessage) != UI_STATUS_NORMAL){
	PostQuitMessage(0);
    }
    /* Setup a 50ms timer. */
    timer = SDL_AddTimer(TImER?TImER:10, __timer_callback, GINT_TO_POINTER(IDC_Timer));
    printf("Timer: %d ms established...\n", TImER?TImER:10); 




    static bool inWindow;
    static bool cursorIsShowing;
    while (TRUE)
	//SDL_WaitEvent(&evert)) 
	{

#ifdef USE_OLD_GTK
	    gtk_main_iteration_do(FALSE); // This solution will allways cost CPU instead of gtk_main()
#endif //USE_OLD_GTK
//            evert = app.WaitEvent();
	    if(!SDL_WaitEvent(&evert)) continue;

	    /* Listen for 'quick buttons' here. */
	    /* Hail to the Great Message Struct! */
	    MTRACE("msg=");
	    switch( evert.type ) 
            {
	      case SDL_QUIT:  //evert.type
	        MTRACE("SDL_QUIT\n");
	        cbAppDestroy();
	        exit(0);
	        break;
	      default: //evert.type
                  MTRACE("was ignored\n");
		   break;

	      case SDL_MOUSEMOTION:  //evert.type
                {
		  MTRACE("SDL_MOUSEMOTION\n");
		  SDL_MouseMotionEvent *e = (SDL_MouseMotionEvent*) &evert;
		    mouseX = e->x; //X_TO_CSB(e->x);
		    mouseY = e->y; //Y_TO_CSB(e->y);
		    i32 st_mouseX = X_TO_CSB(mouseX,screenSize);
		    i32 st_mouseY = Y_TO_CSB(mouseY,screenSize);
		    i32 id_st_mouseX = st_mouseX>16 ? st_mouseX - 16 : 0;
		    i32 id_st_mouseY = st_mouseY>16 ? st_mouseY - 16 : 0;
		//	if( noDirtyRect ) {
		      DirtyRect.x = id_st_mouseX;
		      DirtyRect.y = id_st_mouseY;
		      DirtyRect.w = 32;
		      DirtyRect.h = 32;
		      noDirtyRect = FALSE;
		//	} else {
				// Fancy mousehandling - currently not needed because of how LinScreen works.
//				i32 id_st_mouseXp = id_st_mouseX <320-32 ? id_st_mouseX +32 : 320 ;
//				i32 id_st_mouseYp =id_st_mouseY <200-32 ? id_st_mouseY +32 : 200 ;
//				DirtyRect.x = id_st_mouseX < DirtyRect.x ? id_st_mouseX : DirtyRect.x ;
//				DirtyRect.y = id_st_mouseY < DirtyRect.y ? id_st_mouseY : DirtyRect.y ;
//				DirtyRect.w = id_st_mouseXp > DirtyRect.x + DirtyRect.w ? id_st_mouseXp - DirtyRect.x : DirtyRect.w ;
//				DirtyRect.h = id_st_mouseYp > DirtyRect.y + DirtyRect.h ? id_st_mouseYp - DirtyRect.y : DirtyRect.h ;
//				DirtyRect.x = CLAMP(id_st_mouseX,0,DirtyRect.x);
//				DirtyRect.y = CLAMP(id_st_mouseY,0,DirtyRect.y);
//				DirtyRect.w = CLAMP(id_st_mouseX+32-DirtyRect.x,DirtyRect.w,320-DirtyRect.x);
//				DirtyRect.h = CLAMP(id_st_mouseY+32-DirtyRect.y,DirtyRect.h,200-DirtyRect.y);
		//	}
		    inWindow = true;
		    if (st_mouseY > 200)
			inWindow = false;
		    if (st_mouseY < 1)
			inWindow = false;
		    if (st_mouseX > 320)
			inWindow = false;
		    if (st_mouseX < 1)
			inWindow = false;
		    if (GameMode == 1)
		    {
		      if (inWindow)
		      {
		        if (cursorIsShowing)
		        {
			  SDL_ShowCursor(SDL_DISABLE);
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
			  SDL_ShowCursor(SDL_ENABLE);
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
		        SDL_ShowCursor(SDL_ENABLE);
		        cursorIsShowing = true;
		      };
		    };
		} break;

	      case SDL_USEREVENT: { // evert.type  __timer_loopback
		MTRACE("sdl_userevent->");
		SDL_UserEvent *e = (SDL_UserEvent*) &evert;
		switch( e->code ) 
                {// the menu (and timer)
		  case IDC_Normal: //e->code
		    MTRACE("IDC_Normal\n");
		    __resize_screen( 320, 240 );
		    csbMessage.type = UIM_SETOPTION;
		    csbMessage.p1 = OPT_NORMAL;
		    csbMessage.p2 = 2; //2-(screenSize==1); //new value
		    if (CSBUI(&csbMessage) != UI_STATUS_NORMAL)
		    {
		      PostQuitMessage(0);
		    }
                    break;
		  case IDC_Timer: //e->code
                    MTRACE("IDC_Timer\n");
		    if(GameMode != 1) 
                    {
                      LIN_Invalidate();
                    };
		    csbMessage.type=UIM_TIMER;
		    if (CSBUI(&csbMessage) != UI_STATUS_NORMAL)
		    {
		      PostQuitMessage(0);
		    }; 
                    break;
			  /*
			  * BEGIN the evil VIDEOEXPOSE
			  */
		  case SDL_VIDEOEXPOSE: //e->code
                  {
		    MTRACE("SDL_VIDEOEXPOSE\n");
		    //printf("SDL_VIDEOEXPOSE\n");
		    line++;
		    line = line%10;
		    // memset( SCR, line, 40 );
		    // SDL_BlitSurface(SCR, NULL, WND, NULL );
		    switch(line)
		    {
		      case 0://line
		        // g_print("vblint = %u",VBLInterruptCount);
		        break;
		      case 1://line
		        // g_print("chkvbl = %u",CheckVBLCount);
			break;
			switch(line)
			{
			  case 0: //line
		            // g_print("vblint = %u",VBLInterruptCount);
			    break;
			  case 1://line
		            // g_print("chkvbl = %u",CheckVBLCount);
			    break;
			  case 2://line
		            // g_print("STblt = %u",STBLTCount);
			    break;
			  case 3://line
			    g_print("Time = %u",GameTime);
			    break;
			  case 4://line
			    g_print(  "Skill[%d]=0x%08x      ",
				      MostRecentlyAdjustedSkills[0],
				      LatestSkillValues[0]);
			    break;
			  case 5://line
			    g_print(  "Skill[%d]=0x%08x      ",
				      MostRecentlyAdjustedSkills[1],
				      LatestSkillValues[1]);
			    break;
			  case 6://line
			    switch (latestCharType)
			    {
			      case TYPEIGNORED: //latestCharType
          	                g_print( "%04x key --> Ignored                         ", latestCharp1);
			        break;
			      case TYPEKEY://latestCharType
			        g_print( "%04x key --> Translated %08x", latestCharp1, latestCharXlate);
			        break;
			    };
			    break;
			  case 7://line
		            switch (latestScanType)
		            {
		    	      case TYPESCAN://latestScanType
			      case TYPEIGNORED://latestScanType
				g_print( "%08x mscan --> Ignored                        ",latestScanp1);
				break;
			      case TYPEMSCANL://latestScanType
				g_print( "%08x mscan --> Translated to %08x L",latestScanp1,latestScanXlate);
				break;
			      case TYPEMSCANR://latestScanType
			        g_print( "%08x mscan --> Translated to %08x R",latestScanp1,latestScanXlate);
			        break;
			      default://latestScanType
			        g_print("                                              ");
			    };//switch (laatestScanType)
			    break;
			  case 8: //line
			    switch (latestScanType)
			    {
			      case TYPEMSCANL://latestScanType
			      case TYPEIGNORED://latestScanType
			      case TYPEMSCANR://latestScanType
				g_print( "%08x scan --> Ignored                        ",latestScanp2);
				break;
			      case TYPESCAN://latestScanType
				g_print( "%08x scan --> Translated to %08x", latestScanp2,latestScanXlate);
				break;
			      default://latestScanType
				g_print("                                              ");
			    };
			     break ;
			  case 9://line
			    g_print("Total Moves = %d",totalMoveCount);
			    break;
			}; //switch (line)
		        //TextOut(hdc,325,25+15*line,msg,strlen(msg));
		        //		    g_warning(msg);
		    }; //switch (line)
	 	    /*
		     * This message should be sent when the screen has been erased?
		     *
		     */
/*		    csbMessage.type=UIM_REDRAW_ENTIRE_SCREEN;
		    if (CSBUI(&csbMessage) != UI_STATUS_NORMAL)
		    {
		      PostQuitMessage(0);
		     break;
		    };*/
		    /*
		     * Finally, write to the screen!
		     */
		    csbMessage.type=UIM_PAINT;
		    if (CSBUI(&csbMessage) != UI_STATUS_NORMAL)
		    {
		      PostQuitMessage(0);
		      break;
		    };
		  }; 
                  break;/*
			* END of VIDEOEXPOSE
			*/
		  case IDC_Double: //e->code
	            MTRACE("IDC_Double\n");
		    __resize_screen( 320*2, 240*2);
		    csbMessage.type = UIM_SETOPTION;
		    csbMessage.p1 = OPT_DOUBLE;
		    csbMessage.p2 = 2; //2-(screenSize==2); //new value
		    if (CSBUI(&csbMessage) != UI_STATUS_NORMAL)
		    {
		      PostQuitMessage(0);
		      break;
		    };
		    break;
		  case IDC_Triple://e->code
		    MTRACE("IDC_Triple\n");
		    __resize_screen( 320*3, 240*3 );
		    csbMessage.type = UIM_SETOPTION;
	            csbMessage.p1 = OPT_TRIPLE;
		    csbMessage.p2 = 2; //2-(screenSize==3); //new value
		    if (CSBUI(&csbMessage) != UI_STATUS_NORMAL)
		    {
		      PostQuitMessage(0);
		      break;
		    };
		    break;
		  case IDC_Quadruple://e->code
		    MTRACE("IDC_Quadruple\n");
		    __resize_screen( 320*4, 240*4 );
		    csbMessage.type = UIM_SETOPTION;
		    csbMessage.p1 = OPT_QUADRUPLE;
		    csbMessage.p2 = 2; //2-(screenSize==4); //new value
		    if (CSBUI(&csbMessage) != UI_STATUS_NORMAL)
		    {
		      PostQuitMessage(0);
		      break;
		    };
		    break;
		  case IDC_Quintuple://e->code
		    MTRACE("IDC_Quintuple\n");
		    __resize_screen( 320*5, 240*5 );
		    csbMessage.type = UIM_SETOPTION;
		    csbMessage.p1 = OPT_QUINTUPLE;
		    csbMessage.p2 = 2; //2-(screenSize==4); //new value
		    if (CSBUI(&csbMessage) != UI_STATUS_NORMAL)
		    {
		      PostQuitMessage(0);
		      break;
		    };
		    break;
		  case IDC_Sextuple://e->code
		    MTRACE("IDC_Sextuple\n");
		    __resize_screen( 320*6, 240*6 );
		    csbMessage.type = UIM_SETOPTION;
		    csbMessage.p1 = OPT_SEXTUPLE;
		    csbMessage.p2 = 2; //2-(screenSize==4); //new value
		    if (CSBUI(&csbMessage) != UI_STATUS_NORMAL)
		    {
		      PostQuitMessage(0);
		      break;
		    };
		    break;
		  case IDC_QuickPlay://e->code
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
		  case IDM_Glacial://e->code
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
		  case IDM_Molasses://e->code
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
		  case IDM_VerySlow://e->coe
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
		  case IDM_Slow://e->code
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
		  case IDM_Normal://e->code
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
		  case IDM_Fast://e->code
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
		  case IDM_Quick://e->code
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
		  case IDM_PlayerClock://e->code
		    MTRACE("IDM_PlayerClock\n");
		    csbMessage.type = UIM_SETOPTION;
		    csbMessage.p1 = OPT_PLAYERCLOCK;
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
		  case IDC_Record://e->code
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
		  case IDC_TimerTrace://e->code
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
		  case IDM_DMRULES://e->code
		    MTRACE("IDM_DMRULES\n");
		    csbMessage.type = UIM_SETOPTION;
		    csbMessage.p1 = OPT_DMRULES;
		    if (CSBUI(&csbMessage) != UI_STATUS_NORMAL)
		    {
		      PostQuitMessage(0);
		      break;
		    };
		    break;
		  case IDC_AttackTrace://e->code
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
                  case IDC_AITrace://e->code
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
		  case IDC_ItemsRemaining://e->code
		    MTRACE("IDC_ItemsRemaining\n");
		    csbMessage.type = UIM_SETOPTION;
		    csbMessage.p1 = OPT_ITEMSREMAINING;
		    if (CSBUI(&csbMessage) != UI_STATUS_NORMAL)
		    {
		      PostQuitMessage(0);
		      break;
		    };
		    break;
        	  case IDC_NonCSBItemsRemaining://e->code
        	    MTRACE("IDC_NonCSBItemsRemaining\n");
        	    csbMessage.type = UIM_SETOPTION;
        	    csbMessage.p1 = OPT_NONCSBITEMSREMAINING;
        	    if (CSBUI(&csbMessage) != UI_STATUS_NORMAL)
		    {
              	      PostQuitMessage(0);
		      break;
		    };
		    break;
		  case IDC_Playback://e->code
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
		  case IDC_DispatchTrace://e->code
		    MTRACE("IDC_DispatchTrace\n");
		    if (trace  >= 0 ) 
		    {
		      CLOSE(trace);
		      trace = -1;
		      break;
		    };
		    trace = CREATE("trace.log","w", true);
		    break;
                  case IDM_GraphicTrace://e->code
                    MTRACE("IDC_GraphicTrace\n");
                    csbMessage.type = UIM_SETOPTION;
                    csbMessage.p1 = OPT_GRAPHICTRACE;
                    csbMessage.p2 = 1;
                    if (TimerTraceActive) csbMessage.p2 = 0;
                    if (CSBUI(&csbMessage) != UI_STATUS_NORMAL)
                    {
                      PostQuitMessage(0);
                      break;
                    };
                  case IDC_DSATrace://e->code
                    MTRACE("IDC_DSATrace\n");
                    csbMessage.type = UIM_SETOPTION;
                    csbMessage.p1 = OPT_DSATRACE;
                    csbMessage.p2 = 1;   
                    if (DSATraceActive) csbMessage.p2 = 0;
                    if (CSBUI(&csbMessage) != UI_STATUS_NORMAL)
                    {
                      PostQuitMessage(0);
                      break;
                    };
                    break;  
 		  default://e->code
                    printf("Unknown SDL_USEREVENT\n");
		    MTRACE("Unknown SDL_USEREVENT\n");
		    //return DefWindowProc(hWnd, message, wParam, lParam);
		    break;
		  };//switch (e->code)
	        };
                break; /* case (evert.type ==  SDL_USEREVENT */
	      case SDL_MOUSEBUTTONDOWN://evert->type
                MTRACE("SDL_MOUSEBUTTONDOWN->");
	        {
	          SDL_MouseButtonEvent  *e = (SDL_MouseButtonEvent*) &evert;
	          switch(e->button)
		  {// btn down, left or right
		    case SDL_BUTTON_LEFT://e->button
		      MTRACE("SDL_BUTTON_LEFT\n");
		      printf("left ");
		      csbMessage.type=UIM_LEFT_BUTTON_DOWN;
		      csbMessage.p1 = mouseX = e->x;  // horizontal position of cursor 
		      csbMessage.p2 = mouseY = e->y;  // vertical position of cursor 
		      if (CSBUI(&csbMessage) != UI_STATUS_NORMAL)
		      {
			PostQuitMessage(0);
			break;
		      }
		      break;
		    case SDL_BUTTON_RIGHT://e->button
		      MTRACE("SDL_BUTTON_RIGHT\n");
		      printf("right ");
		      csbMessage.type=UIM_RIGHT_BUTTON_DOWN;
		      csbMessage.p1 = mouseX = e->x;  // horizontal position of cursor 
		      csbMessage.p2 = mouseY = e->y;    // vertical position of cursor 
		      if (CSBUI(&csbMessage) != UI_STATUS_NORMAL)
		      {
		 	PostQuitMessage(0);
			break;
		      };
		      break;
		  };//switch (e->button)
		};
		printf("at (%x, %x)\n", X_TO_CSB(mouseX,screenSize),Y_TO_CSB(mouseY,screenSize));
		break; // Eof SDL_MOUSEBUTTONDOWN
	      case SDL_MOUSEBUTTONUP:
	        MTRACE("SDL_MOUSEBUTTONUP->");
	        {// btn up, left or right
 		  SDL_MouseButtonEvent  *e = (SDL_MouseButtonEvent*) &evert;
		  switch(e->button)
		  {
		    case SDL_BUTTON_RIGHT:
		      MTRACE("SDL_BUTTON_RIGHT\n");
		      csbMessage.type=UIM_RIGHT_BUTTON_UP;
		      csbMessage.p1 = mouseX = e->x;  // horizontal position of cursor 
		      csbMessage.p2 = mouseY = e->y; // vertical position of cursor 
		      if (CSBUI(&csbMessage) != UI_STATUS_NORMAL)
		      {
		        PostQuitMessage(0);
		        break;
		      };
		      break;
		    case SDL_BUTTON_LEFT:
	  	      MTRACE("SDL_BUTTON_LEFT\n");
		      csbMessage.type=UIM_LEFT_BUTTON_UP;
		      csbMessage.p1 = mouseX = e->x;   // horizontal position of cursor 
		      csbMessage.p2 = mouseY = e->y;  // vertical position of cursor 
		      if (CSBUI(&csbMessage) != UI_STATUS_NORMAL)
		      {
		        PostQuitMessage(0);
		        break;
		      };
		      break;
		    };
	          };
		  break;  //Eof SDL_MOUSEBUTTONUP
// /////////////////////////////////////////////////
		case SDL_KEYDOWN:
	          MTRACE("SDL_KEYDOWN\n");
		  {
		    /*SDL_KeyboardEvent *e = (SDL_KeyboardEvent*) &evert;
		      if(e->keysym->sym==SDLK_LEFT)
		      printf("left ");
		      if else(e->keysym->sym==SDLK_RIGHT)
		      printf("right ");
                    */
		    csbMessage.type=UIM_KEYDOWN;
		    csbMessage.p1 =  evert.key.keysym.sym;	//virtual key
		    csbMessage.p2 = (evert.key.keysym.scancode>>16)&0xff; //scancode
		    printf("Key 0x%x pressed\n",(int)csbMessage.p1);
		    if (CSBUI(&csbMessage) != UI_STATUS_NORMAL)
                    {
		      PostQuitMessage(0);
		      break;
		    };
		  };
                  break;
	        case SDL_KEYUP:
		  MTRACE("SDL_KEYUP\n");
		  csbMessage.type=UIM_CHAR;
		  csbMessage.p1 = evert.key.keysym.sym;
		  if (CSBUI(&csbMessage) != UI_STATUS_NORMAL)
		  {
		    PostQuitMessage(0);
		    break;
		  };
		  break;
		case SDL_VIDEORESIZE: 
                  {
                    MTRACE("SDL_VIDEORESIZE\n");
		    SDL_ResizeEvent *e = (SDL_ResizeEvent*) &evert;
		    WindowWidth = e->w;
		    WindowHeight = e->h;
		    st_X = 320.0 / ((float)WindowWidth);
		    st_Y = 200.0 / ((float)WindowHeight);
		    SDL_FreeSurface(WND);
		    WND = SDL_SetVideoMode(WindowWidth,  WindowHeight, 0, SDL_HWSURFACE|SDL_SWSURFACE|SDL_RESIZABLE|SDL_ANYFORMAT);

#ifndef _DYN_WINSIZE
		    switch(screenSize) 
                    {
		      default:
		      case 1: SDL_FreeSurface(SCRAP);	SCRAP = SDL_CreateRGBSurface(SDL_SWSURFACE,320*1,200*1,8,0,0,0,0); break;
		      case 2: SDL_FreeSurface(SCRAP);	SCRAP = SDL_CreateRGBSurface(SDL_SWSURFACE,320*2,200*2,8,0,0,0,0); break;
		      case 3: SDL_FreeSurface(SCRAP);	SCRAP = SDL_CreateRGBSurface(SDL_SWSURFACE,320*3,200*3,8,0,0,0,0); break;
		      case 4: SDL_FreeSurface(SCRAP);	SCRAP = SDL_CreateRGBSurface(SDL_SWSURFACE,320*4,200*4,8,0,0,0,0); break;
		    };// switch (screensize)
#endif //! _DYN_WINSIZE
		    csbMessage.type=UIM_REDRAW_ENTIRE_SCREEN;
		    if (CSBUI(&csbMessage) != UI_STATUS_NORMAL)
		    {
		      PostQuitMessage(0);
		      break;
		    };
		    UI_Invalidate();
	          };
                  break; //case SDL_VIDEORESIZE
			
   /*           case SDL_ACTIVEEVENT: 
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
  //            break; //falltrhrough till VIDEOEXPOSED
	

	    }; /* switch (evert.type)  Eof Great Event Switch */
	} /* Eof Lord Message Loop */

    printf("Quiting SDL.\n");
    
    /* Shutdown all subsystems */
    SDL_Quit();
    
    printf("Quiting....\n");

    exit(0);
    return (0);
}
