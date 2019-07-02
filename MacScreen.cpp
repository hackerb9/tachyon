#include "stdafx.h"

#include "UI.h"

#include <stdio.h>

#include "Objects.h"
#include "Dispatch.h"
#include "CSB.h"
#include "data.h"
//#if TARGET_RT_MAC_MACH_O
	#include <DrawSprocket/DrawSprocket.h>
//#else
//	#include <DrawSprocket.h>
	
//	#define	GetPortPixMap(x)	((x)->portPixMap)
//#endif
//extern CDC *OnDrawDC;
unsigned int STBLTCount=0;

long screenSize=2;
extern i16 globalPalette[16];	//palette16
i16 bitmap[640*400];
short counter;
int dstLineLen;
i16 palette1[16];
i16 palette2[16];
i16 oldPalette1[16];
i16 oldPalette2[16];
pnt logbase(void);

extern i32 gDSpScreenWidth;

bool HasPaletteChanged(i16 *palette, i16 *oldpalette)
{
  bool change=false;
  for (int i=0; i<16; i++)
  {
    if (oldpalette[i] != palette[i])
    {
      oldpalette[i]=palette[i];
      change=true;
    };
  };
  return change;
}


#define ROL(a,n) ( (a<<n)|(a>>(32-n)) )
#define ROR(a,n) ( (a>>n)|(a<<(32-n)) )

bool HasScreenChanged(i8 *STScreen,    // First byte to test
                      int lineLen,     // quadwords
                      int numLine,
                      int lineIncrement,
                      i32 *pOldChecksum)
{
	ui32	eax,edx,ebx,edi,ecx;
	i32*	esi;
	bool	result = FALSE;
	
	edx = 0;
	ebx = 0;
	esi = (i32*)STScreen;
	edi = numLine;

do1line:
	ecx = lineLen;
	if(ecx < 4) goto do4bytes;

do16bytes:
	if(ecx < 4) goto do4bytes;
	
	//  lodsd
	eax = esi[0]; 
	esi++;
	
	//  add edx,eax
	edx += eax;
	
	//  lodsd
	eax = esi[0]; 
	esi++;
	
	//  add ebx,eax
	ebx += eax;
	
	//  lodsd
	eax = esi[0]; 
	esi++;
	
	//  add edx,eax
	edx += eax;
	
	//  lodsd
	eax = esi[0]; 
	esi++;
	
	//  add abx,eax
	ebx += eax;
	
	//  sub ecx,4
	ecx-=4;
	if(ecx >= 4) goto do16bytes;
	if(!ecx) 	goto lineDone;

do4bytes:
	edx = ROL(edx,1);
	eax = esi[0]; 
	esi++;
	edx += eax;
	ecx--;
	if(ecx) goto do4bytes;
	
lineDone:
	edx = ROR(edx,1);
	esi += lineIncrement;
	edi--;
	if(edi) goto do1line;
	ebx = ROL(ebx,1);
	edx += ebx;
	esi = pOldChecksum;
	result = 0;
	if(edx == *esi) return 0;
	else
	{
		result = 1;
		*pOldChecksum = edx;
	}

	return result;
	/*i32	i,j,crc = 0;
	
	for(j=0;j<numLine; j++)
	{
		for(i=0; i<lineLen; i++)
		{
			crc += ((unsigned long*)STScreen)[4*i];
			crc += ((unsigned long*)STScreen)[4*i+1];
			crc += ((unsigned long*)STScreen)[4*i+2];
			crc += ((unsigned long*)STScreen)[4*i+3];
		}
		
		STScreen += lineIncrement;
	}
	
	if(*pOldChecksum == crc) return 0;
	*pOldChecksum = crc;
	return 1;*/
	
}


/*bool HasScreenChanged(i8 *STScreen,    // First byte to test
                      int lineLen,     // quadwords
                      int numLine,
                      int lineIncrement,
                      int *pOldChecksum)
{
 #pragma unused(STScreen,lineLen,numLine,lineIncrement,pOldChecksum)
  bool result = true;
  __asm
  {
    sub edx,edx   // The summation
    sub ebx,ebx
    mov esi, STScreen
    mov edi,numLine //
do1line:
    mov ecx,lineLen
    cmp ecx,4
do16Bytes:
    jc do4Bytes
    lodsd
    add edx,eax
    lodsd
    add ebx,eax
    lodsd
    add edx,eax
    lodsd
    add ebx,eax
    sub ecx,4
    cmp ecx,4
    jnc do16Bytes
    or  cx,cx
    jz  lineDone
do4Bytes:
    rol edx,1
    lodsd
    //add ebx,eax
    //ror ebx,1
    add edx,eax
    loop do4Bytes
lineDone:
    ror edx,1
    add esi,lineIncrement
    dec edi
    jnz do1line
    rol ebx,1
    add edx,ebx
    mov esi,pOldChecksum
    mov result,0
    cmp edx,[esi]
    jz  ret0
    mov result,1
ret0:
    mov [esi],edx
  };
  return result;
}
*/

void STBlt(i8 *STScreen, // first 16-pixel group
           int lineLen,  // # 16-pixel groups
           int numLine,
           int lineIncrement)
{
	#pragma unused(STScreen,lineLen,numLine,lineIncrement)
 
/*  __asm
  {
    mov esi,STScreen
    lea edi,bitmap
do1line:
    mov eax,lineLen
    mov counter,ax
do16bits:
    mov ah,[esi+6]
    mov al,[esi+4]
    mov dh,[esi+2]
    mov dl,[esi+0]
    mov cx,8
doTop8bits:
    sub ebx,ebx
    rcl ah,1
    rcl bl,1
    rcl al,1
    rcl bl,1
    rcl dh,1
    rcl bl,1
    rcl dl,1
    rcl bl,1
    mov bx,palette16[2*ebx]
    mov [edi],bx
    add edi,2
    dec cx
    jnz doTop8bits
    mov ah,[esi+7]
    mov al,[esi+5]
    mov dh,[esi+3]
    mov dl,[esi+1]
    mov cx,8
doBottom8bits:
    sub ebx,ebx
    rcl ah,1
    rcl bl,1
    rcl al,1
    rcl bl,1
    rcl dh,1
    rcl bl,1
    rcl dl,1
    rcl bl,1
    mov bx,palette16[2*ebx]
//    mov ebx,0000ff00h
    mov [edi],bx
    add edi,2
    dec cx
    jnz doBottom8bits
    add esi,8
    dec counter
    jnz do16bits
    add esi,lineIncrement
    dec numLine
    jnz do1Line
  };*/
}

