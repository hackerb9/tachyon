/*
* Linux screen handling...
* I use SDL for for the graphics part,
* and Hermes to stretch the image
* (oddy enough SDL doesn't appear to
* have any stretch-blit functionality?)
* Anyway... SDL uses some pre-
* processor magic that requiers
* pragma pack NOT to be used,
* at least not pack(1),
* unfortunately CSBTypes.h defines pack(1),
* so SDL is included in stdafx.h, before
* we define any pragma packing.
*/

#include "stdafx.h"

#include "UI.h"
#include <stdio.h>

#include "Objects.h"
#include "Dispatch.h"
#include "CSB.h"
#include "Data.h"
#include "CSBTypes.h"


// Linux screen handling

extern SDL_Surface *WND;
extern i32 VBLMultiplier;
extern i16 globalPalette[16];
extern bool fullscreenRequested;
extern bool fullscreenActive;
ui32 STBLTCount=0;

extern i32 screenSize;
gboolean noDirtyRect = TRUE;
SDL_Rect DirtyRect;
i16 palette16[16];
i16 counter;
i32 dstLineLen;
i16 palette1[16];
i16 palette2[16];
i16 oldPalette1[16];
i16 oldPalette2[16];
pnt logbase(void);
#ifdef _DYN_WINSIZE
	i16 bitmap[320*4*200*4];
#	define wrBITMAP (ui32*)(bitmap)
	const int PALETTE_RED_BITS = 16;
	const int PALETTE_GREEN_BITS = 8;
	const int PALETTE_BLUE_BITS = 0;
#else // not _DUN_WINSIZE
	extern SDL_Surface * SCRAP;
#	define wrBITMAP (ui32*)(SCRAP->pixels)
	const int PALETTE_RED_BITS = 0;
	const int PALETTE_GREEN_BITS = 8;
	const int PALETTE_BLUE_BITS = 16; 
	static inline ui16*__wScrM(ui16 *dest, const ui16 ColAB) {
		unsigned int i;
		ui8 *ptr = (ui8*) dest;
		const ui8 A = ColAB >> 8;
		const ui8 B = ColAB & 255;
		for(i=0;i<screenSize;i++) {
			*(ptr++) = B;
		}
		for(i=0;i<screenSize;i++) {
			*(ptr++) = A;
		}
		return (ui16*) ptr;
	}
#endif

bool ForcedScreenDraw = false;
void ForceScreenDraw(void)
{
  ForcedScreenDraw = true;
}

bool IsTextScrollArea(i32, i32)
{
  return false;
}

void SwapTextZOrder(void)
{
  return;
}

