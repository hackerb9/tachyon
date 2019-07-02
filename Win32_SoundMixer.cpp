// DirectSound playback routines for DM/CSB Win32 by Ryan Hill
//
// The public interface is:
//
// bool Win32_SoundMixer_Init(HWND hWnd);
// void Win32_SoundMixer_Play(BYTE *pWave, DWORD dwBytes);
// void Win32_SoundMixer_Shutdown();
//
// Descriptions:
//
// - bool Win32_SoundMixer_Init(HWND hWnd);
// Initializes DirectSound and returns true if we're ready to play back sounds
//
// - void Win32_SoundMixer_Play(BYTE *pWave, DWORD dwBytes);
// Takes in a wave file just as it would be saved on disk or in a resource and plays it, mixing it with
// whatever sounds are already playing
//
// - void Win32_SoundMixer_Shutdown();
// Cleans up.  Call it if you're a clean programmer.
//
// Since nobody wants to get an IDirectSoundBuffer in return, and to have sounds just mix, we internally have a queue
// of the buffers that are in use to prevent them from leaking until we die.  The buffers get added to this queue
// while they play since if you Release() a buffer it instantly stops playing, so somebody has to own it until
// it's done playing.  Now, since we don't really care when it's done playing we just leave them in the buffer and
// the next time the queue wraps around we delete the old buffer that was in its spot.  No big deal and it's a nice
// and clean interface.
//
#include <windows.h>
#include <dsound.h>
#include <assert.h>


const int g_queueSize=64; // This is the maximum number of simultaneous sounds we can play at once.
                          // I artificially set this at 16, it can be anything, it's just limited by resources.

//
// DirectSound GUIDS and a COM helper class
//
#define INITGUID
#include <initguid.h>

DEFINE_GUID(CLSID_DirectSound, 0x47d4d946, 0x62e8, 0x11cf, 0x93, 0xbc, 0x44, 0x45, 0x53, 0x54, 0x0, 0x0);
DEFINE_GUID(IID_IDirectSound, 0x279AFA83, 0x4981, 0x11CE, 0xA5, 0x21, 0x00, 0x20, 0xAF, 0x0B, 0xE5, 0x60);

// Counting Pointers for automatic reference counting
template<class T>
class CntPtrTo
{
public:

   CntPtrTo() { m_pT=NULL; }
   CntPtrTo(CntPtrTo<T> &ptr) : m_pT(ptr.m_pT) { if(m_pT) m_pT->AddRef(); }
   CntPtrTo(T *pTNew) : m_pT(pTNew) { if(m_pT) m_pT->AddRef(); }
   ~CntPtrTo() { if(m_pT) {m_pT->Release(); m_pT=NULL;};}

   T **Address() { assert(m_pT==NULL); return &m_pT; }

   T* Const() const { return const_cast<T *>(m_pT); }

   T* operator ->() { assert(m_pT); return m_pT; }
   const T* operator ->() const { assert(m_pT); return m_pT; }

   operator T*() { return m_pT; }
   operator const T*() const { return m_pT; }

   void operator=(CntPtrTo<T> &ptr) { *this=ptr.m_pT; }

   void operator=(T *pTNew)
   {
      T *pT=m_pT; m_pT=pTNew; // To prevent release recursion problems
      if(pT) pT->Release();
      if(m_pT) m_pT->AddRef();
   }

private:
   T *m_pT;
};

//
// Global Variables (since we are providing a flat API, nobody wants to own us)
//
static CntPtrTo<IDirectSound> g_pDirectSound;

static CntPtrTo<IDirectSoundBuffer> g_queue[g_queueSize];
static int g_queuePos=0;

//
// Utility Functions
//
bool ParseWave(const BYTE *pSound, DWORD dwSize, WAVEFORMATEX* &pWaveHeader, BYTE* &pbWaveData, DWORD &cbWaveSize)
{
   DWORD *pdw = (DWORD *)pSound;

   if( *pdw++ != mmioFOURCC('R', 'I', 'F', 'F'))
      return false; // not even RIFF

   DWORD dwLength = *pdw++;

   if( *pdw++ != mmioFOURCC('W', 'A', 'V', 'E'))
      return false; // not a WAV

   DWORD *pdwEnd = (DWORD *)((BYTE *)pdw + dwLength-4);

   // dwSize and dwLength need to agree
   if(dwSize-8!=dwLength)
      return false;

   while(pdw < pdwEnd)
   {
      DWORD dwType = *pdw++;
      DWORD dwLength = *pdw++;

      switch(dwType)
      {
         case mmioFOURCC('f', 'm', 't', ' '):
            if(!pWaveHeader)
            {
               if(dwLength < sizeof(WAVEFORMAT))
                  return false; // not a WAV

               pWaveHeader = (WAVEFORMATEX *)pdw;

               if(pbWaveData && cbWaveSize)
                  return true;
            }
            break;

         case mmioFOURCC('d', 'a', 't', 'a'):
            if( !pbWaveData || !cbWaveSize )
            {
               pbWaveData = (LPBYTE)pdw;
               cbWaveSize = dwLength;

               if(pWaveHeader)
                  return true;
            }
            break;
      }

      pdw = (DWORD *)((BYTE *)pdw + ((dwLength+1)&~1));
   }

   return false;
}

