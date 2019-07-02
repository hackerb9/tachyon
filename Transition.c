#include "Transition.h"
#include <stdio.h>
/*#include <OSUtils.h>
#include <Debugging.h>
#include <DateTimeUtils.h>
#include <LowMem.h>
#include <TextUtils.h>
#include <DrawSprocket.h>
#include <Sound.h>
//#include "CSB.h"
*/

#if TARGET_RT_MAC_MACH_O
	#include <DrawSprocket/DrawSprocket.h>
#else
	#include <DrawSprocket.h>
	#include <DriverServices.h>
#endif
extern long screenSize;
void WndProc(void*, unsigned short message, unsigned short wParam, unsigned long lParam);

#pragma pack(1)

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
  i16 cbSize;             //    40
  i8  byte38[4];          // "fact"
  i32 int42;              //     4
  i32 numBytes46;         // 
  i8  byte50[4];          // "data"
  i32 numSamples54;       //
  i8  sample58[1];
};

extern bool interrupt;
extern bool gScreenIsInvalid;


void Sleep(i32 msecs)
{
	unsigned long foo;
	msecs /= 10;
	Delay(msecs,&foo);
}

extern SndChannelPtr	gSoundChannel;

FILE* UI_fopen(char* name, const char* mode)
{
	char	f[512];
	FILE*	handle = NULL;
	
	#if TARGET_RT_MAC_MACH_O
		CFBundleRef	ref = CFBundleGetMainBundle();
		CFURLRef	resources = NULL;
		CFStringRef	str = NULL;
		FSRef		fsref;
		
		if(ref)
			resources = CFBundleCopyBundleURL(ref);
		
		if(resources)
		{
			if(CFURLGetFSRef(resources, &fsref))
			{
				FSRefMakePath(&fsref, (UInt8*)f, 400);
				strcat(f, "/Contents/Resources/");
				strcat(f, name);
				/*if(strchr(mode,'w') || strchr(mode,'+'))
				{
					handle = fopen(f, mode);
					fcntl(fileno(handle), )
				}
				else*/
					handle = fopen(f, mode);
			}
		}
		
		if(resources)
			CFRelease(resources);
	#else
	
		handle = fopen(f, mode);
	
	#endif
	return handle;
}

short MessageBox(long par1, const char* text, const char* par2, long mbtype)
{
	#pragma unused(par1, par2, mbtype)
	static StringPtr yes = "\pYes";
	static StringPtr no = "\pNo";
	static StringPtr okay = "\pOK";
	AlertStdAlertParamRec	a;
	SInt16	item;
	Str255 title, explanation;

	MacShowCursor();
	
	title[0] = 0;
	explanation[0] = 0;
	
	if(title) CopyCStringToPascal(text, title);
	if(explanation) CopyCStringToPascal(par2, explanation);

	
	memset(&a, 0, sizeof(a));
	
	a.movable = 1;
	
	if(mbtype == MB_YESNO)
	{
		a.defaultText = yes;
		a.cancelText = no;
		a.defaultButton = 1;
		a.cancelButton = 2;
	}
	else
	{
		a.defaultText = okay;
		a.defaultButton = 1;
		a.cancelButton = 1;
	}
		
	(void) StandardAlert(kAlertNoteAlert, title, explanation, &a, &item);
	
	HideCursor();
	return item;
}

short 	UI_MessageBox(char* text, char* par2, long mbtype)
{
	return MessageBox(0,text,par2,mbtype);		
}


/*
void XHideCursor()
{
	HideCursor();
}

void XShowCursor(ui8 flag)
{
 	#pragma unused(flag)
	ShowCursor();
}
*/



void GetCursorPos(POINT* point)
{
	Point pt;
	//GetMouse(&pt);
	DSpGetMouse(&pt);
	point -> x = pt.h;
	point -> y = pt.v-40;
}

void ScreenToClient(WindowPtr window, POINT* point)
{
	Point pt;
	SetPt(&pt,point->x,point->y);
	SetPortWindowPort(window);
	GlobalToLocal(&pt);
	point->x = pt.h;
	point->y = pt.v;
}



void GetSystemTime(SYSTEMTIME *s)
{
	unsigned long secs;
	DateTimeRec x;
	
	GetDateTime(&secs);
	SecondsToDate(secs,&x);

	s -> wMinute 		= x . minute;
	s -> wSecond 		= x . second;
	s -> wHour   		= x . hour;
	s -> wMilliseconds  = 0;
	s -> wMonth			= x . month;
	s -> wYear			= x . year;
	s -> wDay			= x . day;
}


i64 UI_GetSystemTime(void)
{
	
  // Cumulative milliseconds since 1980 (or whenever..not important)
  /*SYSTEMTIME st;
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
  UnsignedWide w = AbsoluteToNanoseconds(UpTime());
  i64 result = ((((unsigned long long)w.hi)<<32ULL) | (w.lo)) / 1000000;
  return result;
/*	unsigned long secs;
	unsigned long long result;
	GetDateTime(&secs);
	
	result = secs;
	result *= 1000;
	return result;
*/
}


