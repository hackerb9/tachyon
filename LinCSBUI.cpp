#ifdef _LINUX
#include "stdafx.h"
//#include <csl/csl.h> // ********************************* rabbahiff
#ifdef SOUND__ESOUND
# include <esd.h>
#endif//SOUND__ESOUND
#include <unistd.h>
#include "SoundMixer.h"

# include <sys/time.h>
# include <ctype.h>
# include "CSBTypes.h"

#include <stdio.h>

#include "UI.h"
#include "Objects.h"
#include "Dispatch.h"
#include "CSB.h"
#include "Data.h"

extern int eventNum;
extern void ReadConfigFile(void);


void EnqueMouseClick(i32, i32, i32);
void  TAG001afe(i32, i32, i32);
i32 AddSD(char *, i32, float, float, float);
void ItemsRemaining(i32 mode);
//char *parentFolder(char *folderName, char *endName);

extern i32 WindowHeight;
extern i32 WindowWidth;
extern i32 WindowX;
extern i32 WindowY;
extern bool fullscreenRequested;
extern bool virtualFullscreen;

extern ui32 TImER;
extern i32 VBLperTimer;
extern i32 VBLMultiplier;
extern i32 screenSize;
extern i32 keyboardMode;
extern i32 trace;

extern bool PlayfileIsOpen();

void PlayfileOpen(bool);
void RecordfileOpen(bool);
void RecordfilePreOpen(void);
extern bool BeginRecordOK;
extern bool TimerTraceActive;
extern bool AttackTraceActive;
//extern bool GraphicTraceActive;
//extern i32  traceViewportDrawing;
extern bool RepeatGame;
extern i32 NoSpeedLimit;
bool OpenTraceFile(void);
void CloseTraceFile(void);
extern bool cursorIsShowing;
extern bool RecordCommandOption;
extern bool NoRecordCommandOption;
extern bool invisibleMonsters ;//= false;
extern bool drawAsSize4Monsters ;//= false;
//bool extendedPortraits = false;

extern HWND hWnd;
extern HDC hdc;

extern const char* PlayfileName;
extern bool PlaybackCommandOption;
extern bool RecordCommandOption;
extern bool NoRecordCommandOption;
extern bool RecordDesignOption;
extern bool DMRulesDesignOption;
extern bool RecordMenuOption;
extern bool NoRecordMenuOption;
extern bool extendedPortraits;

//char* folderSavedGame;



#ifdef _LINUX
#define strupr( a ) _strupr( a )
static void _strupr(char *str) {
    if( str ) {
	while(*str) {
	    *str = toupper( *str );
	    str++;
	}
    }
}
#endif


#if 1
extern i32 CSBUI(CSB_UI_MESSAGE *msg);
#else

