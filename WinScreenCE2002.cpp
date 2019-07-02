
#include "stdafx.h"

#ifdef _MSVC_CE2002ARM // Pocket PC video handling


#include "UI.h"

#include <stdio.h>

//#include "Objects.h"
#include "Dispatch.h"
#include "CSB.h"
#include "data.h"
void info(char *, unsigned int);
char *getfield(char *buf, i32& col);
bool CheckData(char *buf, i32& col, i32&data, i32 min, i32 max);

extern i32 WindowX;
extern i32 WindowY;
extern i32 VBLMultiplier;
extern i16 globalPalette[16];
extern bool fullscreenRequested;
//extern bool fullscreenActive;
extern bool overlayActive;
//extern bool overlappingText;
//extern CDC *OnDrawDC;
ui32 STBLTCount=0;

struct MAPPING
{
  short srcx,  srcy,
        width, height,
        dstx,  dsty,
        copymode,
        palette,
        overlay; //Should overlays be applied?
  unsigned char id;  //Then name of this window
  unsigned char z_order;
  unsigned int checksum;
  unsigned int firstGroupOffset;
      //The i16 index of the first group.
  unsigned int preMask, numWholeGroup, postMask;
      //These are the masks used when computing hashcodes (or checksums).
      //If the first pixel is on a 16-bit boundary then preMask will be 0.
      //Likewise, if the last pixel is at the end of a group, postMask will be 0.
      // AND if premask contains all the bits then postMask will be zero.
      //numWholeGroup is the number of groups of 16 pixels.
      //
      // preMask and postMask are 32 bits so that two 'planes' can be
      // processed simultaneously.
  void CreateChecksumMasks(void);
};



void MAPPING::CreateChecksumMasks()
{
  // Fill in the firstGoupOffset,
  //             preMask, post Mask,
  //             numWholeGroup
  //
  int numBits, firstGroupOnLine, firstBitInFirstGroup;
  firstGroupOffset = srcy*320/16*4; // Four groups of i16 per 16 bits.
  firstGroupOnLine = srcx/16*4;
  firstGroupOffset += firstGroupOnLine;
  firstBitInFirstGroup = srcx%16;
  preMask = 0;
  postMask = 0;
  numWholeGroup = 0;
  numBits = width;
  if (firstBitInFirstGroup != 0)
  {
    preMask = 0xffff >> firstBitInFirstGroup;
    preMask = ((preMask>>8) | (preMask<<8)) & 0xffff;
    preMask |= preMask <<16;
    numBits -= 16 - firstBitInFirstGroup;
  };
  if (numBits > 0)
  { // There are more bits than will fit in preMask.
    numWholeGroup = numBits/16;
    numBits -= 16*numWholeGroup;
  };
  if (numBits != 0)
  { // Compute postMask.
    postMask = (0xffff0000 >> ((numBits+16)&15)) & 0xffff;
    postMask = (postMask<<8) | (postMask>>8);
    postMask |= postMask <<16;
  };
  if (numBits < 0)
  {
    preMask &= postMask;
    postMask = 0;
  };
}

char *(mapnames[]) = 
{
  "HFULL_VCENTER",
  "HCENTER_VFULL",
  "PRISON_DOOR",
  "PORTRAIT_A",
  "PORTRAIT_B",
  "PORTRAIT_C",
  "PORTRAIT_D",
  "VIEWPORT",
  "SPELL_CONTROLS",
  "ATTACK_CONTROLS",
  "TURN_LEFT",
  "MOVE_FORWARD",
  "TURN_RIGHT",
  "MOVE_BACKWARD",
  "SLIDE_LEFT",
  "SLIDE_RIGHT",
  "SCROLLING_TEXT",
  "MARCHING_ORDER",
  "TIME",
//  "OVERLAPPEDTEXT",
  "END"
};

enum MAPID
{
  WMAP_HFULL_VCENTER = 0,
  WMAP_HCENTER_VFULL,
  WMAP_PRISON_DOOR,
  WMAP_PORTRAIT_A,
  WMAP_PORTRAIT_B,
  WMAP_PORTRAIT_C,
  WMAP_PORTRAIT_D,
  WMAP_VIEWPORT,
  WMAP_SPELL_CONTROLS,
  WMAP_ATTACK_CONTROLS,
  WMAP_TURN_LEFT,
  WMAP_MOVE_FORWARD,
  WMAP_TURN_RIGHT,
  WMAP_MOVE_BACKWARD,
  WMAP_SLIDE_LEFT,
  WMAP_SLIDE_RIGHT,
  WMAP_SCROLLING_TEXT,
  WMAP_MARCHING_ORDER,
  WMAP_TIME,
//  WMAP_OVERLAPPEDTEXT,
  WMAP_END
};