static void createPalette32( i32 * palette32, i16 *palette);
static void mkDirty(i32 x, i32 y, i32 w, i32 h) {
	if( noDirtyRect ) {
		DirtyRect.x = x; //CLAMP(x,0,320);
		DirtyRect.y = y; // CLAMP(y,0,200);
		DirtyRect.w = w; // CLAMP(w,0,320-DirtyRect.x);
		DirtyRect.h = h; // CLAMP(h,0,200-DirtyRect.y);
		noDirtyRect = FALSE;
	} else {
		i32 id_st_mouseXp = x + w;
		i32 id_st_mouseYp = y + h;
		i32 oldW = DirtyRect.x + DirtyRect.w;
		i32 oldH = DirtyRect.y + DirtyRect.h;
		DirtyRect.x = x <= DirtyRect.x ? x : DirtyRect.x ;
		DirtyRect.y = y <= DirtyRect.y ? y : DirtyRect.y ;
		DirtyRect.w = id_st_mouseXp >= oldW ? id_st_mouseXp - DirtyRect.x : oldW - DirtyRect.x ;
		DirtyRect.h = id_st_mouseYp >= oldH ? id_st_mouseYp - DirtyRect.y : oldH - DirtyRect.y ;
	}
}
static void Well___SDL_must_not_be_included_with_pragma_pack(bool updatePalette) {
	const SDL_VideoInfo *info = SDL_GetVideoInfo();
#ifdef _DYN_WINSIZE
	if(updatePalette) {
		i32 *pal = Hermes_PaletteGet(from_palette);
		createPalette32( pal, palette1);
		//SDL_SetPalette(SCR, SDL_LOGPAL|SDL_PHYSPAL, palette32, 0, 16 );
		createPalette32( &pal[0x10], palette2);
		//SDL_SetPalette(SCR, SDL_LOGPAL|SDL_PHYSPAL, palette32, 16, 16 );
		Hermes_PaletteInvalidateCache(from_palette);
	}

	/*
	* According to Hermes documentation, this should take no time...
	*/
	HermesFormat to;
	to.has_colorkey = FALSE;
	to.indexed = (NULL != info->vfmt->palette);
	to.bits = info->vfmt->BitsPerPixel;
	to.r = info->vfmt->Rmask;
	to.g = info->vfmt->Gmask;
	to.b = info->vfmt->Bmask;
	to.a = info->vfmt->Amask;
	Hermes_ConverterRequest( converter, from_format, &to );
	if(updatePalette) {
		Hermes_ConverterPalette( converter, from_palette, to.indexed ? to_palette : NULL );
		if(to.indexed) Hermes_PaletteSet( to_palette, (i32*) info->vfmt->palette ); // FIXME: Little endian will not work here?
	}
#else
	if(updatePalette) {
		i32 pal[0x21];
		createPalette32( pal, palette1);
		createPalette32( &pal[0x10], palette2);
		SDL_SetPalette(SCRAP,SDL_LOGPAL|SDL_PHYSPAL,reinterpret_cast<SDL_Color*>(pal),0,0x20);
    }
#endif

	/*
	* And stretch it...
	*/
	SDL_Rect rect;
	SDL_GetClipRect(WND, &rect );
	const ui32 scZi = rect.w * info->vfmt->BytesPerPixel;
	i32 x,y,w,h;
	if(updatePalette) {
		x = rect.x;
		y = rect.y;
		w = rect.w;
		h = rect.h;
	} else {
		if( noDirtyRect ) {
			return;
		} else {
#ifdef _DYN_WINSIZE
			x = ((float) (DirtyRect.x - 160) ) / st_X + ((float)rect.w)/2.0 + 0.5;
			y = ((float) (DirtyRect.y - 100) ) / st_Y + ((float)rect.h)/2.0 + 0.5;
			w = ((float) (DirtyRect.w) ) / st_X + 0.5;
			h = ((float) (DirtyRect.h) ) / st_Y + 0.5;
#else
			x = (DirtyRect.x - 160) * screenSize + rect.w/2;
			y = (DirtyRect.y - 100) * screenSize + rect.y/2;
			w = DirtyRect.w * screenSize;
			h = DirtyRect.h * screenSize;
		x = rect.x;
		y = rect.y;
		w = rect.w;
		h = rect.h;
	SDL_Rect moffaPjopp = DirtyRect;
	moffaPjopp.x *= screenSize;
	moffaPjopp.y *= screenSize;
	moffaPjopp.w *= screenSize;
	moffaPjopp.h *= screenSize;
		x = moffaPjopp.x;
		y = moffaPjopp.y;
		w = moffaPjopp.w;
		h = moffaPjopp.h;
#endif
			x = CLAMP(x,0,rect.w);
			y = CLAMP(y,0,rect.h);
			w = CLAMP(w,0,rect.w-x);
			h = CLAMP(h,0,rect.h-y);
		}
	}
	noDirtyRect = TRUE;
#ifdef _DYN_WINSIZE
	SDL_LockSurface(WND);
	Hermes_ConverterCopy( converter, (ui32*) bitmap, 0, 0, 320, 200, 320, WND->pixels, rect.x, rect.y, rect.w, rect.h, rect.w * info->vfmt->BytesPerPixel );
//	Hermes_ConverterCopy( converter, (ui32*) bitmap, DirtyRect.x, DirtyRect.y, DirtyRect.w, DirtyRect.h, 320, WND->pixels, x, y, w, h, scZi );
	SDL_UnlockSurface(WND);
#else
	SDL_Rect moffaPjopp = DirtyRect;
	moffaPjopp.x *= screenSize;
	moffaPjopp.y *= screenSize;
	moffaPjopp.w *= screenSize;
	moffaPjopp.h *= screenSize;
	SDL_BlitSurface(SCRAP,&moffaPjopp,WND,&moffaPjopp);
#endif
	SDL_UpdateRect(WND,x, y, w, h);
}

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

bool HasScreenChanged(i8 *STScreen,    // First byte to test
                      i32 lineLen,     // quadwords
                      i32 numLine,
                      i32 lineIncrement,
                      i32 *pOldChecksum)
{
    // Note, this solution is NOT reentrant, but I'm not sure how to write GAS code. 
   int result;
   printf("HasScreenChanged\n");
   /* __asm__ (
    "subl %%edx,%%edx              \n\t" // The summation
    "subl %%ebx,%%ebx              \n\t"
//    "movl %%[STScreen], %%esi        \n\t"
//    "movl %%[numLine], %%edi         \n\t" //
"do1line:                    \n\t"
    "movl lineLen, %%ecx         \n\t"
    "cmpl $4,%%ecx                \n\t"
"do16Bytes:                  \n\t"
    "jc do4Bytes                 \n\t"
    "lodsl                       \n\t"
    "addl %%eax,%%edx              \n\t"
    "lodsl                       \n\t"
    "addl %%eax,%%ebx              \n\t"
    "lodsl                       \n\t"
    "addl %%eax,%%edx              \n\t"
    "lodsl                       \n\t"
    "addl %%eax,%%ebx              \n\t"
    "subl $4,%%ecx                \n\t"
    "cmpl $4,%%ecx                \n\t"
    "jnc do16Bytes               \n\t"
    "orw %%cx,%%cx                 \n\t"
    "jz  lineDone                \n\t"
"do4Bytes:                   \n\t"
    "roll $1,%%edx                \n\t"
    "lodsl                       \n\t"
    //add ebx,eax
    //ror ebx,1
    "addl %%eax,%%edx              \n\t"
    "loop do4Bytes               \n\t"
"lineDone:                   \n\t"
    "rorl $1,%%edx                \n\t"
    "addl lineIncrement, %%esi   \n\t"
    "decl %%edi                   \n\t"
    "jnz do1line                 \n\t"
    "roll $1,%%ebx                \n\t"
    "addl %%ebx,%%edx              \n\t"
    "movl pOldChecksum, %%esi    \n\t"
    "movl $0,%%eax				\n\t" // out=eax
    "cmpl (%%esi),%%edx            \n\t"
    "jz  ret0                    \n\t"
    "movl $1,%%eax				\n\t" // out=eax
"ret0:                       \n\t"
    "movl %%edx,(%%esi)            \n\t"

     :"=a" (result) // FIXASM: output regs/vars go here, e.g.:  "=m" (memory_var)

     : "S" (*STScreen), "D" (numLine) // FIXASM: input regs, e.g.:  "c" (count), "S" (src), "D" (dest)

     : "ebx","ecx","edx","cc", "memory" // FIXASM: clobber list, e.g.:  "%eax", "%ecx", "cc"
  );*/
  return result;
}