i32 CSBUI(CSB_UI_MESSAGE *msg)
{
  i32 CallCheckVBL= 0;
  //printf("Lin_CSBUI type=%d\n", msg->type);
  if (messageMask == 0) return UI_STATUS_NORMAL;
  try
  {
    switch (msg->type)
    {
    case UIM_INITIALIZE:
        ReadConfigFile();
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
        break;
      case OPT_NORMAL:
        screenSize = 1;
        break;
      case OPT_DOUBLE:
        if (msg->p2 == 1) screenSize = 1;
        else screenSize = 2;
        break;
      case OPT_TRIPLE:
        if (msg->p2 == 1) screenSize = 1;
        else screenSize = 3;
        break;
      case OPT_QUADRUPLE:
        if (msg->p2 == 1) screenSize = 1;
        else screenSize = 4;
        break;
      case OPT_RECORD:
        if (msg->p2 == 1)
	{
		RecordMenuOption = true; 
		NoRecordMenuOption = false;
	}
	else {
		//RecordfileOpen(true);
		RecordfileOpen(false);
		RecordMenuOption = false;
		NoRecordMenuOption = true;
	}
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
      case OPT_DIRECTX:
        // No X except X. //usingDirectX = !usingDirectX;
        break;
      case OPT_PLAYERCLOCK:
        playerClock = !playerClock;
        break;
      case OPT_EXTRATICKS:
        extraTicks = !extraTicks;
        break;
      case OPT_ITEMSREMAINING:
        ItemsRemaining(0);// "Defined" in AsciiDump.cpp
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
          if (AttackTraceActive || AITraceActive  || OpenTraceFile())
          {
            TimerTraceActive = true;
          };
        };
        break;
        case OPT_DSATRACE:
        	DSATraceActive = !DSATraceActive;
        	break;
	case OPT_GRAPHICTRACE:
		traceViewportDrawing = 1 - traceViewportDrawing;
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
        ASSERT(0,"no text");
      };
      break;
    case UIM_TERMINATE:
        return UI_STATUS_TERMINATE; 
    case UIM_CHAR:
        {
          latestCharType = TYPEIGNORED;
          i32 key = keyxlate.translate(msg->p1, keyboardMode, TYPEKEY);
          if (key != 0)
          {
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
          latestScanp1 = msg->p1;
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
        CallCheckVBL = VBLperTimer*VBLMultiplier; // do it 10 times
        break;
    case UIM_PAINT:
        display();
        break;
    case UIM_REDRAW_ENTIRE_SCREEN:
        ForceScreenDraw();
        UI_Invalidate();
        break;
    case UIM_LEFT_BUTTON_DOWN:
#ifdef MAEMO_NOKIA_770
	// Stylus based systems such as the Nokia 770
	// need to have the current mouse position updated
	// before the mouse click action is checked
	checkVBL();
#endif
	OnMouseSwitchAction(0x2);
	if (virtualFullscreen)
	{
		i32 x,y,size;
		size = screenSize;
		TranslateFullscreen(msg->p1,msg->p2,x,y);
		EnqueMouseClick(x, y, 1);
		TAG001afe(x, y, 1);
	}
	else
	{
		EnqueMouseClick(X_TO_CSB(msg->p1,screenSize), Y_TO_CSB(msg->p2,screenSize),1);
		TAG001afe(X_TO_CSB(msg->p1,screenSize), Y_TO_CSB(msg->p2,screenSize), 1);
	}
	UI_PushMessage(msg->type);
	break;
    case UIM_LEFT_BUTTON_UP:
        OnMouseSwitchAction(0x0);
#if (MAEMO_NOKIA_770 || _LINUX)
	// For some reason the Nokia requires the following
	// line for unclicks on the Eye to work.
	OnMouseUnClick();
#endif
        break;
    case UIM_RIGHT_BUTTON_DOWN:
        OnMouseSwitchAction(0x1);
        if (virtualFullscreen)
        {
          i32 x, y, size;
          size = screenSize;
          TranslateFullscreen(msg->p1,msg->p2,x,y);
          EnqueMouseClick(x, y, 1);
          TAG001afe(x, y, 1);
        }
        else
	{
		EnqueMouseClick(X_TO_CSB(msg->p1,screenSize), Y_TO_CSB(msg->p2,screenSize),1); //Chaos input
		TAG001afe(X_TO_CSB(msg->p2,screenSize), Y_TO_CSB(msg->p1,screenSize), 1);
	}
        UI_PushMessage(msg->type);
        break;
    case UIM_RIGHT_BUTTON_UP:
        OnMouseSwitchAction(0x0);
        break;
    case UIM_EditGameInfo:
	    //do nothing
    	break;
    default:
      ASSERT(0, "no text");
    };
    do
    {
      if (CallCheckVBL > 0)
      {
        if (eventNum < 20) printf("Attempt checkVBL()\n");
        checkVBL();
        CallCheckVBL--;
      };
      while (msgStackLen != 0)
      {
        msgStackLen--;
        if (msgStack[msgStackLen].type == UIM_TERMINATE)
        {
          return UI_STATUS_TERMINATE;
        };
        if (eventNum < 20) printf("Attempt DispatchCSB()\n");
        DispatchCSB(&msgStack[msgStackLen]);
      };
    } while (CallCheckVBL > 0);
      return UI_STATUS_NORMAL;
  }
  catch (i32)
  {
    g_warning("Exception caught in: i32 CSBUI(CSB_UI_MESSAGE *msg)\n");
    return UI_STATUS_TERMINATE;
  };
};
#endif