enum Z_ORDER
{
  //Most of these make no difference whatsoever because
  //the windows do not overlap.
  //But the scrolling text is an exception.
  //Tapping on the text moves it from the front to the back
  //and vise-versa.
  ZO_HFULL_VCENTER = 0,
  ZO_HCENTER_VFULL,
  ZO_PRISON_DOOR,
  ZO_PORTRAIT_D,
  ZO_PORTRAIT_C,
  ZO_PORTRAIT_B,
  ZO_PORTRAIT_A,
  ZO_VIEWPORT,
  ZO_SPELL_CONTROLS,
  ZO_ATTACK_CONTROLS,
  ZO_TURN_LEFT,
  ZO_MOVE_FORWARD,
  ZO_TURN_RIGHT,
  ZO_MOVE_BACKWARD,
  ZO_SLIDE_LEFT,
  ZO_SLIDE_RIGHT,
  ZO_SCROLLING_TEXT,
  ZO_MARCHING_ORDER,
  ZO_TIME
};

MAPPING screenMap[] =
{                       
  {                     //0 = Full screen centered
    0,  0, 240,200,     //Source
    0,  0,              //Destination
    0,                  //Copy Mode
    0,                  //Palette
    false,              //Overlays
    WMAP_HFULL_VCENTER, //Id
    ZO_HFULL_VCENTER    //z-order  
  }, 

                      
  {                       //1 = Centered horizontal, full vertical
    40,   0, 240, 200,    //Source
     0,   0,              //Destination
     0,                   //Copy mode
     0,                   //Palette
     false,               //Overlays
     WMAP_HCENTER_VFULL,  //Id
     ZO_HCENTER_VFULL     //z-order 
  },   

                      
  {                     // 2 = Shifted Left, full vertical.  Prison door.
    80,   0, 240, 200,  //Source
    0,    0,            //Destination
    0,                  //Copy mode
    0,                  //Palette
    false,              //Overlays
    WMAP_PRISON_DOOR,   //Id
    ZO_PRISON_DOOR      //z-order
  },
                      
  {                   // 3 = Portrait A
    0,  0, 64, 29,    //Source
    0,  0,            //Destination
    1,                //Copy mode
    0,                //Palette
    false,            //Overlays
    WMAP_PORTRAIT_A,  //Id
    ZO_PORTRAIT_A     //z-order
  },
                      
  {                   // 4 = Portrait B
    69,  0, 64, 29,   //Source
    60,  0,           //Destination
    1,                //Copy mode
    0,                //Palette
    false,            //Overlays
    WMAP_PORTRAIT_B,  //Id
    ZO_PORTRAIT_B     //z-order
  },
                      
  {                   // 5 = Portrait C
    138,  0, 64, 29,  //Source
    120,  0,          //Destination
    1,                //Copy mode
    0,                //Palette
    false,            //Overlays
    WMAP_PORTRAIT_C,  //Id
    ZO_PORTRAIT_C     //z-order
  },
                      
  {                   // 6 = Portrait D
    207,  0, 64, 29,  //Source
    180,  0,          //Destination
    1,                //Copy mode
    0,                //Palette
    false,            //Overlays
    WMAP_PORTRAIT_D,  //Id
    ZO_PORTRAIT_D     //z-order
  },
                     
  {                   // 7 = Viewport
    0, 33, 224, 136,  //Source
    8, 29,            //Destination
    0,                //Copy mode
    1,                //Palette
    true,             //Overlays
    WMAP_VIEWPORT,    //Id
    ZO_VIEWPORT       //z-order
  },
                     
  {                       // 8 = Spell Controls
    233, 32, 87, 43,      //Source
    0,  169,              //Destination
    0,                    //Copy mode
    1,                    //Palette
    false,                //Overlays
    WMAP_SPELL_CONTROLS,  //Id
    ZO_SPELL_CONTROLS     //z-order
  },
                     
  {                       // 9 = Attack Controls
    233,  77, 87, 45,     //Source
    102, 167,             //Destination
    0,                    //Copy mode
    0,                    //Palette
    false,                //Overlays
    WMAP_ATTACK_CONTROLS, //Id
    ZO_ATTACK_CONTROLS    //z-order
  },
                      
  {                   // 10 = Turn Left
    233, 124, 29, 22, //Source
    153, 212,         //Destination
    0,                //Copy mode
    0,                //Palette
    false,            //Overlays
    WMAP_TURN_LEFT,   //Id
    ZO_TURN_LEFT      //z-order
  },
                      
  {                     // 11 = Move Forward
    262, 124, 28, 22,   //Source
    182, 212,           //Destination
    0,                  //Copy mode
    0,                  //Palette
    false,              //Overlays
    WMAP_MOVE_FORWARD,  //Id
    ZO_MOVE_FORWARD     //z-order
  },
                     
  {                   // 12 = Turn Right
    290, 124, 30, 22, //Source
    210, 212,         //Destination
    0,                //Copy mode
    0,                //Palette
    false,            //Overlays
    WMAP_TURN_RIGHT,  //Id
    ZO_TURN_RIGHT     //z-order
  },
                      
  {                   // 13 = Slide Left
    233, 146, 29, 23, //Source
    153, 234,         //Destination
    0,                //Copy mode
    0,                //Palette
    false,            //Overlays
    WMAP_SLIDE_LEFT,  //Id
    ZO_SLIDE_LEFT     //z-order
  },
                  
  {                     // 14 = Move Backward
    262, 146, 28, 23,   //Source
    182, 234,           //Destination
    0,                  //Copy mode
    0,                  //Palette
    false,              //Overlays
    WMAP_MOVE_BACKWARD, //Id
    ZO_MOVE_BACKWARD    //z-order
  },
                     
  {                     // 15 = Slide Right
    290, 146, 30, 23,   //Source
    210, 234,           //Destination
    0,                  //Copy mode
    0,                  //Palette
    false,              //Overlays
    WMAP_SLIDE_RIGHT,   //Id
    ZO_SLIDE_RIGHT      //z-order
  },
                     
  {                       // 16 = Scrolling Text
    0, 169, 153, 30,      //Source
    0, 212,               //Destination
    0,                    //Copy mode
    0,                    //Palette
    false,                //Overlays
    WMAP_SCROLLING_TEXT,  //Id
    ZO_SCROLLING_TEXT     //z-order
  },
                     
//  {                       // 17 = Overlapped Text
//    0, 169, 240, 30,      //Source
//    0, 180,               //Destination
//    0,                    //Copy mode
//    0,                    //Palette
//    false,                //Overlays
//    WMAP_OVERLAPPEDTEXT,  //Id
//    ZO_SCROLLING_TEXT     //z-order
//  },
                      
  {                       // 18 = Marching Order
    275,   0, 45, 33,     //Source
    195, 169,             //Destination
    1,                    //Copy mode
    1,                    //Palette
    false,                //Overlays
    WMAP_MARCHING_ORDER,  //Id
    ZO_MARCHING_ORDER     //z-order
  } ,

  {                       // 19 = Time
    0,   0,  0,  0,       //Source
    8,  212,               //Destination
    0,                    //Copy mode
    0,                    //Palette
    false,                //Overlays
    WMAP_TIME,            //Id
    ZO_TIME               //z-order
  } ,

  {                // End of Array
    0, 0, 0, 0,    //Source
    0, 0,          //Destination
    0,             //Copy mode
    0,             //Palette
    false,         //Overlays
    WMAP_END,      //Id
    0              //z-order
  } 
};