void STBlt2(i8 *STScreen, // first 16-pixel group
            int lineLen,  // # 16-pixel groups
            int numLine,
            int lineIncrement)
{
	#pragma unused(STScreen,lineLen,numLine,lineIncrement)
 
/*  __asm
  {
    mov esi,STScreen
    lea edi,bitmap
do1line:
    mov eax,lineLen
    mov counter,ax
    shl eax,6  ; 16 pixels/group and 4 bytes per pixel
    mov dstLineLen,eax
do16bits:
    mov ah,[esi+6]
    mov al,[esi+4]
    mov dh,[esi+2]
    mov dl,[esi+0]
    mov cx,8
    push  esi
    mov esi,dstLineLen
doTop8bits:
    sub ebx,ebx
    rcl ah,1
    rcl bl,1
    rcl al,1
    rcl bl,1
    rcl dh,1
    rcl bl,1
    rcl dl,1
    rcl bl,1
    mov bx,palette16[2*ebx]
    mov [edi],bx
    mov [edi+esi],bx
    add edi,2
    mov [edi],bx
    mov [edi+esi],bx
    add edi,2
    dec cx
    jnz doTop8bits
    pop esi
    mov ah,[esi+7]
    mov al,[esi+5]
    mov dh,[esi+3]
    mov dl,[esi+1]
    mov cx,8
    push  esi
    mov esi,dstLineLen
doBottom8bits:
    sub ebx,ebx
    rcl ah,1
    rcl bl,1
    rcl al,1
    rcl bl,1
    rcl dh,1
    rcl bl,1
    rcl dl,1
    rcl bl,1
    mov bx,palette16[2*ebx]
    mov [edi],bx
    mov [edi+esi],bx
    add edi,2
    mov [edi],bx
    mov [edi+esi],bx
    add edi,2
    dec cx
    jnz doBottom8bits
    pop esi
    add esi,8
    dec counter
    jnz do16bits
    add esi,lineIncrement
    add edi,dstLineLen
    dec numLine
    jnz do1Line
  };*/
}

/*
void createPalette32(i16 *palette)
{ // Create 32-bit color palette from the ST 9-bit palette
  for (int i=0; i<16; i++)
  {
    ASSERT(palette[i]<=0x777);
    palette32[i]  = (((palette[i]>>8) & 7)*0xff/0x7)<<16;
    palette32[i] |= (((palette[i]>>4) & 7)*0xff/0x7)<< 8;
    palette32[i] |= (((palette[i]>>0) & 7)*0xff/0x7)<< 0;
  };
}
*/

void createPalette16(i16 *palette)
{ // Create 16-bit color palette from the ST 9-bit palette
  for (int i=0; i<16; i++)
  {
    ASSERT(palette[i]<=0x777);
    globalPalette[i]=0;
    globalPalette[i] |= (((palette[i]>>8) & 7)*0x1f/0x7)<<10;//red
    globalPalette[i] |= (((palette[i]>>4) & 7)*0x1f/0x7)<< 5;//green
    globalPalette[i] |= (((palette[i]>>0) & 7)*0x1f/0x7)<< 0;//blue
  };
}


bool ForcedScreenDraw = false;

void ForceScreenDraw(void)
{
  ForcedScreenDraw = true;
}


/*void UpdateScreenArea(
                      i8  *STScreen,
                      int  x0,
                      int  y0,
                      int  width,
                      int  height,
                      i16 *palette,
                      bool paletteChanged,
                      int *pOldChecksum)
{
  int LineStart = (x0/16) * 8;
  int LineEnd   = ((x0+width+15)/16) * 8;
  if (!paletteChanged)
  {
    if (!HasScreenChanged(STScreen+160*y0+LineStart,
                          (LineEnd-LineStart) / 4,
                          height,
                          160-(LineEnd-LineStart),
                          pOldChecksum)) return;
  };
  createPalette16(palette);
  if (screenSize==2)
    STBlt2(STScreen + 160*y0 + LineStart,
           (LineEnd-LineStart)/8,
           height,
           160-(LineEnd-LineStart));
  else
    STBlt(STScreen + 160*y0 + LineStart,
          (LineEnd-LineStart)/8,
          height,
          160-(LineEnd-LineStart));
  STBLTCount++;

  /*bitmapInfo.bmiHeader.biSize=0x28;
  bitmapInfo.bmiHeader.biWidth=width * screenSize;
  bitmapInfo.bmiHeader.biHeight=-height * screenSize;
  bitmapInfo.bmiHeader.biPlanes=1;
  bitmapInfo.bmiHeader.biBitCount=16;
  bitmapInfo.bmiHeader.biCompression=BI_RGB;
  bitmapInfo.bmiHeader.biSizeImage=0;
  bitmapInfo.bmiHeader.biXPelsPerMeter=0;
  bitmapInfo.bmiHeader.biYPelsPerMeter=0;
  bitmapInfo.bmiHeader.biClrUsed=0;
  bitmapInfo.bmiHeader.biClrImportant=0;

  UI_SetDIBitsToDevice(
                  // handle to device context
    x0*screenSize,    // x-coordinate of upper-left corner of
                      // dest. rect.
    y0*screenSize,    // y-coordinate of upper-left corner of
                      // dest. rect.
    width*screenSize, // source rectangle width
    height*screenSize,// source rectangle height
    0,                // x-coordinate of lower-left corner of
                      // source rect.
    0,                // y-coordinate of lower-left corner of
                      // source rect.
    0,                // first scan line in array
    height*screenSize,// number of scan lines
    (char *)bitmap,   // address of array with DIB bits
    &bitmapInfo,      // address of structure with bitmap info.
    DIB_RGB_COLORS    // RGB or palette indexes
    );
  
};
*/