#ifdef USE_OLD_GTK
volatile gboolean dialog_not_clicked = TRUE;
volatile gint dialog_answer;
void __dialog_release( gpointer A ) { dialog_not_clicked = FALSE; }
void __dialog_unset( gpointer A ) { dialog_answer = 1; dialog_not_clicked = FALSE; gtk_widget_destroy(GTK_WIDGET(A)); }
void __dialog_reset( gpointer A ) { dialog_answer = 0; dialog_not_clicked = FALSE; gtk_widget_destroy(GTK_WIDGET(A)); }
#endif //USE_OLD_GTK

i32 UI_MessageBox(const char *msg, const char *title, i32 flags ) {
#ifdef USE_OLD_GTK
    printf("\nMessageBox: [%s] %s", title, msg);
    GtkWidget *dialog;
    GString *text = g_string_new(msg);

  i32 i=1;// default answer
    if(title==NULL){
	title="Error: \n";
    }
    else
    {
	text = g_string_prepend(text,": \n");
    }
    text = g_string_prepend(text,title);
  bool saveCursorShowing, cursorIsShowing, is_ok=false;
  saveCursorShowing = cursorIsShowing = ( SDL_ENABLE == SDL_ShowCursor(SDL_QUERY) );
  if (!cursorIsShowing) SDL_ShowCursor(SDL_ENABLE);
  
    GtkWidget *label, *yes_button, *no_button, *ok_button;
	dialog = gtk_dialog_new();
	label = gtk_label_new (text->str);
	dialog_not_clicked = TRUE;
	dialog_answer = 1;


      if (flags & MESSAGE_OK){
	ok_button = gtk_button_new_with_label("Okay");
	gtk_signal_connect_object (GTK_OBJECT (ok_button), "clicked",GTK_SIGNAL_FUNC (__dialog_reset),GTK_OBJECT( dialog ));
	gtk_container_add (GTK_CONTAINER (GTK_DIALOG(dialog)->action_area),ok_button);
	is_ok=true;
  }
    else if (flags & MESSAGE_YESNO){
	yes_button = gtk_button_new_with_label("Yes");
	no_button = gtk_button_new_with_label("No");
	gtk_signal_connect_object (GTK_OBJECT (yes_button), "clicked",GTK_SIGNAL_FUNC (__dialog_reset), GTK_OBJECT(dialog));
	gtk_signal_connect_object (GTK_OBJECT (no_button), "clicked",GTK_SIGNAL_FUNC (__dialog_unset), GTK_OBJECT(dialog));
	gtk_container_add (GTK_CONTAINER (GTK_DIALOG(dialog)->action_area),yes_button);
	gtk_container_add (GTK_CONTAINER (GTK_DIALOG(dialog)->action_area),no_button);
   }   
   GtkWidget *hbox = gtk_hbox_new(FALSE,10);
   gtk_container_add (GTK_CONTAINER (GTK_DIALOG(dialog)->vbox),hbox);
   gtk_container_add (GTK_CONTAINER (hbox),gtk_label_new(""));
   gtk_container_add (GTK_CONTAINER (hbox),label);
   gtk_container_add (GTK_CONTAINER (hbox),gtk_label_new(""));
   gtk_window_set_modal(GTK_WINDOW(dialog),TRUE);
   gtk_signal_connect_object (GTK_OBJECT (dialog), "destroy",GTK_SIGNAL_FUNC (__dialog_release),GTK_OBJECT( dialog ));
   gtk_widget_show_all (dialog);

	while(dialog_not_clicked) gtk_main_iteration_do(FALSE);
	i = dialog_answer;
	i32 mask = UI_DisableAllMessages();

  UI_EnableMessages(mask);
  if (!saveCursorShowing) SDL_ShowCursor(SDL_DISABLE);
  if (i == 0 && is_ok == false) return MESSAGE_IDYES; // 0 == first button pressed
  if (i == 1 || i == -1)  return MESSAGE_IDNO; // 1 == second button pressed, (-1) == if MsgBox is 'destroyed'
  g_string_free(text,TRUE);

#else
  printf("Message:\n");
  printf("  Flags = %08x\n", flags);
  printf("  Title = %s\n", title);
  printf("  Message = %s\n", msg);
#endif //USE_OLD_GTK

  return MESSAGE_IDOK;
}