i32 ScrollingTextWidth(void)
{
  i32 i;
  for (i=0; screenMap[i].id != WMAP_END; i++)
  {
    if (strcmp(mapnames[i],"SCROLLING_TEXT") == 0)
    {
      //return 240;
      return screenMap[i].width;
    };
  };
  return 240;
}

void GetTimeLocation(i32& x, i32& y)
{
  i32 i;
  for (i=0; screenMap[i].id != WMAP_END; i++)
  {
    if (screenMap[i].id == WMAP_TIME)
    {
      x = screenMap[i].dstx;
      y = screenMap[i].dsty;
      break;
    };
  };
}


bool ReadCEdisplayData(char *buf, i32& col)
{
  i32 k, n, dstx, dsty, srcx, srcy, width, height;
  i32 copymode, palette, overlay, zorder;
  char *field;
  field = getfield(buf,col);
  for (k=0; strcmp(mapnames[k], "END") != 0; k++)
  {
    if (strcmp(mapnames[k], field) == 0)
    {
      break; 
    };
  };
  if (   (strcmp(mapnames[k], "END") != 0)
      &&  CheckData(buf,col,srcx,0,319)
      &&  CheckData(buf,col,srcy,0,199)
      &&  CheckData(buf,col,dstx,0,4096)
      &&  CheckData(buf,col,dsty,0,4096)
      &&  CheckData(buf,col,width,0,320)
      &&  CheckData(buf,col,height,0,200)
      &&  CheckData(buf,col,copymode,0,1)
      &&  CheckData(buf,col,palette,0,1)
      &&  CheckData(buf,col,overlay,0,1)
      &&  CheckData(buf,col,zorder,0,17)
     )
  {
    for (n=0; screenMap[k].id != WMAP_END; n++)
    {
      if (screenMap[n].id == k) break;
    };
    screenMap[n].srcx = srcx;
    screenMap[n].srcy = srcy;
    screenMap[n].dstx = dstx;
    screenMap[n].dsty = dsty;
    screenMap[n].width = width;
    screenMap[n].height = height;
    screenMap[n].copymode = copymode;
    screenMap[n].palette = palette;
    screenMap[n].overlay = overlay;
    screenMap[n].z_order = zorder;
    return true;
  };
  return false;
}