void STBlt(i8 *STScreen, // first 16-pixel group
	   ui16 *destination,
           i32 lineLen,  // # 16-pixel groups
           i32 numLine,
           i32 lineIncrement)
{printf("STBlt\n");/*
    __asm__ (
    "movl STScreen, %%esi        \n\t"
    "movl destination, %%edi          \n\t"
"do2line:                    \n\t"
    "movl lineLen, %%eax         \n\t"
    "movw %%ax, counter          \n\t"
"do16bits:                   \n\t"
    "movb 6(%%esi),%%ah            \n\t"
    "movb 4(%%esi),%%al            \n\t"
    "movb 2(%%esi),%%dh            \n\t"
    "movb 0(%%esi),%%dl            \n\t"
    "movw $8,%%cx                 \n\t"
"doTop8bits:                 \n\t"
    "subl %%ebx,%%ebx              \n\t"
    "rclb $1,%%ah                 \n\t"
    "rclb $1,%%bl                 \n\t"
    "rclb $1,%%al                 \n\t"
    "rclb $1,%%bl                 \n\t"
    "rclb $1,%%dh                 \n\t"
    "rclb $1,%%bl                 \n\t"
    "rclb $1,%%dl                 \n\t"
    "rclb $1,%%bl                 \n\t"
    "movw palette16(,%%ebx,2), %%bx \n\t"
    "movw %%bx,(%%edi)             \n\t"
    "addl $2,%%edi                \n\t"
    "decw %%cx                    \n\t"
    "jnz doTop8bits              \n\t"
    "movb 7(%%esi),%%ah            \n\t"
    "movb 5(%%esi),%%al            \n\t"
    "movb 3(%%esi),%%dh            \n\t"
    "movb 1(%%esi),%%dl            \n\t"
    "movw $8,%%cx                 \n\t"
"doBottom8bits:              \n\t"
    "subl %%ebx,%%ebx              \n\t"
    "rclb $1,%%ah                 \n\t"
    "rclb $1,%%bl                 \n\t"
    "rclb $1,%%al                 \n\t"
    "rclb $1,%%bl                 \n\t"
    "rclb $1,%%dh                 \n\t"
    "rclb $1,%%bl                 \n\t"
    "rclb $1,%%dl                 \n\t"
    "rclb $1,%%bl                 \n\t"
    "movw palette16(,%%ebx,2), %%bx \n\t"
//    mov ebx,0000ff00h
    "movw %%bx,(%%edi)             \n\t"
    "addl $2,%%edi                \n\t"
    "decw %%cx                    \n\t"
    "jnz doBottom8bits           \n\t"
    "addl $8,%%esi                \n\t"
    "decw counter					\n\t"
    "jnz do16bits                \n\t"
    "addl lineIncrement, %%esi   \n\t"
    "decw numLine					\n\t"
    "jnz do2Line                 \n\t"

     : // FIXASM: output regs/vars go here, e.g.:  "=m" (memory_var)

     :  // FIXASM: input regs, e.g.:  "c" (count), "S" (src), "D" (dest)

     : "eax","ebx","ecx","edx","edi","esi","cc", "memory" // FIXASM: clobber list, e.g.:  "%eax", "%ecx", "cc"
  );*/
}