/* 
* The audio management
*/

const unsigned int WAV_OFFSET = 58;
static const unsigned int AUDIOCOUNT = 3;//16;
static unsigned int AUDIOPOS = 0;
static int AUDIO[AUDIOCOUNT];
static i8* AUDIODATA[AUDIOCOUNT];
static ui32 AUDIOSIZE[AUDIOCOUNT];
static ui32 AUDIOWRITTEN[AUDIOCOUNT];
#include <signal.h>
#include <fcntl.h>


void UI_Initialize_sounds(void) 
{
  Mix_OpenAudio(7000, AUDIO_U8, 1, 128);
//  printf("InitializeSounds not implemented\n");
}

void UI_StopSound(void)
{
	// FIXME: Stop all sound, or just ignore this call?
}

struct SNDHEAD
{
  i8  byte0[4];           //"RIFF"
  i32 Size;               //  6076  // #bytes after this integer
  i8  byte8[8];           //"WAVEfmt "
  i32 int16;              //    18
  i16 wFormatTag;         //     1
  i16 nChannels;          //     1
  i32 nSamplesPerSecond;  // 11025
  i32 nAvgBytesPerSec;    // 11025 // important
  i16 nBlockAlign;        //     1 // 2 for 16-bit
  i16 wBitsPerSample;     //     8
  //i16 cbSize;             //    40
  //i8  byte38[4];          // "fact"
  //i32 int42;              //     4
  //i32 numBytes46;         //
  i8  byte50[4];          // "data"
  i32 numSamples54;       //
  i8  sample58[1];
}
#ifdef   __GNUC__
	__attribute__((packed))
#endif //__GNUC__
;

class Channels /* To keep track of active channels and 
                * each channel's data.  We need to free the 
                * data when a channel has completed playing.
                *
                * We will use this class to:
                * 1 - Find an inactive channel when one is needed
                * 2 - Free inactive chunks and memory
                * 3 - Allocate new chunks and memory
                * 3 - Clean up at program termination.
                */
{
private:
  Mix_Chunk *chunks[MIX_CHANNELS];
  int numSample[MIX_CHANNELS];
  SNDHEAD *m_sndHead[MIX_CHANNELS];
public:
  Channels(void);
  ~Channels(void);
  void Play(SNDHEAD *sndHead, int numSamples);
};

Channels::Channels(void)
{
  int i;
  for (i=0; i<MIX_CHANNELS; i++)
  {
    chunks[i] = NULL;
    numSample[i] = 0;
    m_sndHead[i] = NULL;
  };
}

Channels::~Channels(void)
{
  int i;
  for (i=0; i<MIX_CHANNELS; i++)
  {
    if (chunks[i] != NULL)
    Mix_FreeChunk(chunks[i]);
    if (m_sndHead[i] != NULL) UI_free(m_sndHead[i]);
  };
}

void Channels::Play(SNDHEAD *sndHead, int numSample)
{
  int i;
  ui8 *samples;
  samples = (ui8 *)sndHead->sample58;
  for (i=0; i<MIX_CHANNELS; i++)
  {
    if (Mix_Playing(i)) continue;
    if (chunks[i] != NULL)
    {
      UI_free(m_sndHead[i]);
      chunks[i]->abuf = NULL;
      Mix_FreeChunk(chunks[i]);
      chunks[i] = NULL;
    };
    //printf("Mix_QuickLoad_RAW @%d\n",d.Time);
    chunks[i] = Mix_QuickLoad_RAW(samples, numSample);
    if (chunks[i] == NULL)
    {
      UI_free(sndHead);
      return;
    };
    m_sndHead[i] = sndHead;
    //printf("Mix_PlayChannel @%d\n", d.Time);
    Mix_PlayChannel(i, chunks[i], 0);
    return;
  };
  UI_free(sndHead);
}