//int screenMapStart = 0;  // index of first map
//int screenMapNum = 1;    // # of maps
unsigned char windowList[99] = {(unsigned char)WMAP_END};


void MapCoordinate(i32 a, i32 b, i32& x, i32& y)
{//Translate physical pixel units on actual screen.
 // to pixel units on Atari screen
  i32 area, windowIndex;
  for (windowIndex=0;
       screenMap[windowList[windowIndex]].id!=WMAP_END;
       windowIndex++)
  {
    //Locate topmost window.
  };
  for (windowIndex--; windowIndex >= 0; windowIndex--)
  {
    area = windowList[windowIndex];
    //area = screenMapStart + i;
    if (a < screenMap[area].dstx) continue;
    if (a >= screenMap[area].dstx + screenMap[area].width) continue;
    if (b < screenMap[area].dsty)    continue;
    if (b >= screenMap[area].dsty + screenMap[area].height) continue;
    x = a - screenMap[area].dstx  + screenMap[area].srcx;
    y = b - screenMap[area].dsty  + screenMap[area].srcy;
    return;
  };
  x = 0;
  y = 200;
  return;
}

bool IsPortraitBar(int x, int y)
{
  int i;
  for (i=3; i<=6; i++)
  {
    if (   (x >= screenMap[i].srcx + 60 - 20)
        && (x <  screenMap[i].srcx + 60)
        && (y >= screenMap[i].srcy)
        && (y <  screenMap[i].srcy + screenMap[i].height)
       )
    {
      return true;
    };
  };
  return false;
}

// Function to see if user tapped in text area of screen.
bool IsTextScrollArea(int x, int y)
{
  MAPPING *pMapping;
  i32 windowIndex;
  for (windowIndex=0;
       pMapping = &screenMap[windowList[windowIndex]], pMapping->id!=WMAP_END;
       windowIndex++)
  {
    //Locate topmost window.
  };
  for (windowIndex--;
       windowIndex >= 0;
       windowIndex--)
  {
    pMapping = &screenMap[windowList[windowIndex]];
    if (x < pMapping->srcx) continue;
    if (y < pMapping->srcy) continue;
    if (x >= pMapping->srcx + pMapping->width) continue;
    if (y >= pMapping->srcy + pMapping->height) continue;
    return    (pMapping->id == WMAP_SCROLLING_TEXT) ;
      //     || (pMapping->id == WMAP_OVERLAPPEDTEXT);
  };
  return false;
}


void SortWindowListBy_Z_order(void)
{
  int i, j;
  for (i=0; screenMap[windowList[i+1]].id!=WMAP_END; i++)
  {
    for (j=i; j>=0; j--)
    {
      ui8 temp;
      if (screenMap[windowList[j]].z_order >= screenMap[windowList[j+1]].z_order)
      {
        break;
      };
      temp = windowList[j];
      windowList[j] = windowList[j+1];
      windowList[j+1] = temp;
    };
  };
}