void STBlt2(i8 *STScreen, // first 16-pixel group
	    ui16 *destination,
            i32 lineLen,  // # 16-pixel groups
            i32 numLine,
            i32 lineIncrement)
{printf("STBlt2\n");
    /*__asm__ (
    "movl STScreen, %%esi        \n\t"
    "movl destination, %%edi          \n\t"
"do3line:                    \n\t"
    "movl lineLen, %%eax         \n\t"
    "movw %%ax, counter          \n\t"
    "shll $6,%%eax                \n\t" //# 16 pixels/group and 4 bytes per pixel
    "movl %%eax, dstLineLen      \n\t"
"do16bits_2:                   \n\t"
    "movb 6(%%esi),%%ah            \n\t"
    "movb 4(%%esi),%%al            \n\t"
    "movb 2(%%esi),%%dh            \n\t"
    "movb 0(%%esi),%%dl            \n\t"
    "movw $8,%%cx                 \n\t"
    "pushl %%esi                  \n\t"
    "movl dstLineLen, %%esi      \n\t"
"doTop8bits_2:                 \n\t"
    "subl %%ebx,%%ebx              \n\t"
    "rclb $1,%%ah                 \n\t"
    "rclb $1,%%bl                 \n\t"
    "rclb $1,%%al                 \n\t"
    "rclb $1,%%bl                 \n\t"
    "rclb $1,%%dh                 \n\t"
    "rclb $1,%%bl                 \n\t"
    "rclb $1,%%dl                 \n\t"
    "rclb $1,%%bl                 \n\t"
    "movw palette16(,%%ebx,2), %%bx \n\t"
    "movw %%bx,(%%edi)             \n\t"
    "movw %%bx,(%%edi,%%esi)        \n\t"
    "addl $2,%%edi                \n\t"
    "movw %%bx,(%%edi)             \n\t"
    "movw %%bx,0(%%edi,%%esi,1)        \n\t"
    "addl $2,%%edi                \n\t"
    "decw %%cx                    \n\t"
    "jnz doTop8bits_2              \n\t"
    "popl %%esi                   \n\t"
    "movb 7(%%esi),%%ah            \n\t"
    "movb 5(%%esi),%%al            \n\t"
    "movb 3(%%esi),%%dh            \n\t"
    "movb 1(%%esi),%%dl            \n\t"
    "movw $8,%%cx                 \n\t"
    "pushl %%esi                  \n\t"
    "movl dstLineLen, %%esi      \n\t"
"doBottom8bits_2:              \n\t"
    "subl %%ebx,%%ebx              \n\t"
    "rclb $1,%%ah                 \n\t"
    "rclb $1,%%bl                 \n\t"
    "rclb $1,%%al                 \n\t"
    "rclb $1,%%bl                 \n\t"
    "rclb $1,%%dh                 \n\t"
    "rclb $1,%%bl                 \n\t"
    "rclb $1,%%dl                 \n\t"
    "rclb $1,%%bl                 \n\t"
    "movw palette16(,%%ebx,2), %%bx \n\t"
    "movw %%bx,(%%edi)             \n\t"
    "movw %%bx,(%%edi,%%esi)        \n\t"
    "addl $2,%%edi                \n\t"
    "movw %%bx,(%%edi)             \n\t"
    "movw %%bx,(%%edi,%%esi)        \n\t"
    "addl $2,%%edi                \n\t"
    "decw %%cx                    \n\t"
    "jnz doBottom8bits_2           \n\t"
    "popl %%esi                   \n\t"
    "addl $8,%%esi                \n\t"
    "decw counter					\n\t"
    "jnz do16bits_2                \n\t"
    "addl lineIncrement, %%esi   \n\t"
    "addl dstLineLen, %%edi      \n\t"
    "decw numLine					\n\t"
    "jnz do3Line                 \n\t"

     : // FIXASM: output regs/vars go here, e.g.:  "=m" (memory_var)

     : // FIXASM: input regs, e.g.:  "c" (count), "S" (src), "D" (dest)

     : "eax","ebx","ecx","edx","edi","esi","cc", "memory" // FIXASM: clobber list, e.g.:  "%%eax", "%%ecx", "cc"
  );*/
}

void STBlt3(i8 *STScreen, // first 16-pixel group
	    ui16 *destination,
            i32 lineLen,  // # 16-pixel groups
            i32 numLine,
            i32 lineIncrement)
{printf("STBlt3\n");
  /*  __asm__ (
    "movl STScreen, %%esi        \n\t"
    "movl destination, %%edi          \n\t"
"do4line:                    \n\t"
    "movl lineLen, %%eax         \n\t"
    "movw %%ax, counter          \n\t"
    "imull $16*2*3,%%eax          \n\t" //# 16 pixels per group * 2 bytes per pixel * 3 pixels per pixel
    "movl %%eax, dstLineLen      \n\t"
"do16bits_3:                   \n\t"
    "movb 6(%%esi),%%ah            \n\t"
    "movb 4(%%esi),%%al            \n\t"
    "movb 2(%%esi),%%dh            \n\t"
    "movb 0(%%esi),%%dl            \n\t"
    "movw $8,%%cx                 \n\t"
    "pushl %%esi                  \n\t"
    "movl dstLineLen, %%esi      \n\t"
"doTop8bits_3:                 \n\t"
    "subl %%ebx,%%ebx              \n\t"
    "rclb $1,%%ah                 \n\t"
    "rclb $1,%%bl                 \n\t"
    "rclb $1,%%al                 \n\t"
    "rclb $1,%%bl                 \n\t"
    "rclb $1,%%dh                 \n\t"
    "rclb $1,%%bl                 \n\t"
    "rclb $1,%%dl                 \n\t"
    "rclb $1,%%bl                 \n\t"
    "movw palette16(,%%ebx,2), %%bx \n\t"
    "movw %%bx,(%%edi)             \n\t"
    "movw %%bx,(%%edi,%%esi)        \n\t"
    "movw %%bx,(%%edi,%%esi,2)      \n\t"
    "addl $2,%%edi                \n\t"
    "movw %%bx,(%%edi)             \n\t"
    "movw %%bx,(%%edi,%%esi)        \n\t"
    "movw %%bx,(%%edi,%%esi,2)      \n\t"
    "addl $2,%%edi                \n\t"
    "movw %%bx,(%%edi)             \n\t"
    "movw %%bx,(%%edi,%%esi)        \n\t"
    "movw %%bx,(%%edi,%%esi,2)      \n\t"
    "addl $2,%%edi                \n\t"
    "decw %%cx                    \n\t"
    "jnz doTop8bits_3              \n\t"
    "popl %%esi                   \n\t"
    "movb 7(%%esi),%%ah            \n\t"
    "movb 5(%%esi),%%al            \n\t"
    "movb 3(%%esi),%%dh            \n\t"
    "movb 1(%%esi),%%dl            \n\t"
    "movw $8,%%cx                 \n\t"
    "pushl %%esi                  \n\t"
    "movl dstLineLen, %%esi      \n\t"
"doBottom8bits_3:              \n\t"
    "subl %%ebx,%%ebx              \n\t"
    "rclb $1,%%ah                 \n\t"
    "rclb $1,%%bl                 \n\t"
    "rclb $1,%%al                 \n\t"
    "rclb $1,%%bl                 \n\t"
    "rclb $1,%%dh                 \n\t"
    "rclb $1,%%bl                 \n\t"
    "rclb $1,%%dl                 \n\t"
    "rclb $1,%%bl                 \n\t"
    "movw palette16(,%%ebx,2), %%bx \n\t"
    "movw %%bx,(%%edi)             \n\t"
    "movw %%bx,(%%edi,%%esi)        \n\t"
    "movw %%bx,(%%edi,%%esi,2)      \n\t"
    "addl $2,%%edi                \n\t"
    "movw %%bx,(%%edi)             \n\t"
    "movw %%bx,(%%edi,%%esi)        \n\t"
    "movw %%bx,(%%edi,%%esi,2)      \n\t"
    "addl $2,%%edi                \n\t"
    "movw %%bx,(%%edi)             \n\t"
    "movw %%bx,(%%edi,%%esi)        \n\t"
    "movw %%bx,(%%edi,%%esi,2)      \n\t"
    "addl $2,%%edi                \n\t"
    "decw %%cx                    \n\t"
    "jnz doBottom8bits_3           \n\t"
    "popl %%esi                   \n\t"
    "addl $8,%%esi                \n\t"
    "decw counter					\n\t"
    "jnz do16bits_3                \n\t"
    "addl lineIncrement, %%esi   \n\t"
    "addl dstLineLen, %%edi      \n\t"
    "addl dstLineLen, %%edi      \n\t"
    "decw numLine					\n\t"
    "jnz do4Line                 \n\t"

     : // FIXASM: output regs/vars go here, e.g.:  "=m" (memory_var)

     : // FIXASM: input regs, e.g.:  "c" (count), "S" (src), "D" (dest)

     : "eax","ebx","ecx","edx","edi","esi","cc", "memory" // FIXASM: clobber list, e.g.:  "%%eax", "%%ecx", "cc"
  );*/
}