extern DSpContextReference	gContextRef;

/*static RGBColor pix2pal(i16* pal, unsigned char id)
{	
	RGBColor out;
	register ui16 vlue, a;
	
	vlue = pal[id];
	
	out.red   = ((vlue>>8)&7)*0x20;
	out.green = ((vlue>>4)&7)*0x20;
	out.blue  = ((vlue>>0)&7)*0x20;
	return out;
} */

/*void UpdateScreenArea(
                      i8  *STScreen,
                      int  x0,
                      int  y0,
                      int  width,
                      int  height,
                      i16 *pal,
                      bool paletteChanged,
                      i32 *pOldChecksum)
{
  #pragma unused(x0, y0, width, height, pal)

  
	CGrafPtr		wp;
  	Ptr				s;
	
	register ui32 				x,xmax,xmin, 
								y,ymax;
	register ui32 				q0,q1;
	register ui32				red,green,blue;
	
	//register unsigned long long b8;
	register Ptr				dest;
	register unsigned long 		rr, 
								res,
								buf,
								rgba;
  	register i8*				stscreen = STScreen;
	
	if (!paletteChanged)
	{
	    i32 LineStart = (x0/16) * 8;
		i32 LineEnd   = ((x0+width+15)/16) * 8;
		
		if (!HasScreenChanged(STScreen+160*y0+LineStart,
	                          (LineEnd-LineStart) / 4,
	                          height,
	                          160-(LineEnd-LineStart),
	                          pOldChecksum)) return;
	};
  	
	DSpContext_GetBackBuffer(gContextRef,kDSpBufferKind_Normal,(CGrafPtr*)&wp);
	//DSpContext_GetFrontBuffer(gContextRef,(CGrafPtr*)&wp);
	s = GetPixBaseAddr(GetPortPixMap(wp))+40*4*640;

	
	#define DRAW(aa,n) {		dest = &s[8*(640*y+16*x+n)+1];	\
							asm{extrwi	res,aa,4,4*(n%8);	\
								slwi	res,res,1		;	\
								lhzx	res,rr,res		;	\
								li		rgba,0			;	\
								extrwi	blue,res,4,28	;	\
								extrwi	green,res,4,24	;	\
								extrwi	red,res,4,20	;	\
								insrwi	rgba,red,3,0	;	\
								insrwi	rgba,green,3,8	;	\
								insrwi	rgba,blue,3,16	;	\
								stw		rgba,0(dest)	;	\
								stw		rgba,4(dest)	;	\
								stw		rgba,2560(dest)	;	\
								stw		rgba,2564(dest)	;	\
							}					}
	xmin = x0/16;
	xmax = (x0+width+7)/16;
	ymax = y0+height;
	
	for(y=y0;y<ymax;y++)
	{
		for(x=xmin;x<xmax;x++)
		{
			//buf = (ui32)(STScreen + y*160 + x*8);
			asm {
				li 		rr,160
				li		q0,0
				slwi	buf,x,3
				mullw	rr,rr,y
				li		q1,0
				add		buf,rr,buf
				li		rr,0
				add		buf,stscreen,buf

				lwz		res,4(buf)
//				extrwi	rr,res,1,31
//				insrwi	q1,rr,1,28
				rlwimi	q1,res,3,28,28
				
//				extrwi	rr,res,1,30
//				insrwi	q1,rr,1,24
				rlwimi	q1,res,6,24,24
				
//				extrwi	rr,res,1,29
//				insrwi	q1,rr,1,20
				rlwimi	q1,res,9,20,20
				
//				extrwi	rr,res,1,28
//				insrwi	q1,rr,1,16
				rlwimi	q1,res,12,16,16

//				extrwi	rr,res,1,27
//				insrwi	q1,rr,1,12
				rlwimi	q1,res,15,12,12

//				extrwi	rr,res,1,26
//				insrwi	q1,rr,1,8
				rlwimi	q1,res,18,8,8

//				extrwi	rr,res,1,25
//				insrwi	q1,rr,1,4
				rlwimi	q1,res,21,4,4

//				extrwi	rr,res,1,24
//				insrwi	q1,rr,1,0
				rlwimi	q1,res,24,0,0

//

//				extrwi	rr,res,1,23
//				insrwi	q0,rr,1,28
				rlwimi	q0,res,27,28,28
				
//				extrwi	rr,res,1,22
//				insrwi	q0,rr,1,24
				rlwimi	q0,res,30,24,24

//				extrwi	rr,res,1,21
//				insrwi	q0,rr,1,20
				rlwimi	q0,res,1,20,20

//				extrwi	rr,res,1,20
//				insrwi	q0,rr,1,16
				rlwimi	q0,res,4,16,16

//				extrwi	rr,res,1,19
//				insrwi	q0,rr,1,12
				rlwimi	q0,res,7,12,12

//				extrwi	rr,res,1,18
//				insrwi	q0,rr,1,8
				rlwimi	q0,res,10,8,8

//				extrwi	rr,res,1,17
//				insrwi	q0,rr,1,4
				rlwimi	q0,res,13,4,4

//				extrwi	rr,res,1,16
//				insrwi	q0,rr,1,0
				rlwimi	q0,res,16,0,0


//
//

//				extrwi	rr,res,1,15
//				insrwi	q1,rr,1,29
				rlwimi	q1,res,18,29,29
					
//				extrwi	rr,res,1,14
//				insrwi	q1,rr,1,25
				rlwimi	q1,res,21,25,25

//				extrwi	rr,res,1,13
//				insrwi	q1,rr,1,21
				rlwimi	q1,res,24,21,21

//				extrwi	rr,res,1,12
//				insrwi	q1,rr,1,17
				rlwimi	q1,res,27,17,17

//				extrwi	rr,res,1,11
//				insrwi	q1,rr,1,13
				rlwimi	q1,res,30,13,13

//				extrwi	rr,res,1,10
//				insrwi	q1,rr,1,9
				rlwimi	q1,res,1,9,9

//				extrwi	rr,res,1,9
//				insrwi	q1,rr,1,5
				rlwimi	q1,res,4,5,5

//				extrwi	rr,res,1,8
//				insrwi	q1,rr,1,1
				rlwimi	q1,res,7,1,1


//

//				extrwi	rr,res,1,7
//				insrwi	q0,rr,1,29
				rlwimi	q0,res,10,29,29
				
//				extrwi	rr,res,1,6
//				insrwi	q0,rr,1,25
				rlwimi	q0,res,13,25,25

//				extrwi	rr,res,1,5
//				insrwi	q0,rr,1,21
				rlwimi	q0,res,16,21,21
				
//				extrwi	rr,res,1,4
//				insrwi	q0,rr,1,17
				rlwimi	q0,res,19,17,17

//				extrwi	rr,res,1,3
//				insrwi	q0,rr,1,13
				rlwimi	q0,res,22,13,13

//				extrwi	rr,res,1,2
//				insrwi	q0,rr,1,9
				rlwimi	q0,res,25,9,9

//				extrwi	rr,res,1,1
//				insrwi	q0,rr,1,5
				rlwimi	q0,res,28,5,5

//				extrwi	rr,res,1,0
//				insrwi	q0,rr,1,1
				rlwimi	q0,res,31,1,1
				
	///////////////
		
				lwz		res,0(buf)

	///////////////

//				extrwi	rr,res,1,31
//				insrwi	q1,rr,1,30
				rlwimi	q1,res,1,30,30	
					
//				extrwi	rr,res,1,30
//				insrwi	q1,rr,1,26
				rlwimi	q1,res,4,26,26	

//				extrwi	rr,res,1,29
//				insrwi	q1,rr,1,22
				rlwimi	q1,res,7,22,22	

//				extrwi	rr,res,1,28
//				insrwi	q1,rr,1,18
				rlwimi	q1,res,10,18,18	

//				extrwi	rr,res,1,27
//				insrwi	q1,rr,1,14
				rlwimi	q1,res,13,14,14	

//				extrwi	rr,res,1,26
//				insrwi	q1,rr,1,10
				rlwimi	q1,res,16,10,10	

//				extrwi	rr,res,1,25
//				insrwi	q1,rr,1,6
				rlwimi	q1,res,19,6,6	

//				extrwi	rr,res,1,24
//				insrwi	q1,rr,1,2
				rlwimi	q1,res,22,2,2	


//

//				extrwi	rr,res,1,23
//				insrwi	q0,rr,1,30
				rlwimi	q0,res,25,30,30
				
//				extrwi	rr,res,1,22
//				insrwi	q0,rr,1,26
				rlwimi	q0,res,28,26,26

//				extrwi	rr,res,1,21
//				insrwi	q0,rr,1,22
				rlwimi	q0,res,31,22,22

//				extrwi	rr,res,1,20
//				insrwi	q0,rr,1,18
				rlwimi	q0,res,2,18,18

//				extrwi	rr,res,1,19
//				insrwi	q0,rr,1,14
				rlwimi	q0,res,5,14,14

//				extrwi	rr,res,1,18
//				insrwi	q0,rr,1,10
				rlwimi	q0,res,8,10,10

//				extrwi	rr,res,1,17
//				insrwi	q0,rr,1,6
				rlwimi	q0,res,11,6,6

//				extrwi	rr,res,1,16
//				insrwi	q0,rr,1,2
				rlwimi	q0,res,14,2,2


//
//

//				extrwi	rr,res,1,15
//				insrwi	q1,rr,1,31
				rlwimi	q1,res,16,31,31

//				extrwi	rr,res,1,14
//				insrwi	q1,rr,1,27
				rlwimi	q1,res,19,27,27

//				extrwi	rr,res,1,13
//				insrwi	q1,rr,1,23
				rlwimi	q1,res,22,23,23

//				extrwi	rr,res,1,12
//				insrwi	q1,rr,1,19
				rlwimi	q1,res,25,19,19

//				extrwi	rr,res,1,11
//				insrwi	q1,rr,1,15
				rlwimi	q1,res,28,15,15

//				extrwi	rr,res,1,10
//				insrwi	q1,rr,1,11
				rlwimi	q1,res,31,11,11

//				extrwi	rr,res,1,9
//				insrwi	q1,rr,1,7
				rlwimi	q1,res,2,7,7

//				extrwi	rr,res,1,8
//				insrwi	q1,rr,1,3
				rlwimi	q1,res,5,3,3


//

//				extrwi	rr,res,1,7
//				insrwi	q0,rr,1,31
				rlwimi	q0,res,8,31,31
				
//				extrwi	rr,res,1,6
//				insrwi	q0,rr,1,27
				rlwimi	q0,res,11,27,27

//				extrwi	rr,res,1,5
//				insrwi	q0,rr,1,23
				rlwimi	q0,res,14,23,23

//				extrwi	rr,res,1,4
//				insrwi	q0,rr,1,19
				rlwimi	q0,res,17,19,19

//				extrwi	rr,res,1,3
//				insrwi	q0,rr,1,15
				rlwimi	q0,res,20,15,15

//				extrwi	rr,res,1,2
//				insrwi	q0,rr,1,11
				rlwimi	q0,res,23,11,11

//				extrwi	rr,res,1,1
//				insrwi	q0,rr,1,7
				rlwimi	q0,res,26,7,7

//				extrwi	rr,res,1,0
//				insrwi	q0,rr,1,3
				rlwimi	q0,res,29,3,3

				lwz		rr,pal		
			}
			
			DRAW(q0,0);
			DRAW(q0,1);
			DRAW(q0,2);
			DRAW(q0,3);
			DRAW(q0,4);
			DRAW(q0,5);
			DRAW(q0,6);
			DRAW(q0,7);

			DRAW(q1,8);
			DRAW(q1,9);
			DRAW(q1,10);
			DRAW(q1,11);
			DRAW(q1,12);
			DRAW(q1,13);
			DRAW(q1,14);
			DRAW(q1,15);			
			
		}
	}
	

	STBLTCount++;
	
	DSpContext_SwapBuffers(gContextRef,NULL,NULL);
	return ;
}*/