void SwapTextZOrder(void)
{
  //Move Text scrolling area to/from front/back.
  i32 mapIndex;
  i32 textZOrder = 0;
  i32 maxZOrder = -1;
  MAPPING *pMapping;
  for (mapIndex=0;
       pMapping=&screenMap[mapIndex], pMapping->id!=WMAP_END;
       mapIndex++)
  {
    if (pMapping->z_order > maxZOrder) maxZOrder = pMapping->z_order;
    if (   (pMapping->id == WMAP_SCROLLING_TEXT) )
       //  ||(pMapping->id == WMAP_OVERLAPPEDTEXT))
    {
      textZOrder = pMapping->z_order;
    };
  };
  for (mapIndex=0;
       pMapping=&screenMap[mapIndex], pMapping->id!=WMAP_END;
       mapIndex++)
  {
    if (   (pMapping->id == WMAP_SCROLLING_TEXT) )
        // ||(pMapping->id == WMAP_OVERLAPPEDTEXT))
    {
      if (textZOrder == 0)
      {
        pMapping->z_order = maxZOrder;
      }
      else
      {
        pMapping->z_order = 0;
      }
    }
    else
    {
      if (textZOrder == 0)
      {
        pMapping->z_order--;
      }
      else
      {
        pMapping->z_order++;
      };
    };
  };
  SortWindowListBy_Z_order();
  ForceScreenDraw();
}


i32 screenSize=1;
i16 palette16[16];
i16 counter;
i16 bitSkip;
i32 dstLineLen;
BITMAPINFO bitmapInfo;
i16 palette[2][16];
i16 oldPalette[2][16];
i16 oldVideoMode;
pnt logbase(void);
unsigned int nibbles[40*320];
ui8 noOverlay[320]; //All zeros for one line of overlay

bool HasPaletteChanged(i16 *palette, i16 *oldpalette)
{
  bool change=false;
  for (i32 i=0; i<16; i++)
  {
    if (oldpalette[i] != palette[i])
    {
      oldpalette[i]=palette[i];
      change=true;
    };
  };
  return change;
}


void createPalette16(i16 *palette, VIDEOCE *)
{ // Create 16-bit color palette from the ST 9-bit palette
  for (i32 i=0; i<16; i++)
  {
    ASSERT(palette[i]<=0x777,"illegal palette");
    palette16[i]=0;
    palette16[i] |= (((palette[i]>>8) & 7)*0x1f/0x7)<<10;//red
    palette16[i] |= (((palette[i]>>4) & 7)*0x1f/0x7)<< 5;//green
    palette16[i] |= (((palette[i]>>0) & 7)*0x1f/0x7)<< 0;//blue
  };
}


bool ForcedScreenDraw = false;

void ForceScreenDraw(void)
{
  ForcedScreenDraw = true;
}

unsigned int bit0Table[256];
unsigned int bit1Table[256];
unsigned int bit2Table[256];
unsigned int bit3Table[256];
bool bitTablesInitialized = false;

void BuildBitTables(void)
{
  int i, j;
  unsigned int n;
  for (i=0; i<256; i++)
  {
    bit0Table[i] = bit1Table[i] = bit2Table[i] = bit3Table[i] = 0;
    n = i;
    for (j=28; j>=0; j-=4)
    {
      if (n & 1)
      {
        bit0Table[i] |= 1<<j;
        bit1Table[i] |= 2<<j;
        bit2Table[i] |= 4<<j;
        bit3Table[i] |= 8<<j;
      };
      n >>= 1;
    };
  };
  for (i=0; i< sizeof (screenMap)/sizeof(screenMap[0]); i++)
  {
    screenMap[i].CreateChecksumMasks();
  };
  bitTablesInitialized = true;
}

