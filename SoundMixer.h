//
#include "SDL.h"
#include "SDL_audio.h"
#include "SDL_version.h"

#define MIX_EFFECTSMAXSPEED  "MIX_EFFECTSMAXSPEED"
#define MIX_CHANNELS 8
#define MIX_CHANNEL_POST  -2
#define MIX_MAX_VOLUME		128	/* Volume of a chunk */
#define Mix_SetError	SDL_SetError
#define Mix_GetError	SDL_GetError

/* The internal format for an audio chunk */
struct Mix_Chunk {
	int allocated;
	Uint8 *abuf;
	Uint32 alen;
	Uint8 volume;		/* Per-sample volume, 0-128 */
};


/* The different fading types supported */
enum Mix_Fading 
{
  MIX_NO_FADING,
  MIX_FADING_OUT,
  MIX_FADING_IN
};

enum Mix_MusicType
{
	MUS_NONE,
	MUS_CMD,
	MUS_WAV,
	MUS_MOD,
	MUS_MID,
	MUS_OGG,
	MUS_MP3,
	MUS_MP3_MAD,
	MUS_FLAC,
	MUS_MODPLUG
};



#define Mix_PlayChannel(channel,chunk,loops) Mix_PlayChannelTimed(channel,chunk,loops,-1)
/* The same as above, but the sound is played at most 'ticks' milliseconds */
extern DECLSPEC int SDLCALL Mix_PlayChannelTimed(int channel, Mix_Chunk *chunk, int loops, int ticks);
typedef void (*Mix_EffectFunc_t)(int chan, void *stream, int len, void *udata);
typedef void (*Mix_EffectDone_t)(int chan, void *udata);
extern DECLSPEC int SDLCALL Mix_VolumeMusic(int volume);
int _Mix_UnregisterAllEffects_locked(int channel);
extern DECLSPEC void SDLCALL Mix_FreeChunk(Mix_Chunk *chunk);
extern DECLSPEC int SDLCALL Mix_UnregisterAllEffects(int channel);
extern DECLSPEC int SDLCALL Mix_HaltChannel(int channel);
void _Mix_DeinitEffects(void);
typedef struct _Mix_Music Mix_Music;
extern DECLSPEC int SDLCALL Mix_Volume(int channel, int volume);
extern DECLSPEC int SDLCALL Mix_Playing(int channel);
/* Load raw audio data of the mixer format from a memory buffer */
Mix_Chunk *Mix_QuickLoad_RAW(Uint8 *mem, Uint32 len);


int Mix_OpenAudio(int frequency, 
                  Uint16 format, 
                  int nchannels, 
                  int chunksize);
void Mix_CloseAudio(void);
static void mix_channels(void *udata, Uint8 *stream, int len);