void UI_GetCursorPos(i32 *x, i32 *y)
{
  Point pt;
  DSpGetMouse(&pt);
  *x = pt.h/screenSize;
  *y = (pt.v-40)/screenSize;
}


#pragma mark -
static void	SNDHEADToSoundHeader(SNDHEAD* in, SoundHeader* out)
{
	out -> samplePtr 		= (Ptr)&in -> sample58[0];
	out -> length 			= in -> Size;
	out -> sampleRate		= (in -> nSamplesPerSecond) << 16L;//rate11025hz;
	out -> loopStart		= 0;
	out -> loopEnd			= NULL;
	out -> encode 			= stdSH;
	out -> baseFrequency	= 20;
	out -> sampleArea[0]	= 0;
}

void sndPlaySound(char *wave, long flags)
{
	SNDHEAD 	*soundData 	= (SNDHEAD*)wave;
	Boolean		isSync 		= (flags & SND_SYNC) == SND_SYNC;
	Boolean 	isMemory 	= (flags & SND_MEMORY) == SND_MEMORY;
	Boolean		isAsync  	= (flags & SND_ASYNC) == SND_ASYNC;
	OSErr		err 		= noErr;
	SndCommand	cmd;
	SoundHeader	header;
	
	if(!soundData)
	{
		cmd.cmd 	= quietCmd;
		cmd.param1 	= 0;
		cmd.param2	= 0L;
		err = SndDoImmediate(gSoundChannel,&cmd);
	}
	else
	{
		SNDHEADToSoundHeader(soundData,&header);

		//carbonsound/sound.h
		cmd.cmd 	= bufferCmd;
		cmd.param1 	= 0;
		cmd.param2 	= (unsigned long)&header;
		 
		err = SndDoImmediate(gSoundChannel,&cmd);//,false);

		if(isSync)
		{
			SCStatus	status;
			status.scChannelBusy = 1;
			
			while(status.scChannelBusy)
			{
				//  wait for completion
				err = SndChannelStatus(gSoundChannel, sizeof(status), &status);
				
				if(err) break;
			
				if(!status.scChannelBusy) break;
			}
			
		}
	}
}


void InvalidateRect(HWND w, i32, i32)
{
	gScreenIsInvalid = 1;

	//  calling WndProc with WM_PAINT message would
	//  be a fatal error here hence CSBUI is not reentrant 
}

#define MAX_MALLOCS	1000
#define TAG_SIZE	128

typedef struct mallocitem {
	unsigned char*	fPointer;
	char			fFile[54];
	int				fLine;	
	int				fSize;
} mallocitem;

mallocitem	gMallocItems[MAX_MALLOCS] = {0,};
int			gNumMallocItems = 0;



void* ax_malloc(Size size, const char* file, int line)
{
    
	/*unsigned char* 	block;
	int		i;
	
	if(interrupt)
		Debugger();
	
	if(gNumMallocItems >= MAX_MALLOCS) DebugStr("\pMALLOC ERROR!");
	file = file; line=line;
	block = (unsigned char*) _malloc(size+TAG_SIZE);

	if(block)
	{
		for(i=0; i<(size+TAG_SIZE); i++)
			block[i]=0;
		for(i=size; i<(size+TAG_SIZE); i++)
			block[i]=0xf1;

		if(size == 1020) Debugger();
		strcpy(gMallocItems[gNumMallocItems].fFile, file);
		gMallocItems[gNumMallocItems].fLine = line;
		gMallocItems[gNumMallocItems].fPointer = block;
		gMallocItems[gNumMallocItems].fSize = size;
		gNumMallocItems ++;
	}
	return block;*/
	
	return NewPtr(size+64);
}

void ax_free(void* block, const char* file, int line)
{
	/*int	i,j;
	
	file = file; line=line;
	
	for(i=0; i<gNumMallocItems; i++)
	{
		for(j=gMallocItems[i].fSize; j<(gMallocItems[i].fSize+TAG_SIZE); j++)
		{
			if( (gMallocItems[i].fPointer)[j] != 0xf1 )
			{
				//DebugStr("\pBlock check failed");
			}
		}
	}
	
	for(i=0; i<gNumMallocItems; i++)
	{
		if(gMallocItems[i].fPointer == block)
		{
			memmove(&gMallocItems[i],&gMallocItems[i+1],(MAX_MALLOCS-i-1)*sizeof(mallocitem));
			memset(&gMallocItems[MAX_MALLOCS-1],0,sizeof(mallocitem));
			gNumMallocItems--;
			return _free(block);
		}	
			
	}
	
	if(i==gNumMallocItems)
	{
		DebugStr("\pHEY! THIS BLOCK WAS NOT ALLOCATED");
	}
	*/
	//return _free(block);
	DisposePtr((Ptr)block);
}