unsigned int DePlane(unsigned char *src, int count, int shift, int numLines)
{
  // src points to a group of 8 bytes representing 16 pixels
  //  src is the 4-bitplane Atari format.
  // The destination is the nibbles array of unsigned int.
  // 8 pixels will be placed in the form of 32-bit integers.
  //  The first pixel is in the bottom four bits of the first integer
  //  and the eighth pixel is in the top four bits of the first integer.
  //  The ninth is in the bottom four bits of the second integer, and so on.
  // count is the number of pixels to be stored.
  // shift is the number of pixels to skip in the first 16-pixel group.
  // Repeat for numlines.  For each line src+=160, dst+=40
  // Result is the hashcode of the entire bitmap.
  int i, sDelta, sd, s1, s2, numGroup;
  unsigned char *s;
  unsigned int *d, *dst;
  unsigned int hashCode = 0, n, m, mask;
  static unsigned int masks[8] =
  {
    0xffffffff,
    0x0000000f,
    0x000000ff,
    0x00000fff,
    0x0000ffff,
    0x000fffff,
    0x00ffffff,
    0x0fffffff,
  };
  numGroup = (count + 7) >> 3;
  dst = nibbles;
  if (shift < 8)
  {
    sDelta = 1;
  }
  else
  {
    src++;
    sDelta = 7;
  };
  mask = masks[count & 7];
  if ((shift & 7) == 0)
  {
    for (; numLines>0; numLines--,src+=160,dst+=40)
    {
      for (i=0,s=src,d=dst,sd=sDelta; i<numGroup; s+=sd,sd^=6,i++,d++)
      {
        n =  bit0Table[s[0]]
           | bit1Table[s[2]]
           | bit2Table[s[4]]
           | bit3Table[s[6]];
        *d = n;
        if (hashCode & 0x80000000)
        {
          hashCode <<= 1;
          hashCode ^= 0x8d;
        }
        else
        {
          hashCode <<= 1;
        }
        hashCode ^= n;
      };
      d--;
      hashCode ^= *d;
      *d &= mask;
      hashCode ^= *d;
    };
  }
  else
  {
    s1 = 4 * (shift & 7);
    s2 = 32 - s1;
    for (; numLines>0; numLines--,src+=160,dst+=40)
    {
      m =  bit0Table[src[0]]
         | bit1Table[src[2]]
         | bit2Table[src[4]]
         | bit3Table[src[6]];
      for (i=0,s=src,d=dst,sd=sDelta; s+=sd,sd^=6,i<numGroup; i++,d++)
      {
        n =  bit0Table[s[0]]
           | bit1Table[s[2]]
           | bit2Table[s[4]]
           | bit3Table[s[6]];
        *d = (m>>s1) | (n<<s2);
        if (hashCode & 0x80000000)
        {
          hashCode <<= 1;
          hashCode ^= 0x8d;
        }
        else
        {
          hashCode <<= 1;
        }
        hashCode ^= *d;
        m = n;
      };
      d--;
      hashCode ^= *d;
      *d &= mask;
      hashCode ^= *d;
    };
  };
  return hashCode;
}


void ModifyPortraits(VIDEOCE *pVCE, MAPPING *pMAP, int *width, int *height)
{
  int i;
  unsigned short *pW;
  pW = (unsigned short *)pVCE->GetBitmap();
  for (i=0; i<pMAP->height; i++, pW+=320)
  {
    *((ui32 *)(pW+44)) = *((ui32 *)(pW+46));
    *((ui32 *)(pW+46)) = *((ui32 *)(pW+48));
    pW[48] = pW[50];
    pW[49] = pW[53];
    *((ui32 *)(pW+50)) = *((ui32 *)(pW+54));
    *((ui32 *)(pW+52)) = *((ui32 *)(pW+56));
    *((ui32 *)(pW+54)) = *((ui32 *)(pW+60));
    *((ui32 *)(pW+56)) = *((ui32 *)(pW+62));
  };
  *width = 60;
  *height = pMAP->height;
}


#define LARGER(x, y)  ((x)>(y))?(x):(y)
#define SMALLER(x, y) ((x)<(y))?(x):(y)

void DrawZ(VIDEOCE *vce,
           i32 srcx1, i32 srcy1,
           i32 dstx1, i32 dsty1,
           i32 width, i32 height,
           i32 windowOverlayIndex)
{
  i32 windowIndex;
  i32 dstx2, dsty2;
  i32 ox1, ox2, oy1, oy2; //Overlapping window
  i32 centerDstX1, centerDstX2, centerWidth;
  MAPPING *pMapping;
  if (height <= 0) return;
  if (width <= 0) return;
  dstx2 = dstx1 + width - 1;
  dsty2 = dsty1 + height - 1;
  for (windowIndex = windowOverlayIndex;
       pMapping = &screenMap[windowList[windowIndex]], pMapping->id != WMAP_END;
       windowIndex++)
  {
    ox1 = pMapping->dstx;
    ox2 = ox1 + pMapping->width - 1;
    oy1 = pMapping->dsty;
    oy2 = oy1 + pMapping->height - 1;
    if (ox1 > dstx2) continue;
    if (ox2 < dstx1) continue;
    if (oy2 < dsty1) continue;
    if (oy1 > dsty2) continue;
    if (ox1 > dstx1)
    {
      //LEFTSIDE
      DrawZ(vce, 
            srcx1, srcy1, 
            dstx1, dsty1, 
            ox1-dstx1, height, 
            windowIndex + 1);
    };
    if (ox2 < dstx2)
    {
      //RIGHTSIDE
      DrawZ(vce,
            srcx1 + ox2 + 1 - dstx1, srcy1,
            ox2+1, dsty1,
            dstx2 - ox2, height,
            windowIndex + 1);
    };
    centerDstX1  = LARGER(dstx1, ox1);
    centerDstX2 = SMALLER(dstx2, ox2);
    centerWidth    = centerDstX2 - centerDstX1 + 1;
    if (centerWidth > 0)
    {
    if (oy2 < dsty2)
    {
      //BOTTOM
      DrawZ(vce,
            srcx1 + centerDstX1 - dstx1, srcy1 + oy2 - dsty1 + 1,
            centerDstX1, oy2 + 1,
            centerWidth, dsty2 - oy2,
            windowIndex + 1);
    };
    if (oy1 > dsty1)
    {
      //TOP
      DrawZ(vce,
            srcx1 + centerDstX1 - dstx1, srcy1,
            centerDstX1, dsty1,
            centerWidth, oy1-dsty1,
            windowIndex + 1);
      };
    };
    return;
  };
  vce->DrawBitmap(srcx1, srcy1, dstx1, dsty1, width, height);
}