Channels sdlChannels;



/*
* Play a sound. We do currently not bother about volume, it just looks nice...
*/
static bool LIN_PlaySound(i8* audio, const ui32 /*size*/, int volume)
{
  ui8 *samples;
  int numSample;
  SNDHEAD *header;
  //printf("LIN_PlaySound @%d\n",d.Time);
  header = (SNDHEAD *)audio;
  //samples = header->sample58;
  numSample = header->numSamples54;
  sdlChannels.Play(header, numSample);
};


bool UI_PlaySound(const char *wave, i32 flags, i32 attenuation /*not used*/) {
//SDL_AudioSpec *SDL_LoadWAV(const char *file, SDL_AudioSpec *spec, Uint8 **audio_buf, Uint32 *audio_len);
  return LIN_PlaySound((pnt)wave/* + 58*/,((SNDHEAD*)wave)->Size - WAV_OFFSET+sizeof(i32), SDL_MIX_MAXVOLUME);
}

enum {
    		    enum_directory=42,
		    enum_ignore, // ignore = do not expect arguments
		    enum_height,
		    enum_quick,
		    enum_vblmultiplier,
		    enum_dungeon,
		    enum_width,
		    enum_x,
		    enum_y,
		    enum_module,
		    enum_extralarge,
		    enum_verylarge,
		    enum_large,
		    enum_small,
		    enum_play,
		    enum_timer,
		    enum_path,
    		    enum_gamsav,
	    };