bool	INTERLACED	= 	0;

void UpdateScreenArea1024(
                      i8  *STScreen,
                      int  x0,
                      int  y0,
                      int  width,
                      int  height,
                      i16 *pal,
                      bool paletteChanged,
                      i32 *pOldChecksum)
{
  #pragma unused(x0, y0, width, height, pal)

  	enum {
  		ROW_BYTES	=	4*1024
  	};
  	
  	
	CGrafPtr					wp;
  	unsigned char*				s;
	
	register unsigned long 		x,xmax,xmin,y,ymax;
	register unsigned long 		q0,q1;
	register unsigned char*		dest;
	register unsigned long 		rr,res,buf,argb;
  	register unsigned char*		stscreen = (unsigned char*)STScreen;
	
	if (!paletteChanged)
	{
	    i32 LineStart = (x0/16) * 8;
		i32 LineEnd   = ((x0+width+15)/16) * 8;
		
		if (!HasScreenChanged(STScreen+160*y0+LineStart,
	                          (LineEnd-LineStart) / 4,
	                          height,
	                          160-(LineEnd-LineStart),
	                          pOldChecksum)) return;
	};
  	
	DSpContext_GetBackBuffer(gContextRef,kDSpBufferKind_Normal,(CGrafPtr*)&wp);
	s = (unsigned char*)GetPixBaseAddr(GetPortPixMap(wp))+40*ROW_BYTES;

								
		#define DRAWI(aa,n) {		dest = &s[8*(1024*y+16*x+n)];	\
								asm{li		argb,0				;	\
									extrwi	res,aa,4,4*(n%8)	;	\
									slwi	res,res,1			;	\
									lhzx	res,rr,res			;	\
									rlwimi	argb,res,13,8,10	;	\
									rlwimi	argb,res,9,16,18	;	\
									rlwimi	argb,res,5,24,26	;	\
									stw		argb,0(dest)		;	\
									stw		argb,4(dest)		;	\
									srwi	argb,argb,1			;	\
									stw		argb,4096(dest)		;	\
									stw		argb,4100(dest)		;	\
									}	}

		#define DRAW(aa,n) {		dest = &s[8*(1024*y+16*x+n)];	\
								asm{li		argb,0				;	\
									extrwi	res,aa,4,4*(n%8)	;	\
									slwi	res,res,1			;	\
									lhzx	res,rr,res			;	\
									rlwimi	argb,res,13,8,10	;	\
									rlwimi	argb,res,9,16,18	;	\
									rlwimi	argb,res,5,24,26	;	\
									stw		argb,0(dest)		;	\
									stw		argb,4(dest)		;	\
									stw		argb,4096(dest)		;	\
									stw		argb,4100(dest)		;	\
									}	}
	
	xmin = x0>>4;
	xmax = (x0+width+15)>>4;
	//xmax = (x0+width+7)/16;
	ymax = y0+height;
	
	if(!INTERLACED)
	{	
		for(y=y0;y<ymax;y++)
		{
			for(x=xmin;x<xmax;x++)
			{
				//buf = (ui32)(STScreen + y*160 + x*8);
				asm {
					li 		rr,160
					li		q0,0
					slwi	buf,x,3
					mullw	rr,rr,y
					li		q1,0
					add		buf,rr,buf
					li		rr,0
					add		buf,stscreen,buf

					lwz		res,4(buf)

					rlwimi	q1,res,3,28,28
					rlwimi	q0,res,27,28,28
					rlwimi	q1,res,6,24,24
					rlwimi	q0,res,30,24,24
					rlwimi	q1,res,9,20,20
					rlwimi	q0,res,1,20,20
					rlwimi	q1,res,12,16,16
					rlwimi	q0,res,4,16,16
					rlwimi	q1,res,15,12,12
					rlwimi	q0,res,7,12,12
					rlwimi	q1,res,18,8,8
					rlwimi	q0,res,10,8,8
					rlwimi	q1,res,21,4,4
					rlwimi	q0,res,13,4,4
					rlwimi	q1,res,24,0,0
					rlwimi	q0,res,16,0,0

					rlwimi	q1,res,18,29,29
					rlwimi	q0,res,10,29,29
					rlwimi	q1,res,21,25,25
					rlwimi	q0,res,13,25,25
					rlwimi	q1,res,24,21,21
					
					rlwimi	q0,res,16,21,21
					rlwimi	q1,res,27,17,17
					rlwimi	q0,res,19,17,17
					rlwimi	q1,res,30,13,13
					rlwimi	q0,res,22,13,13
					rlwimi	q1,res,1,9,9
					rlwimi	q0,res,25,9,9
					rlwimi	q1,res,4,5,5
					rlwimi	q0,res,28,5,5
					rlwimi	q1,res,7,1,1
					rlwimi	q0,res,31,1,1
					
					lwz		res,0(buf)

					rlwimi	q1,res,1,30,30	
					rlwimi	q0,res,25,30,30
					rlwimi	q1,res,4,26,26	
					rlwimi	q0,res,28,26,26
					rlwimi	q1,res,7,22,22	
					rlwimi	q0,res,31,22,22
					rlwimi	q1,res,10,18,18	
					rlwimi	q0,res,2,18,18
					rlwimi	q1,res,13,14,14	
					rlwimi	q0,res,5,14,14
					rlwimi	q1,res,16,10,10	
					rlwimi	q0,res,8,10,10
					rlwimi	q1,res,19,6,6	
					rlwimi	q0,res,11,6,6
					rlwimi	q1,res,22,2,2	
					rlwimi	q0,res,14,2,2

					rlwimi	q1,res,16,31,31
					rlwimi	q0,res,8,31,31
					rlwimi	q1,res,19,27,27
					rlwimi	q0,res,11,27,27
					rlwimi	q1,res,22,23,23
					rlwimi	q0,res,14,23,23
					rlwimi	q1,res,25,19,19
					rlwimi	q0,res,17,19,19
					rlwimi	q1,res,28,15,15
					rlwimi	q0,res,20,15,15
					rlwimi	q1,res,31,11,11
					rlwimi	q0,res,23,11,11
					rlwimi	q1,res,2,7,7
					rlwimi	q0,res,26,7,7
					rlwimi	q1,res,5,3,3
					rlwimi	q0,res,29,3,3

					lwz		rr,pal		
				}
				
				DRAW(q0,0);
				DRAW(q0,1);
				DRAW(q0,2);
				DRAW(q0,3);
				DRAW(q0,4);
				DRAW(q0,5);
				DRAW(q0,6);
				DRAW(q0,7);

				DRAW(q1,8);
				DRAW(q1,9);
				DRAW(q1,10);
				DRAW(q1,11);
				DRAW(q1,12);
				DRAW(q1,13);
				DRAW(q1,14);
				DRAW(q1,15);			
				
			}
		}
	}
	else	
	{
		for(y=y0;y<ymax;y++)
		{
			for(x=xmin;x<xmax;x++)
			{
				//buf = (ui32)(STScreen + y*160 + x*8);
				asm {
					li 		rr,160
					li		q0,0
					slwi	buf,x,3
					mullw	rr,rr,y
					li		q1,0
					add		buf,rr,buf
					li		rr,0
					add		buf,stscreen,buf

					lwz		res,4(buf)

					rlwimi	q1,res,3,28,28
					rlwimi	q0,res,27,28,28
					rlwimi	q1,res,6,24,24
					rlwimi	q0,res,30,24,24
					rlwimi	q1,res,9,20,20
					rlwimi	q0,res,1,20,20
					rlwimi	q1,res,12,16,16
					rlwimi	q0,res,4,16,16
					rlwimi	q1,res,15,12,12
					rlwimi	q0,res,7,12,12
					rlwimi	q1,res,18,8,8
					rlwimi	q0,res,10,8,8
					rlwimi	q1,res,21,4,4
					rlwimi	q0,res,13,4,4
					rlwimi	q1,res,24,0,0
					rlwimi	q0,res,16,0,0

					rlwimi	q1,res,18,29,29
					rlwimi	q0,res,10,29,29
					rlwimi	q1,res,21,25,25
					rlwimi	q0,res,13,25,25
					rlwimi	q1,res,24,21,21
					rlwimi	q0,res,16,21,21
					rlwimi	q1,res,27,17,17
					rlwimi	q0,res,19,17,17
					rlwimi	q1,res,30,13,13
					rlwimi	q0,res,22,13,13
					rlwimi	q1,res,1,9,9
					rlwimi	q0,res,25,9,9
					rlwimi	q1,res,4,5,5
					rlwimi	q0,res,28,5,5
					rlwimi	q1,res,7,1,1
					rlwimi	q0,res,31,1,1
					
					lwz		res,0(buf)

					rlwimi	q1,res,1,30,30	
					rlwimi	q0,res,25,30,30
					rlwimi	q1,res,4,26,26	
					rlwimi	q0,res,28,26,26
					rlwimi	q1,res,7,22,22	
					rlwimi	q0,res,31,22,22
					rlwimi	q1,res,10,18,18	
					rlwimi	q0,res,2,18,18
					rlwimi	q1,res,13,14,14	
					rlwimi	q0,res,5,14,14
					rlwimi	q1,res,16,10,10	
					rlwimi	q0,res,8,10,10
					rlwimi	q1,res,19,6,6	
					rlwimi	q0,res,11,6,6
					rlwimi	q1,res,22,2,2	
					rlwimi	q0,res,14,2,2

					rlwimi	q1,res,16,31,31
					rlwimi	q0,res,8,31,31
					rlwimi	q1,res,19,27,27
					rlwimi	q0,res,11,27,27
					rlwimi	q1,res,22,23,23
					rlwimi	q0,res,14,23,23
					rlwimi	q1,res,25,19,19
					rlwimi	q0,res,17,19,19
					rlwimi	q1,res,28,15,15
					rlwimi	q0,res,20,15,15
					rlwimi	q1,res,31,11,11
					rlwimi	q0,res,23,11,11
					rlwimi	q1,res,2,7,7
					rlwimi	q0,res,26,7,7
					rlwimi	q1,res,5,3,3
					rlwimi	q0,res,29,3,3

					lwz		rr,pal		
				}
				
				DRAWI(q0,0);
				DRAWI(q0,1);
				DRAWI(q0,2);
				DRAWI(q0,3);
				DRAWI(q0,4);
				DRAWI(q0,5);
				DRAWI(q0,6);
				DRAWI(q0,7);

				DRAWI(q1,8);
				DRAWI(q1,9);
				DRAWI(q1,10);
				DRAWI(q1,11);
				DRAWI(q1,12);
				DRAWI(q1,13);
				DRAWI(q1,14);
				DRAWI(q1,15);			
				
			}
		}
	}
	STBLTCount++;
	
	DSpContext_SwapBuffers(gContextRef,NULL,NULL);
	return ;
}