void UpdateScreenArea(
                      VIDEOCE *vce,
                      i8   *STScreen,
                      i32   windowIndex,
                      i16  *palette,
                      bool  paletteChanged,
                      ui32 *pOldHash,
                      OVERLAYDATA *pOverlayData)
{
//  i32 LineEnd   = (((pMapping->srcx&0xff0)+pMapping->width+15)/16) * 8;
//  unsigned char nibbles[320];
//  unsigned char *pNibbles = nibbles;
  i32 LineStart;
  i32 i, line, rem, numGroup, g, overlayIncrement;
  i32 width, height;
  i32 numPixel;
  i16 *pPalette;
  ui8 *pOverlay, *pOverlayLine;
  MAPPING *pMapping;
  unsigned short *pBitmap, *d;
  unsigned int newHash;
  unsigned int *s, *src;
  unsigned int n;
  bool overlayChanged;
  pMapping = &screenMap[windowList[windowIndex]];
  LineStart = (pMapping->srcx/16) * 8;
  newHash = DePlane((unsigned char *)(STScreen+160*pMapping->srcy+LineStart), //The first 16-pixel group
                    pMapping->width,
                    pMapping->srcx & 15,
                    pMapping->height);
  overlayChanged = false;
  if (pOverlayData != NULL)
  {
    if (pOverlayData->m_change) overlayChanged = true;
    pOverlayData->m_change = false;
  };
  if (!paletteChanged  && !overlayChanged )
  {
    if (newHash == *pOldHash) return;
  }
  *pOldHash = newHash;
  createPalette16(palette,vce);
  if (pOverlayData != NULL)
  {
//  RecordFile.Comment("overlay active");
    pOverlayData->CreateOverlayTable(palette, true);
    pPalette = pOverlayData->m_table;
    pOverlayLine = currentOverlay.m_overlay+224*135;
    overlayIncrement = -224;
  }
  else
  {
//    RecordFile.Comment("No Overlay");
    pPalette = palette16;
    pOverlayLine = noOverlay;
    overlayIncrement = 0;
  };
  STBLTCount++;

  numPixel = pMapping->width;
  rem = numPixel & 7;
  numGroup = numPixel >> 3;
  pBitmap = vce->GetBitmap();
  src = nibbles;
  for (line=0, s=src; line<pMapping->height; line++, src+=40, pBitmap+=320)
  {
    pOverlay = pOverlayLine;
    for (g=0, d=pBitmap,s=src; g<numGroup; g++)
    {
      n = *(s++);
      for (i=0; i<8; i++)
      {
        *(d++) = pPalette[(16 * *(pOverlay++)) | (n & 15)];
//        {
//          if ((line==0) && (g==0) && (i==0))
//          {
//            char msg[80];
//            sprintf(msg, "%d %d n=%08x *d = %04x",pMapping->width, pMapping->height,n,*(d-1) & 0xffff);
//            RecordFile.Comment(msg);
//          }
//        }
        n >>= 4;
      };
    };
    n = *(s++);
    for (i=0; i<rem; i++)
    {
      *(d++) = pPalette[(16 * *(pOverlay++)) | (n & 15)];
      n >>= 4;
    };
    pOverlayLine += overlayIncrement;
  };
  switch (pMapping->copymode)
  {
  case 0:
    height = pMapping->height;
    width = pMapping->width;
    break;
  case 1:
    ModifyPortraits(vce, pMapping, &width, &height);
    break;
  default:
    height = 0;
    width = 0;  //Get rid of compiler warning.
    die(0xf3f5);
  };
  //vce->DrawBitmap(0,0,pMapping->dstx,pMapping->dsty,width,height);
  DrawZ(vce,
        0, 0,                           // Source xy
        pMapping->dstx, pMapping->dsty, // destination xy
        width, height,                  // size
        windowIndex+1);                 // overlayed windows list
};