void Win32_SoundMixer_CreateSoundFromWave(BYTE *pWave, DWORD dwBytes, CntPtrTo<IDirectSoundBuffer> &retval)
{
  HRESULT hr;
   WAVEFORMATEX format; // Filled out completely by the wave
   DSBUFFERDESC desc;
   desc.dwSize = sizeof DSBUFFERDESC;
   desc.dwFlags = DSBCAPS_CTRLDEFAULT;
   desc.dwBufferBytes = 0; // To be filled in after parsing the wave
   desc.lpwfxFormat = &format;
//   desc.guid3DAlgorithm = GUID_NULL;
   desc.dwReserved = 0;

   WAVEFORMATEX *pFormat=NULL;
   BYTE *pData=NULL;
   DWORD dwDataSize=0;

   if(!ParseWave(pWave, dwBytes, pFormat, pData, dwDataSize))
      return; // Couldn't make sense of the data

   format = *pFormat;
   desc.dwBufferBytes = dwDataSize;

   // Create the DirectSound buffer
   CntPtrTo<IDirectSoundBuffer> pDirectSoundBuffer;

   if(FAILED(g_pDirectSound->CreateSoundBuffer(&desc, pDirectSoundBuffer.Address(), NULL)))
   {
      return;
   };

   // Copy the bytes across
   {
//===============================================
//      void *p1=NULL;
//      DWORD bytes1=0;
//
//      if(FAILED(hr = pDirectSoundBuffer->Lock(0, 0, &p1, &bytes1, NULL, NULL, DSBLOCK_ENTIREBUFFER)))
//         return;

     
      void *p1=NULL; 
      DWORD bytes1=0; 
      void *p2=NULL; 
      DWORD bytes2=0; 
      if(FAILED(hr=pDirectSoundBuffer->Lock(0, dwDataSize, &p1, &bytes1, &p2, &bytes2, DSBLOCK_ENTIREBUFFER))) 
         return; 

      if(bytes1!=dwDataSize) 
         return;
//======================================================         

      CopyMemory( p1, pData, dwDataSize);
      if(FAILED(pDirectSoundBuffer->Unlock(p1, bytes1, NULL, 0)))
         return;
   }

   retval=pDirectSoundBuffer;
}

//
// Public Interface
//

bool Win32_SoundMixer_Init(HWND hWnd)
{
   if(FAILED(CoInitialize(NULL)))
      return false;

   CntPtrTo<IDirectSound> pDirectSound;

   if(FAILED(CoCreateInstance(CLSID_DirectSound, NULL, CLSCTX_INPROC_SERVER, IID_IDirectSound, (void **)pDirectSound.Address())))
      return false;

   if(FAILED(pDirectSound->Initialize(NULL)))
      return false;

   // Here we could go exclusive and set the primary buffer format to the sound format, but I don't know the format.
   // Since people could potentially add high quality sounds, this is just fine.  It might produce a slight quality loss
   // but we need to try it to find out.
   if(FAILED(pDirectSound->SetCooperativeLevel(hWnd, DSSCL_NORMAL)))
      return false;

   g_pDirectSound=pDirectSound;
   return true;
}

void Win32_SoundMixer_Play(BYTE *pWave, DWORD dwBytes)
{
   // Parse the wave file data
   CntPtrTo<IDirectSoundBuffer> pDirectSoundBuffer;
   Win32_SoundMixer_CreateSoundFromWave( pWave, dwBytes, pDirectSoundBuffer );

   if(!pDirectSoundBuffer)
   {
      return; // We failed to create the sound, so just return and do nothing
   };
   if (pDirectSoundBuffer->Play(0, 0, 0) != DS_OK)
   {
   };

   g_queue[g_queuePos]=pDirectSoundBuffer;
   if(++g_queuePos==g_queueSize)
      g_queuePos=0;
}

void Win32_SoundMixer_Shutdown()
{
   if(!g_pDirectSound)
      return;

   for(int i=0;i<g_queueSize;i++)
      g_queue[i]=NULL;

   g_pDirectSound=NULL;
   CoUninitialize();
}