bool UI_ProcessOption(char **argv, int &argc)
{
    int state=0;
    int c_index=0;
    int _argc = argc;
#ifdef SOUND__ESOUND
    doInitializeSounds = SOUND_IS_ESD;
#endif //SOUND__ESOUND
    while(_argc-->0)
    {
	char *key=argv[c_index++], *value;
	switch(state) {

		default:
		    if(!strcmp("--directory",key) ) {
			state=enum_directory;
                  }else if(!strcmp("--dmrules",key) ) {
                        DM_rules = true;
		    }else if(!strcmp("--module",key) || !strcmp("--modules",key) ) {
			state=enum_module;
		    }else if(!strcmp("--gamsav",key) ) {
			state=enum_gamsav;
		    }else if(!strcmp("--root-path",key) ) {
		    	state=enum_path;
		    }else if(!strcmp("--repeat",key) ) {
			state=enum_ignore;
			RepeatGame = true;
		    }else if(!strcmp("--height",key) ) {
			state=enum_height;
		    }else if(!strcmp("--quick",key) ) {
			state=enum_quick;
		   // }else if(!strcmp("--nosound",key) ) {
		   //state=enum_ignore;
		   //doInitializeSounds = SOUND_IS_OFF;
		   // }else if(!strcmp("--sound-pipe",key) || !strcmp("--soundpipe",key) ) {
		   //state=enum_ignore;
		   //doInitializeSounds = SOUND_IS_PIPE;
		    }else if(!strcmp("--vblmultiplier",key) ) {
			state=enum_vblmultiplier;
		    }else if(!strcmp("--dungeon",key) ) {
			state=enum_dungeon;
		    }else if(!strcmp("--width",key) ) {
			state=enum_width;
//		    }else if(!strcmp("--x",key) ) {
//			state=enum_x;
//		    }else if(!strcmp("--y",key) ) {
//			state=enum_y;
		    }else if(!strcmp("--fullscreen",key) ) {
			    state=enum_ignore;
			    fullscreenRequested = true;
                    }else if(!strcmp("--humongous",key)) {
                        state=enum_ignore; screenSize=6;
                        WindowHeight = 1200;
                        WindowWidth=1920; 
                    }else if(!strcmp("--extremelylarge",key)) {
                        state=enum_ignore; screenSize=5;
                        WindowHeight = 1000;
                        WindowWidth=1600; 
		    }else if(!strcmp("--extralarge",key) ) {
			state=enum_ignore; screenSize=4;
			WindowHeight=800;
			WindowWidth=1280;
		    }else if(!strcmp("--verylarge",key) ) {
			state=enum_ignore; screenSize=3;
			WindowHeight=600;
			WindowWidth=960;
		    }else if(!strcmp("--large",key) ) {
			state=enum_ignore; screenSize=2;
			WindowHeight=400;
			WindowWidth=640;
		    }else if(!strcmp("--small",key) ) {
			state=enum_ignore; screenSize=1;
			WindowHeight=200;
			WindowWidth=320;
		    }else if(!strcmp("--play",key) ) {
			state=enum_play;
		    }else if(!strcmp("--timer",key) ) {
			state=enum_timer;
		    }else if( !strcmp("--version",key) ){
			    exit(0);// CSB version is printed from main()
		    }else if( !strcmp("--norecord",key) ){
			    NoRecordCommandOption = true;
			    RecordCommandOption = false;
			    state=enum_ignore;
		    }else if( !strcmp("--record",key) ){
			    NoRecordCommandOption = false;
			    RecordCommandOption = true;
			    state=enum_ignore;
		    }else if(!strcmp("--help",key) || !strcmp("-help",key) || !strcmp("-h",key) || !strcmp("help",key) ) {
			printf("Help message:\n"
			"  Possible arguements are:\n"
//			"  --gamsav\n"
//				"\tUsage: --gamsav modules/chaos\\ strikes\\ back-atari/\n"
//				"\tThe directory where any saved game files are\n"
//				"\tThe game also tries to save in this directory\n"
//				"\tIf not set, then default is to save in your current working directory\n"
			"  --directory\n"	
				"\tUsage: --directory /tmp/\n"
			    	"\tThe directory which holds any game file\n"
			"  --module\n"
				"\tUsage: --module /tmp/\n"
			    	"\tThe directory which holds any other game file\n"
			"  --root-path\n"	
				"\tUsage: --root-path /tmp/\n"
			   	"\tThe fall-back directory if the above pathways failed\n"
//			"  --repeat\n"
//			"  --height\t in pixels\n"
//			"  --width\t in pixels\n"
			"  --quick\n"
				"\tPlay movie faster than light\n"
//			"  --vblmultiplier\n"
			"  --dungeon\n"
				"\tUsage: --dungeon dungeon.dat\n"
			    	"\tPlay the game in another dungeon.\n"
//			"  --x\n"
//			"  --y\n"
//			"  --fullscreen\n"
			"  --nosound\n"
				"\tTurn off all sound support. Default if no sounddriver has been\n"
				"\tcompiled in.\n"
//			"  --sound-pipe\n"
//				"\tUsage: --sound-pipe | esdcat -b -m -r 5120\n"
//				"\tPipe sound data to stdout.\n"
//				"\tThis data can be fed to a properly setup sound device,\n"
//				"\tbut the quality of the sound will wary.\n"
//				"\tThe sound data is 8-bit signed, mono and plays at 5120 Hz.\n"
//				"\tNote that --sound-pipe does not take any arguments.\n"
			"  --norecord\n"
			    	"\tPrevents a dungeon to autorecord a session, even if the dungeon\n"
				"\titself tries to force autorecord. It doesn't prevent you from\n"
				"\tmanually selecting record. Actually, there is no reason not to\n"
				"\tuse this switch.\n"
			"  --record\n"
			    	"\tAutorecord a session. Files will be stored in the location as\n"
			    	"\tspecified by the above arguments, such as --gamsav.\n"
			    	"\tThis option is usefull for debugging new dungeons.\n"
			"  --timer\n"
				"\tUsage: --timer 50\n"
			    	"\tSet internal callback-timer in ms.\n"
                        "  --extremelylarge\n"
                                "\tSet screensize to 1600x1000 pixels\n"
			"  --extralarge\n"
				"\tSet screensize to 1280x800 pixels\n"
			"  --verylarge\n"
				"\tSet screensize to 960x600 pixels\n"
			"  --large\n"
				"\tSet screensize to 640x400 pixels\n"
			"  --small\n"
				"\tSet screensize to 320x200 pixels\n"
			"  --play\n"
				"\tUsage: --play Playfile.log\n"
			    	"\tPlayes a recorded game. Might be reffered to as a \"movie\".\n"
			"  --version\n"
				"\tPrints the version of the program and exits.\n"
				"\tFirst numbers is the version of the CSB engine. The last number\n"
				"\t(after the 'v') counts extra bug-fixes to the Linux-client only.\n"
			" \n\n  FEATURES:\n"
#ifdef				SOUND__ESOUND
				"\tESound (esd) support.\n"
#endif				//SOUND__ESOUND
#ifdef				_DYN_WINSIZE
				"\tSupports any screen size (via hermes).\n"
#endif				//_DYN_WINSIZE
#ifdef				USE_OLD_GTK
				"\tExtra menu-options enabled (via gtk)\n"
#endif				//USE_OLD_GTK
			" \n\n  KNOWN BUGS:"
				"\tGiven the evolution of this program, it might be a good thing\n"
			    	"\tto list them here. \n\n"
//			    	"\t* Fullscreen is not working. \n"
//			    	"\t* The GTK menu feels slow because it's only updated every --timer\n"
//			    	"\t  settings. Set timer to 10 ms if you want the GTK interface to be\n"
//			    	"\t  faster. \n"
//			    	"\t* The DISPLAY variable is not honoured. \n"
//			    	"\t* Sound has always been a problem. We have now implemented an\n"
//			    	"\t  option to pipe sound to stdout, so that you can format the\n"
//			    	"\t  sound and send it to the soundcard by your own means. This\n"
//			    	"\t  is an experimental solution, which in our testing works\n"
//			    	"\t  quite bad.\n"
//			    	"\t \n"
			    	"\tIf you encounter a bug then catch it on record, send the Record000.txt\n"
				"\t(whatever) and the saved game you used to Paul R. Stevens\n"
				"\t<prsteven@facstaff.wisc.edu>.\n"
			    	"\tIf the bug is not possible to catch on record, then send it to Erik\n\tSvanberg <svanberg@acc.umu.se>.\n"

			);// end of printf()
			exit(0);// Exit after the help message has been displayed
		    } else {
			state = 0;
		    }

		    if(state){
			c_index--;
			memmove(&argv[c_index],&argv[c_index+1],(argc-c_index-1)*sizeof(char *));
			argc--;
			if(state == enum_ignore) state=0;
		    }
		continue;

		case enum_directory:
//		    root = key;
		    folderParentName = key; // Nasty hack. :o)
		    break;
		
		case enum_module:
		    folderName = key;
		    break;

		//case enum_gamsav:
		//    folderSavedGame = key;
		//    break;

		case enum_path:
		    root = key;
		    break;


		case enum_height:
		    sscanf(key,"%d", &WindowHeight);
		    break;
		
		case enum_timer:
		    sscanf(key,"%d", &TImER);
		    break;
		
		case enum_quick:
		    sscanf(key,"%d", &NoSpeedLimit);
		    break;
		
		case enum_vblmultiplier:
		    sscanf(key,"%d", &VBLMultiplier);
		    break;
		
		case enum_dungeon:
		    dungeonName = key;
		    root = (char *)""; // Nasty hack. :o)
				   // In CSB dungeonName is added to root... and dungeonName is already a working  path
		    break;
		
		case enum_width:
		    sscanf(key,"%d", &WindowWidth);
		    break;
/*	not used with SDL
		case enum_x:
		    sscanf(key,"%d", &WindowX);
		    break;
		    break;
		case enum_y:
		    sscanf(key,"%d", &WindowY);
		    break;
		    break;
*/		
		case enum_extralarge:
		    screenSize = 4;
		    break;
		case enum_verylarge:
		    screenSize = 3;
		    break;
		case enum_large:
		    screenSize = 2;
		    break;
		case enum_small:
		    screenSize = 1;
		    break;
		
		case enum_play:
		    PlayfileName=key;
//		g_warning("pag %s",PlayfileName);
		    extraTicks = false;
		    PlaybackCommandOption = true;
		    break;
	}
	state=0;
    } // end of while
    
    return true;
  }