void UpdateScreenArea640(
                      i8  *STScreen,
                      int  x0,
                      int  y0,
                      int  width,
                      int  height,
                      i16 *pal,
                      bool paletteChanged,
                      i32 *pOldChecksum)
{
  #pragma unused(x0, y0, width, height, pal)

  	enum {
  		ROW_BYTES	=	4*640
  	};
  	
  	
	CGrafPtr					wp;
  	unsigned char*				s;
	
	register unsigned long 		x,xmax,xmin,y,ymax;
	register unsigned long 		q0,q1;
	register unsigned char*		dest;
	register unsigned long 		rr,res,buf,argb;
  	register unsigned char*		stscreen = (unsigned char*)STScreen;
	
	if (!paletteChanged)
	{
	    i32 LineStart = (x0/16) * 8;
		i32 LineEnd   = ((x0+width+15)/16) * 8;
		
		if (!HasScreenChanged(STScreen+160*y0+LineStart,
	                          (LineEnd-LineStart) / 4,
	                          height,
	                          160-(LineEnd-LineStart),
	                          pOldChecksum)) return;
	};
  	
	DSpContext_GetBackBuffer(gContextRef,kDSpBufferKind_Normal,(CGrafPtr*)&wp);
	s = (unsigned char*)GetPixBaseAddr(GetPortPixMap(wp))+40*ROW_BYTES;

	
		#define DRAWIX(aa,n) {		dest = &s[8*(640*y+16*x+n)];	\
								asm{li		argb,0				;	\
									extrwi	res,aa,4,4*(n%8)	;	\
									slwi	res,res,1			;	\
									lhzx	res,rr,res			;	\
									rlwimi	argb,res,13,8,10	;	\
									rlwimi	argb,res,9,16,18	;	\
									rlwimi	argb,res,5,24,26	;	\
									stw		argb,0(dest)		;	\
									stw		argb,4(dest)		;	\
									srwi	argb,argb,1			;	\
									stw		argb,2560(dest)		;	\
									stw		argb,2564(dest)		;	\
									}	}

		#define DRAWX(aa,n) {		dest = &s[8*(640*y+16*x+n)];	\
								asm{li		argb,0				;	\
									extrwi	res,aa,4,4*(n%8)	;	\
									slwi	res,res,1			;	\
									lhzx	res,rr,res			;	\
									rlwimi	argb,res,13,8,10	;	\
									rlwimi	argb,res,9,16,18	;	\
									rlwimi	argb,res,5,24,26	;	\
									stw		argb,0(dest)		;	\
									stw		argb,4(dest)		;	\
									stw		argb,2560(dest)		;	\
									stw		argb,2564(dest)		;	\
									}	}
	
	xmin = x0>>4;
	xmax = (x0+width+15)>>4;
	//xmax = (x0+width+7)/16;
	ymax = y0+height;
	
	if(!INTERLACED)
	{	
		for(y=y0;y<ymax;y++)
		{
			for(x=xmin;x<xmax;x++)
			{
				//buf = (ui32)(STScreen + y*160 + x*8);
				asm {
					li 		rr,160
					li		q0,0
					slwi	buf,x,3
					mullw	rr,rr,y
					li		q1,0
					add		buf,rr,buf
					li		rr,0
					add		buf,stscreen,buf

					lwz		res,4(buf)

					rlwimi	q1,res,3,28,28
					rlwimi	q0,res,27,28,28
					rlwimi	q1,res,6,24,24
					rlwimi	q0,res,30,24,24
					rlwimi	q1,res,9,20,20
					rlwimi	q0,res,1,20,20
					rlwimi	q1,res,12,16,16
					rlwimi	q0,res,4,16,16
					rlwimi	q1,res,15,12,12
					rlwimi	q0,res,7,12,12
					rlwimi	q1,res,18,8,8
					rlwimi	q0,res,10,8,8
					rlwimi	q1,res,21,4,4
					rlwimi	q0,res,13,4,4
					rlwimi	q1,res,24,0,0
					rlwimi	q0,res,16,0,0

					rlwimi	q1,res,18,29,29
					rlwimi	q0,res,10,29,29
					rlwimi	q1,res,21,25,25
					rlwimi	q0,res,13,25,25
					rlwimi	q1,res,24,21,21
					rlwimi	q0,res,16,21,21
					rlwimi	q1,res,27,17,17
					rlwimi	q0,res,19,17,17
					rlwimi	q1,res,30,13,13
					rlwimi	q0,res,22,13,13
					rlwimi	q1,res,1,9,9
					rlwimi	q0,res,25,9,9
					rlwimi	q1,res,4,5,5
					rlwimi	q0,res,28,5,5
					rlwimi	q1,res,7,1,1
					rlwimi	q0,res,31,1,1
					
					lwz		res,0(buf)

					rlwimi	q1,res,1,30,30	
					rlwimi	q0,res,25,30,30
					rlwimi	q1,res,4,26,26	
					rlwimi	q0,res,28,26,26
					rlwimi	q1,res,7,22,22	
					rlwimi	q0,res,31,22,22
					rlwimi	q1,res,10,18,18	
					rlwimi	q0,res,2,18,18
					rlwimi	q1,res,13,14,14	
					rlwimi	q0,res,5,14,14
					rlwimi	q1,res,16,10,10	
					rlwimi	q0,res,8,10,10
					rlwimi	q1,res,19,6,6	
					rlwimi	q0,res,11,6,6
					rlwimi	q1,res,22,2,2	
					rlwimi	q0,res,14,2,2

					rlwimi	q1,res,16,31,31
					rlwimi	q0,res,8,31,31
					rlwimi	q1,res,19,27,27
					rlwimi	q0,res,11,27,27
					rlwimi	q1,res,22,23,23
					rlwimi	q0,res,14,23,23
					rlwimi	q1,res,25,19,19
					rlwimi	q0,res,17,19,19
					rlwimi	q1,res,28,15,15
					rlwimi	q0,res,20,15,15
					rlwimi	q1,res,31,11,11
					rlwimi	q0,res,23,11,11
					rlwimi	q1,res,2,7,7
					rlwimi	q0,res,26,7,7
					rlwimi	q1,res,5,3,3
					rlwimi	q0,res,29,3,3

					lwz		rr,pal		
				}
				
				DRAWX(q0,0);
				DRAWX(q0,1);
				DRAWX(q0,2);
				DRAWX(q0,3);
				DRAWX(q0,4);
				DRAWX(q0,5);
				DRAWX(q0,6);
				DRAWX(q0,7);

				DRAWX(q1,8);
				DRAWX(q1,9);
				DRAWX(q1,10);
				DRAWX(q1,11);
				DRAWX(q1,12);
				DRAWX(q1,13);
				DRAWX(q1,14);
				DRAWX(q1,15);			
				
			}
		}
	}
	else	
	{
		for(y=y0;y<ymax;y++)
		{
			for(x=xmin;x<xmax;x++)
			{
				//buf = (ui32)(STScreen + y*160 + x*8);
				asm {
					li 		rr,160
					li		q0,0
					slwi	buf,x,3
					mullw	rr,rr,y
					li		q1,0
					add		buf,rr,buf
					li		rr,0
					add		buf,stscreen,buf

					lwz		res,4(buf)

					rlwimi	q1,res,3,28,28
					rlwimi	q0,res,27,28,28
					rlwimi	q1,res,6,24,24
					rlwimi	q0,res,30,24,24
					rlwimi	q1,res,9,20,20
					rlwimi	q0,res,1,20,20
					rlwimi	q1,res,12,16,16
					rlwimi	q0,res,4,16,16
					rlwimi	q1,res,15,12,12
					rlwimi	q0,res,7,12,12
					rlwimi	q1,res,18,8,8
					rlwimi	q0,res,10,8,8
					rlwimi	q1,res,21,4,4
					rlwimi	q0,res,13,4,4
					rlwimi	q1,res,24,0,0
					rlwimi	q0,res,16,0,0

					rlwimi	q1,res,18,29,29
					rlwimi	q0,res,10,29,29
					rlwimi	q1,res,21,25,25
					rlwimi	q0,res,13,25,25
					rlwimi	q1,res,24,21,21
					rlwimi	q0,res,16,21,21
					rlwimi	q1,res,27,17,17
					rlwimi	q0,res,19,17,17
					rlwimi	q1,res,30,13,13
					rlwimi	q0,res,22,13,13
					rlwimi	q1,res,1,9,9
					rlwimi	q0,res,25,9,9
					rlwimi	q1,res,4,5,5
					rlwimi	q0,res,28,5,5
					rlwimi	q1,res,7,1,1
					rlwimi	q0,res,31,1,1
					
					lwz		res,0(buf)

					rlwimi	q1,res,1,30,30	
					rlwimi	q0,res,25,30,30
					rlwimi	q1,res,4,26,26	
					rlwimi	q0,res,28,26,26
					rlwimi	q1,res,7,22,22	
					rlwimi	q0,res,31,22,22
					rlwimi	q1,res,10,18,18	
					rlwimi	q0,res,2,18,18
					rlwimi	q1,res,13,14,14	
					rlwimi	q0,res,5,14,14
					rlwimi	q1,res,16,10,10	
					rlwimi	q0,res,8,10,10
					rlwimi	q1,res,19,6,6	
					rlwimi	q0,res,11,6,6
					rlwimi	q1,res,22,2,2	
					rlwimi	q0,res,14,2,2

					rlwimi	q1,res,16,31,31
					rlwimi	q0,res,8,31,31
					rlwimi	q1,res,19,27,27
					rlwimi	q0,res,11,27,27
					rlwimi	q1,res,22,23,23
					rlwimi	q0,res,14,23,23
					rlwimi	q1,res,25,19,19
					rlwimi	q0,res,17,19,19
					rlwimi	q1,res,28,15,15
					rlwimi	q0,res,20,15,15
					rlwimi	q1,res,31,11,11
					rlwimi	q0,res,23,11,11
					rlwimi	q1,res,2,7,7
					rlwimi	q0,res,26,7,7
					rlwimi	q1,res,5,3,3
					rlwimi	q0,res,29,3,3

					lwz		rr,pal		
				}
				
				DRAWIX(q0,0);
				DRAWIX(q0,1);
				DRAWIX(q0,2);
				DRAWIX(q0,3);
				DRAWIX(q0,4);
				DRAWIX(q0,5);
				DRAWIX(q0,6);
				DRAWIX(q0,7);

				DRAWIX(q1,8);
				DRAWIX(q1,9);
				DRAWIX(q1,10);
				DRAWIX(q1,11);
				DRAWIX(q1,12);
				DRAWIX(q1,13);
				DRAWIX(q1,14);
				DRAWIX(q1,15);			
				
			}
		}
	}
	STBLTCount++;
	
	DSpContext_SwapBuffers(gContextRef,NULL,NULL);
	return ;
}