void DeterminePictureMapping(unsigned char *vle)
{
  int i=0;
  switch (videoMode)
  {
  case VM_CREDITS:
  case VM_THEEND:
  case VM_CENTERMENU:
  case VM_PRESENTS:
     vle[i++] = WMAP_HCENTER_VFULL;
    break;
  case VM_PRISONDOOR:
    vle[i++] = WMAP_PRISON_DOOR;
    break;
  case VM_LEFTMENU:
  case VM_ADVENTURE:
  case VM_INVENTORY:
  case VM_SLEEPING:
    vle[i++] = WMAP_PORTRAIT_A;
    vle[i++] = WMAP_PORTRAIT_B;
    vle[i++] = WMAP_PORTRAIT_C;
    vle[i++] = WMAP_PORTRAIT_D;
    vle[i++] = WMAP_VIEWPORT;
    vle[i++] = WMAP_SPELL_CONTROLS;
    vle[i++] = WMAP_ATTACK_CONTROLS;
    vle[i++] = WMAP_TURN_LEFT;
    vle[i++] = WMAP_MOVE_FORWARD;
    vle[i++] = WMAP_TURN_RIGHT;
    vle[i++] = WMAP_SLIDE_LEFT;
    vle[i++] = WMAP_MOVE_BACKWARD;
    vle[i++] = WMAP_SLIDE_RIGHT;
//    if (overlappingText)
//    {
//      vle[i++] = WMAP_OVERLAPPEDTEXT;
//    }
//    else
    {
      vle[i++] = WMAP_SCROLLING_TEXT;
    };
    vle[i++] = WMAP_MARCHING_ORDER;
    break;
  case VM_CAST:
  default:
    vle[i++] = WMAP_HFULL_VCENTER;
    break;
  };
  vle[i] = WMAP_END;
  // Now convert each Id to a screenMap index
  for (; i>=0; i--)
  {
    int n;
    for (n=0; screenMap[n].id!=WMAP_END; n++)
    {
      if (screenMap[n].id == vle[i])
      {
        vle[i] = n;
        break;
      };
    };
  };
}


bool pc[2];  //Two palette-changed flags

void display (void)
{
  VIDEOCE vce;
  MAPPING *pMapping;
  int windowIndex;
  static i32 numDisplay = 0;
  if (!bitTablesInitialized) BuildBitTables();
  DeterminePictureMapping(windowList);
  SortWindowListBy_Z_order();
  vce.GetBitmap(320,320);
  numDisplay++;
  if ((VBLMultiplier!=1) && ((d.Time&0xf)!=0) && (VBLMultiplier!=99)) return;
  if (d.DynamicPaletteSwitching)
  {
    memcpy(palette[0],&d.Palette11978, 32);
    memcpy(palette[1],&d.Palette11946, 32);
//  {
//    static bool once = true;
//    if ((d.CurrentPalette == 4) && once)
//    {
//      int i;
//      once = false;
//        for (i=0; i<16; i++)
//      {
//        char msg[80];
//        sprintf(msg," 11946 %d  %04x",i,palette[1][i]&0xffff);
//        RecordFile.Comment(msg);
//      };
//    };
//  };

    memcpy(globalPalette,&d.Palette11978,32);
  }
  else
  {
    memcpy(palette[0],globalPalette, 32);
    memcpy(palette[1],globalPalette, 32);
  };
  pc[0] = HasPaletteChanged(palette[0], oldPalette[0]);
  pc[1] = HasPaletteChanged(palette[1], oldPalette[1]);
  pc[0] = pc[0] || ForcedScreenDraw;
  pc[1] = pc[1] || ForcedScreenDraw;
  pc[0] = pc[0] || (oldVideoMode != videoMode);
  pc[1] = pc[1] || (oldVideoMode != videoMode);
  if (oldVideoMode != videoMode) vce.ClearScreen();
  oldVideoMode = videoMode;
  ForcedScreenDraw = false;
  for (windowIndex=0; 
       pMapping = &screenMap[windowList[windowIndex]], pMapping->id!=WMAP_END; 
       windowIndex++)
  {
    UpdateScreenArea(
                     &vce,
                     physbase(),//STScreen,
                     windowIndex,
                     palette[pMapping->palette],
                     pc[pMapping->palette],
                     &pMapping->checksum,
                     (    overlayActive
                       && pMapping->overlay
                       && (videoMode==VM_ADVENTURE)
                     ) ? &currentOverlay : NULL);
  };
}



#endif