i64 UI_GetSystemTime(void)
{
    struct timeval t;
    gettimeofday( &t, NULL );
    return ((guint32) t.tv_usec)/1000 + ((guint64) t.tv_sec)*1000;
}	


i32 EditDialog::DoModal(void)
{
  return 0;
}


LISTING::LISTING(void)
{
  m_listing = (char *)malloc(1);
  *m_listing = 0;
}

void LISTING::Clear(void)
{
  free(m_listing);
  m_listing = NULL;
}

LISTING::~LISTING(void)
{
  Clear();
}

void LISTING::AddLine(const char *line)
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

void LISTING::DisplayList(const char * title)
{
	printf("[%s] %s",title, m_listing);
}


#if defined SDL12
void UI_SetDIBitsToDevice(
                           int dstX,
                           int dstY,
                           int width,
                           int height,
                           int,
                           int,
                           int,
                           int,
                           char *bitmap,
                           void *,
                           int
                         )
{
  static int firstTime = 0;
  int line;
  SDL_Surface *surf;
  SDL_Rect wholeScreen[1];
  ui8 *pixels;
  surf = SDL_GetVideoSurface();
/*
  if (firstTime < 1)
  {
    firstTime++;
    FILE *f;
    f = fopen("debug", "a");
    fprintf(f,"w=%d, h=%d, pitch=%d, bitsPerPixel=%d  Rmask=%08x\n",
              surf->w, surf->h,surf->pitch,
              surf->format->BitsPerPixel,surf->format->Rmask);
    fprintf(f,"dstX, dstY, width, height = %d,%d, %d, %d\n",
            dstX, dstY, width, height);
    fprintf(f,"size=%d, WindowWidth=%d\n", screenSize,WindowWidth);
    fclose(f);
  };
*/
  pixels = (ui8 *)surf->pixels;
  SDL_LockSurface(surf);
  for (line=0; line<height; line++)
  {
    int pitch;
    pitch = surf->pitch;
    memcpy(pixels+pitch*(line+dstY)+2*dstX, bitmap+2*WindowWidth*(line), width*2);
    //pixels[0] = 0;
    //pixels[1] = 255;
    //pixels[pitch*100+200] = 0;
    //pixels[pitch*100+201] = 255;
  };
  SDL_UnlockSurface(surf);
  SDL_UpdateRect(surf, dstX, dstY, width, height);
  return;
}
#elif defined SDL20