void STBlt4(i8 *STScreen, // first 16-pixel group
	    ui16 *destination,
            i32 lineLen,  // # 16-pixel groups
            i32 numLine,
            i32 lineIncrement)
{printf("STBlt4\n");
   /* __asm__ (
    "movl STScreen, %%esi        \n\t"
    "movl destination, %%edi          \n\t"
"do5line:                    \n\t"
    "movl lineLen, %%eax         \n\t"
    "movw %%ax, counter          \n\t"
    "imull $16*2*4,%%eax          \n\t" //# 16 pixels per group * 2 bytes per pixel * 4 pixels per pixel
    "movl %%eax, dstLineLen      \n\t"
"do16bits_4:                   \n\t"
    "movb 6(%%esi),%%ah            \n\t"
    "movb 4(%%esi),%%al            \n\t"
    "movb 2(%%esi),%%dh            \n\t"
    "movb 0(%%esi),%%dl            \n\t"
    "movw $8,%%cx                 \n\t"
    "pushl %%esi                  \n\t"
    "movl dstLineLen, %%esi      \n\t"
"doTop8bits_4:                 \n\t"
    "subl %%ebx,%%ebx              \n\t"
    "rclb $1,%%ah                 \n\t"
    "rclb $1,%%bl                 \n\t"
    "rclb $1,%%al                 \n\t"
    "rclb $1,%%bl                 \n\t"
    "rclb $1,%%dh                 \n\t"
    "rclb $1,%%bl                 \n\t"
    "rclb $1,%%dl                 \n\t"
    "rclb $1,%%bl                 \n\t"
    "movw palette16(,%%ebx,2), %%bx \n\t"
    "movw %%bx,(%%edi)             \n\t"
    "movw %%bx,(%%edi,%%esi)        \n\t"
    "movw %%bx,(%%edi,%%esi,2)      \n\t"
    "addl %%esi,%%edi              \n\t"
    "movw %%bx,(%%edi,%%esi,2)      \n\t"
    "subl %%esi,%%edi              \n\t"
    "addl $2,%%edi                \n\t"
    "movw %%bx,(%%edi)             \n\t"
    "movw %%bx,(%%edi,%%esi)        \n\t"
    "movw %%bx,(%%edi,%%esi,2)      \n\t"
    "addl %%esi,%%edi              \n\t"
    "movw %%bx,(%%edi,%%esi,2)      \n\t"
    "subl %%esi,%%edi              \n\t"
    "addl $2,%%edi                \n\t"
    "movw %%bx,(%%edi)             \n\t"
    "movw %%bx,(%%edi,%%esi)        \n\t"
    "movw %%bx,(%%edi,%%esi,2)      \n\t"
    "addl %%esi,%%edi              \n\t"
    "movw %%bx,(%%edi,%%esi,2)      \n\t"
    "subl %%esi,%%edi              \n\t"
    "addl $2,%%edi                \n\t"
    "movw %%bx,(%%edi)             \n\t"
    "movw %%bx,(%%edi,%%esi)        \n\t"
    "movw %%bx,(%%edi,%%esi,2)      \n\t"
    "addl %%esi,%%edi              \n\t"
    "movw %%bx,(%%edi,%%esi,2)      \n\t"
    "subl %%esi,%%edi              \n\t"
    "addl $2,%%edi                \n\t"
    "decw %%cx                    \n\t"
    "jnz doTop8bits_4              \n\t"
    "popl %%esi                   \n\t"
    "movb 7(%%esi),%%ah            \n\t"
    "movb 5(%%esi),%%al            \n\t"
    "movb 3(%%esi),%%dh            \n\t"
    "movb 1(%%esi),%%dl            \n\t"
    "movw $8,%%cx                 \n\t"
    "pushl %%esi                  \n\t"
    "movl dstLineLen, %%esi      \n\t"
"doBottom8bits_4:              \n\t"
    "subl %%ebx,%%ebx              \n\t"
    "rclb $1,%%ah                 \n\t"
    "rclb $1,%%bl                 \n\t"
    "rclb $1,%%al                 \n\t"
    "rclb $1,%%bl                 \n\t"
    "rclb $1,%%dh                 \n\t"
    "rclb $1,%%bl                 \n\t"
    "rclb $1,%%dl                 \n\t"
    "rclb $1,%%bl                 \n\t"
    "movw palette16(,%%ebx,2), %%bx \n\t"
    "movw %%bx,(%%edi)             \n\t"
    "movw %%bx,(%%edi,%%esi)        \n\t"
    "movw %%bx,(%%edi,%%esi,2)      \n\t"
    "addl %%esi,%%edi              \n\t"
    "movw %%bx,(%%edi,%%esi,2)      \n\t"
    "subl %%esi,%%edi              \n\t"
    "addl $2,%%edi                \n\t"
    "movw %%bx,(%%edi)             \n\t"
    "movw %%bx,(%%edi,%%esi)        \n\t"
    "movw %%bx,(%%edi,%%esi,2)      \n\t"
    "addl %%esi,%%edi              \n\t"
    "movw %%bx,(%%edi,%%esi,2)      \n\t"
    "subl %%esi,%%edi              \n\t"
    "addl $2,%%edi                \n\t"
    "movw %%bx,(%%edi)             \n\t"
    "movw %%bx,(%%edi,%%esi)        \n\t"
    "movw %%bx,(%%edi,%%esi,2)      \n\t"
    "addl %%esi,%%edi              \n\t"
    "movw %%bx,(%%edi,%%esi,2)      \n\t"
    "subl %%esi,%%edi              \n\t"
    "addl $2,%%edi                \n\t"
    "movw %%bx,(%%edi)             \n\t"
    "movw %%bx,(%%edi,%%esi)        \n\t"
    "movw %%bx,(%%edi,%%esi,2)      \n\t"
    "addl %%esi,%%edi              \n\t"
    "movw %%bx,(%%edi,%%esi,2)      \n\t"
    "subl %%esi,%%edi              \n\t"
    "addl $2,%%edi                \n\t"
    "decw %%cx                    \n\t"
    "jnz doBottom8bits_4           \n\t"
    "popl %%esi                   \n\t"
    "addl $8,%%esi                \n\t"
    "decw counter					\n\t"
    "jnz do16bits_4                \n\t"
    "addl lineIncrement, %%esi   \n\t"
    "addl dstLineLen, %%edi      \n\t"
    "addl dstLineLen, %%edi      \n\t"
    "addl dstLineLen, %%edi      \n\t"
    "decw numLine					\n\t"
    "jnz do5Line                 \n\t"

     : // FIXASM: output regs/vars go here, e.g.:  "=m" (memory_var)

     : // FIXASM: input regs, e.g.:  "c" (count), "S" (src), "D" (dest)

     : "eax","ebx","ecx","edx","edi","esi","cc", "memory" // FIXASM: clobber list, e.g.:  "%%eax", "%%ecx", "cc"
  );*/
}