void UpdateScreenArea(i8  *STScreen,
                      int  x0,
                      int  y0,
                      int  width,
                      int  height,
                      i16 *pal,
                      bool paletteChanged,
                      i32 *pOldChecksum)
{
	if(gDSpScreenWidth == 640)
		return UpdateScreenArea640(STScreen,x0,y0,width,height,pal,paletteChanged,pOldChecksum);
	
	UpdateScreenArea1024(STScreen,x0,y0,width,height,pal,paletteChanged,pOldChecksum);
	
	
}

bool pc1, pc2;
void display (void)
{
  static i32 oldChecksumA;
  static i32 oldChecksumB;
  static i32 oldChecksumC;
  static i32 oldChecksumD;
  if (d.DynamicPaletteSwitching)
  {
    memcpy(palette1,&d.Palette11978, 32);
    memcpy(palette2,&d.Palette11946, 32);
    memcpy(globalPalette,&d.Palette11978,32);
  }
  else
  {
    memcpy(palette1,globalPalette, 32);
    memcpy(palette2,globalPalette, 32);
  };
  pc1 = HasPaletteChanged(palette1, oldPalette1);
  pc2 = HasPaletteChanged(palette2, oldPalette2);
  pc1 = pc1 || ForcedScreenDraw;
  pc2 = pc2 || ForcedScreenDraw;
  ForcedScreenDraw = false;
  {
 	UpdateScreenArea(         //Viewport
                     physbase(),//STScreen,
                     0,
                     0x21,
                     0xe0,
                     0xa9-0x21,
                     palette2,
                     pc2,
                     &oldChecksumA);
    UpdateScreenArea(   //Text scrolling area
                     physbase(),//STScreen,
                     0,
                     0xa9,
                     320,
                     0xc8-0xa9,
                     palette1,
                     pc1,
                     &oldChecksumB);
    UpdateScreenArea(             //portrait area
                     physbase(),//STScreen,
                     0,
                     0,
                     320,
                     0x21,
                     palette1,
                     pc1,
                     &oldChecksumC);

    UpdateScreenArea(              //spells,weapons,moves
                     physbase(),//STScreen,
                     0xe0,
                     0x21,
                     0x140-0xe0,
                     0xa9-0x21,
                     palette2,
                     pc2,
                     &oldChecksumD);
	
	
  };
}



void CopyViewportToScreen(void)
{
  aReg A0,A1;
  dReg D0;
   
  A0 = d.pViewportBMP;
  A1 = d.LogicalScreenBase+5280;
  for (D0W=0; D0W <136; D0W++)
  {
    memmove(A1,A0,112);
    A0 += 112;
    A1 += 160;
  };
}

void DebugDisplay(bool viewport)
{
  ForceScreenDraw();
  if (viewport) CopyViewportToScreen();
  display();
}