extern SDL_Renderer *sdlRenderer;
extern SDL_Texture *sdlTexture;

void UI_SetDIBitsToDevice(
                           int dstX,
                           int dstY,
                           int width,
                           int height,
                           int,
                           int,
                           int,
                           int,
                           char *bitmap,
                           void *,
                           int
                         )
{
  SDL_Rect textureRect;
  SDL_Rect rendererRect;
  void *dstPixels;
  int pitch;
  int line;
  if (sdlTexture == NULL) return;
  textureRect.x = 0;
  textureRect.y = 0;
  textureRect.w = width;
  textureRect.h = height;
  if (SDL_LockTexture(sdlTexture,
                      &textureRect,
                      &dstPixels,
                      &pitch))
  {
    UI_MessageBox(SDL_GetError(),
                  "UI_SetDIBitsToDevice",
                  MESSAGE_OK);
    die(0x53a9);
  };
              
  for (line=0; line<height; line++)
  {
    memcpy((ui8 *)dstPixels+pitch*line, 
           bitmap+2*320*(line), width*2);
  };
  SDL_UnlockTexture(sdlTexture);
  //SDL_RenderClear(sdlRenderer);
  rendererRect.x = dstX;
  rendererRect.y = dstY;
  rendererRect.w = width;
  rendererRect.h = height;
  SDL_RenderCopy(sdlRenderer,
                 sdlTexture,
                 &textureRect,
                 &rendererRect);
  SDL_RenderPresent(sdlRenderer);
  return;
}
#else
  xxxError
#endif

#endif //_LINUX