static void createPalette32( i32 * palette32, i16 *palette)
{ // Create 32-bit color palette from the ST 9-bit palette
  for (i32 i=0; i<16; i++)
  {
    ASSERT(palette[i]<=0x777, "no text");
 //   palette32[i].r = (((palette[i]>>8) & 7)*0xff/0x7); //<<16;
 //   palette32[i].g = (((palette[i]>>4) & 7)*0xff/0x7); //<< 8;
 //   palette32[i].b = (((palette[i]>>0) & 7)*0xff/0x7); //<< 0;
   palette32[i] = (
		(((palette[i]>>8) & 7)*(0xff/0x7))<< PALETTE_RED_BITS // Red
	  ) | (
		(((palette[i]>>4) & 7)*0xff/0x7)<< PALETTE_GREEN_BITS  // Green
	  ) | (
		(((palette[i]>>0) & 7)*0xff/0x7)<< PALETTE_BLUE_BITS  // Blue
	  );
  }
}


void UpdateScreenArea(
                      ui8  *STScreen,
		      ui32 *destination,
                      i32  x0,
                      i32  y0,
                      i32  width,
                      i32  height,
                      ui16 palette,
                      bool paletteChanged,
                      i32 *pOldChecksum)
{
	/*if(!paletteChanged) if(!noDirtyRect) {
		i32 y1 = (y0 < DirtyRect.y) ? DirtyRect.y : y0 ;
		i32 x1 = (x0 < DirtyRect.x) ? DirtyRect.x : x0 ;
		i32 ih = y1 + height - DirtyRect.h - DirtyRect.y ;
		i32 iw = x1 + width - DirtyRect.w - DirtyRect.x ;
		height -= ih;
		width -= iw;
		x0 = x1;
		y0 = y1;
		if( width < 0 || height < 0 ) return;
		paletteChanged = true;
	}*/
  const i32 LineStart = (x0/16) * 8;
  const i32 LineEnd   = ((x0+width+15)/16) * 8;
#ifdef _DYN_WINSIZE
  ui16 *dest = (ui16*) ((char*)destination) + 160*y0 + LineStart;
#else
  ui16 *dest = ((ui16*)destination) + 160*y0*screenSize*screenSize + LineStart*screenSize;
#endif
  ui16 *scr = (ui16*) (STScreen + 160*y0 + LineStart);
  const int increment = 160-(LineEnd-LineStart);
  if (!paletteChanged)
  {
/*    if (!HasScreenChanged(STScreen+160*y0+LineStart,
                          (LineEnd-LineStart) / 4,
                          height,
                          160-(LineEnd-LineStart),
                          pOldChecksum)) return;*/
	i32 chk = 0;
	int h = height;
	ui32 *CC = (ui32*) scr;
	while( h-- > 0 ) {
		int lineCnt = (LineEnd-LineStart)/2;
		while( lineCnt-- > 0 ) {
			chk += *(CC++);
		}
		CC = (ui32*) (((char*)CC) + increment);
	}
	bool huh = (chk == *pOldChecksum);
	*pOldChecksum = chk;
	if(huh) return;
  };
  mkDirty(x0,y0,width,height);
/* This is not the most efficient solution, but it is portable. :o) */
  while( height-- > 0 ) 
  {
#ifndef _DYN_WINSIZE
    ui16 *oldScr = scr;
    unsigned int bongo = screenSize;
    while(bongo-->0) 
    {
      scr = oldScr;
#endif
      int lineCnt = (LineEnd-LineStart)/8;
      while( lineCnt-- > 0 ) 
      {
	/* Read the four ST planes. */
	ui16 _one = *(scr++); ui16 four = GUINT16_FROM_BE(_one);
	ui16 _two = *(scr++); ui16 three = GUINT16_FROM_BE(_two);
	ui16 _three=*(scr++); ui16 two = GUINT16_FROM_BE(_three);
	ui16 _four= *(scr++); ui16 one = GUINT16_FROM_BE(_four);
	/* Get pixel# 0, 4, 8, 12 */
	ui16 c1 = ((one>>0) & 0x8888) | ((two>>1) & 0x4444) | ((three>>2) & 0x2222) | ((four>>3) & 0x1111);
	/* Get pixel# 1, 5, 9, 13 */
	ui16 c2 = ((one<<1) & 0x8888) | ((two>>0) & 0x4444) | ((three>>1) & 0x2222) | ((four>>2) & 0x1111);
	/* Get pixel# 2, 6, 10, 14 */
	ui16 c3 = ((one<<2) & 0x8888) | ((two<<1) & 0x4444) | ((three>>0) & 0x2222) | ((four>>1) & 0x1111);
	/* Get pixel# 3, 7, 11, 15 */
	ui16 c4 = ((one<<3) & 0x8888) | ((two<<2) & 0x4444) | ((three<<1) & 0x2222) | ((four>>0) & 0x1111);

	ui32 C1 = c1;
	ui32 C2 = c2;
	ui32 C3 = c3;
	ui32 C4 = c4;

	/* This out-commented code should behave exactly like the code below it (if dest is a ui32*), but it doesn't... */
	/* Write pixel# 0, 1, 2, 3 */
//		ui32 Col1 = (C1<<12)&0x0F000000 | (C2<<4)&0x000F0000 | (C3>>4)&0x00000F00 | (C4>>12)&0x0000000F | palette; *(dest++) = GUINT32_FROM_BE(Col1);
	/* Write pixel# 4, 5, 6, 7 */
//		ui32 Col2 = (C1<<16)&0x0F000000 | (C2<<8)&0x000F0000 | (C3>>0)&0x00000F00 | (C4>>8)&0x0000000F | palette;  *(dest++) = GUINT32_FROM_BE(Col2);
	/* Write pixel# 8, 9, 10, 11 */
//		ui32 Col3 = (C1<<20)&0x0F000000 | (C2<<10)&0x000F0000 | (C3<<4)&0x00000F00 | (C4>>4)&0x0000000F | palette;  *(dest++) = GUINT32_FROM_BE(Col3);
	/* Write pixel# 12, 13, 14, 15 */
//		ui32 Col4 = (C1<<24)&0x0F000000 | (C2<<14)&0x000F0000 | (C3<<8)&0x00000F00 | (C4>>0)&0x0000000F | palette;  *(dest++) = GUINT32_FROM_BE(Col4);

#ifdef _DYN_WINSIZE
	/* Write pixel# 0, 1, 2, 3 */
	ui16 Col1 = (c1>>4)&0x0F00 | (c2>>12)&0x000F | palette; *(dest++) = GUINT16_FROM_BE(Col1);
	ui16 Col2 = (c3>>4)&0x0F00 | (c4>>12)&0x000F | palette; *(dest++) = GUINT16_FROM_BE(Col2);
	/* Write pixel# 4, 5, 6, 7 */
	ui16 Col3 = (c1>>0)&0x0F00 | (c2>>8)&0x000F | palette; *(dest++) = GUINT16_FROM_BE(Col3);
	ui16 Col4 = (c3>>0)&0x0F00 | (c4>>8)&0x000F | palette; *(dest++) = GUINT16_FROM_BE(Col4);
	/* Write pixel# 8, 9, 10, 11 */
	ui16 Col5 = (c1<<4)&0x0F00 | (c2>>4)&0x000F | palette; *(dest++) = GUINT16_FROM_BE(Col5);
	ui16 Col6 = (c3<<4)&0x0F00 | (c4>>4)&0x000F | palette; *(dest++) = GUINT16_FROM_BE(Col6);
	/* Write pixel# 12, 13, 14, 15 */
	ui16 Col7 = (c1<<8)&0x0F00 | (c2<<0)&0x000F | palette; *(dest++) = GUINT16_FROM_BE(Col7);
		ui16 Col8 = (c3<<8)&0x0F00 | (c4<<0)&0x000F | palette; *(dest++) = GUINT16_FROM_BE(Col8);
      }; //lineCnt
      scr = (ui16*) (  ((ui8*)scr) + increment  );
      dest = (ui16*) (   ((ui8*)dest) + increment*2 );
#else
        /* Write pixel# 0, 1, 2, 3 */
        ui16 Col1 = (c1>>4)&0x0F00 | (c2>>12)&0x000F | palette; dest = __wScrM(dest, GUINT16_FROM_BE(Col1));
        ui16 Col2 = (c3>>4)&0x0F00 | (c4>>12)&0x000F | palette; dest = __wScrM(dest, GUINT16_FROM_BE(Col2));
        /* Write pixel# 4, 5, 6, 7 */
        ui16 Col3 = (c1>>0)&0x0F00 | (c2>>8)&0x000F | palette; dest = __wScrM(dest, GUINT16_FROM_BE(Col3));
        ui16 Col4 = (c3>>0)&0x0F00 | (c4>>8)&0x000F | palette; dest = __wScrM(dest, GUINT16_FROM_BE(Col4));
        /* Write pixel# 8, 9, 10, 11 */
        ui16 Col5 = (c1<<4)&0x0F00 | (c2>>4)&0x000F | palette; dest = __wScrM(dest, GUINT16_FROM_BE(Col5));
        ui16 Col6 = (c3<<4)&0x0F00 | (c4>>4)&0x000F | palette; dest = __wScrM(dest, GUINT16_FROM_BE(Col6));
        /* Write pixel# 12, 13, 14, 15 */
        ui16 Col7 = (c1<<8)&0x0F00 | (c2<<0)&0x000F | palette; dest = __wScrM(dest, GUINT16_FROM_BE(Col7));
        ui16 Col8 = (c3<<8)&0x0F00 | (c4<<0)&0x000F | palette; dest = __wScrM(dest, GUINT16_FROM_BE(Col8));
      }; // lineCnt
      scr = (ui16*) (  ((ui8*)scr) + increment);
      dest = dest + increment * screenSize;
    }; //bongo
#endif
  }; //height
  STBLTCount++;
};




bool pc1, pc2;
void display (void){
  static i32 oldChecksumA;
  static i32 oldChecksumB;
  static i32 oldChecksumC;
  static i32 oldChecksumD;
  static int numDisplay = 0;
  numDisplay++;
  if ((VBLMultiplier!=1) && ((d.Time&0xf)!=0)) return;
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

/*  static ui32 CCCP[8000];
  ui32 * ptr = (ui32*) physbase();
  ui32 * cck = CCCP;
  int yPos = 0;
  int x = 320,y = 200,xm = 0,ym = 0;
  while(yPos<200) {
	int xPos = 0;
	bool T1 = false;
	while(xPos<320) {
		const ui32 A = *(ptr++);
		const ui32 B = *cck;
		*(cck++) = A;
		const bool t1 = (A!=B);
	//	if (t1) {
			const bool t2 = t1 & (x>xPos);
			x = (t2) ? xPos : x ;
	//		while(xPos<320) {
	//			const ui32 Ap = *(ptr++);
	//			const ui32 Bp = *cck;
	//			*(cck++) = Ap;
	//		    	xPos += 8;
	//			if (A!=B) break;
	//		}
			const bool t3 = t1 & (xm<xPos);
			T1 |= true;
			xm =(t3) ? xPos : xm ;
	//	}
		xPos += 8;
	}
	const bool t2 = (y>yPos);
	const bool t3 = (ym<yPos);
	y = (T1&t2) ? yPos : y ;
	ym =(T1&t3) ? yPos : ym ;
	yPos++;
  }
  if(xm-x>0 && ym-y>0) mkDirty(CLAMP(x,0,320),CLAMP(y,0,200),CLAMP(xm-x+1,0,320),CLAMP(ym-y+1,0,200));*/
#ifndef _DYN_WINSIZE
	SDL_LockSurface(SCRAP);
#endif
  {
    UpdateScreenArea(         //Viewport
                     physbase(),//STScreen,
		     wrBITMAP,
                     0,
                     0x21,
                     0xe0,
                     0xa9-0x21,
                     0x1010,
                     pc2,
                     &oldChecksumA);
    UpdateScreenArea(   //Text scrolling area
                     physbase(),//STScreen,
		     wrBITMAP,
                     0,
                     0xa9,
                     320,
                     0xc8-0xa9,
                     0x0000,
                     pc1,
                     &oldChecksumB);
    UpdateScreenArea(             //portrait area
                     physbase(),//STScreen,
		     wrBITMAP,
                    0,
                     0,
                     320,
                     0x21,
                     0x0000,
                     pc1,
                     &oldChecksumC);
    UpdateScreenArea(              //spells,weapons,moves
                     physbase(),//STScreen,
		     wrBITMAP,
                    0xe0,
                     0x21,
                     0x140-0xe0,
                     0xa9-0x21,
                     0x1010,
                     pc2,
                     &oldChecksumD);
	//g_warning("olololol TUT %d", STBLTCount);
#ifndef _DYN_WINSIZE
	SDL_UnlockSurface(SCRAP);
#endif
	Well___SDL_must_not_be_included_with_pragma_pack(pc1|pc2);
  };
}
