#target = Raspberry
#target = Ubuntu_SDL12
target = Ubuntu_SDL20

ifeq ($(target), Raspberry)
  options = -O -D_LINUX -D RGB655 -D SDL12  -c 
  gincludes =  -I/usr/include/SDL `pkg-config --cflags glib-2.0`
  #glibs = -lSDL
  glibs = `pkg-config --libs glib-2.0` -lSDL
  comp = gcc $(options) $(gincludes)
endif
ifeq ($(target), Ubuntu_SDL12)
  options = -O -D_LINUX -D RGB655 -D SDL12  -c 
  gincludes =  -I/usr/include/SDL `pkg-config --cflags glib-2.0`
  #glibs = -lSDL
  glibs = `pkg-config --libs glib-2.0` -lSDL
  comp = gcc $(options) $(gincludes)
endif
ifeq ($(target), Ubuntu_SDL20)
  options =   -O -D _LINUX -D RGB655 -D SDL20 -c 
#  gincludes =  `pkg-config --cflags glib-2.0` \
#               -I/usr/lib/glib-2.0/include \
#               `pkg-config --cflags sdl2`
  gincludes =  `pkg-config --cflags glib-2.0` \
               -I/usr/lib/glib-2.0/include \
               `pkg-config --cflags sdl2`
#  glibs = `pkg-config --libs gtk-2.0 gmodule-2.0` \
#          `pkg-config --libs sdl2`
  glibs =  `sdl2-config --libs`
  comp = gcc $(options) $(gincludes)
endif

./CSB/CSBwin:  \
        DSA.o       data.o         Code11f52.o  Graphics.o  Magic.o          \
        CSBCode.o   Timer.o        Character.o  system.o    SmartDiscard.o   \
        VBL.o       Mouse.o        Recording.o  LinCSBUI.o  CSBlinux.o       \
        Code51a4.o  CSBUI.o        MoveObject.o Hint.o      Monster.o        \
        Code17818.o Viewport.o     AsciiDump.o  Menu.o      Chaos.o          \
        SaveGame.o  Translation.o  Bitmaps.o    md5C.o      utility.o        \
        WinScreen.o Code13ea4.o    Codea59a.o   RC4.o       Attack.o         \
        Sound.o     Code1f9e6.o    Code390e.o   Code222ea.o Statistics.o     \
        SoundMixer.o
	g++     \
            DSA.o       data.o        Code11f52.o  Graphics.o  Magic.o        \
            Character.o CSBCode.o     system.o     Timer.o     SmartDiscard.o \
            VBL.o       Mouse.o       Recording.o  LinCSBUI.o  CSBlinux.o     \
            Code51a4.o  CSBUI.o       MoveObject.o Hint.o      Monster.o      \
            Code17818.o Viewport.o    AsciiDump.o  Menu.o      Chaos.o        \
            SaveGame.o  Translation.o Bitmaps.o    md5C.o      utility.o      \
            WinScreen.o Code13ea4.o   Codea59a.o   RC4.o       Attack.o       \
            Sound.o     Code1f9e6.o   Code390e.o   Code222ea.o Statistics.o   \
            SoundMixer.o  $(glibs) \
	    -o ./CSB/CSBwin

stdafx.h: Objects.h  CSBTypes.h
	touch stdafx.h

SoundMixer.o:  SoundMixer.cpp SoundMixer.h
	$(comp) SoundMixer.cpp

AsciiDump.o: AsciiDump.cpp stdafx.h UI.h Dispatch.h Data.h CSB.h
	$(comp) AsciiDump.cpp

Attack.o: Attack.cpp stdafx.h UI.h Dispatch.h Data.h CSB.h
	$(comp) Attack.cpp

Bitmaps.o: Bitmaps.cpp stdafx.h UI.h Dispatch.h Data.h CSB.h
	$(comp) Bitmaps.cpp

Chaos.o: Chaos.cpp stdafx.h UI.h Dispatch.h Data.h CSB.h
	$(comp) Chaos.cpp

Character.o: Character.cpp stdafx.h UI.h Dispatch.h Data.h CSB.h
	$(comp) Character.cpp

Code51a4.o: Code51a4.cpp stdafx.h UI.h Dispatch.h Data.h CSB.h
	$(comp) Code51a4.cpp

Code11f52.o: Code11f52.cpp stdafx.h UI.h Dispatch.h Data.h CSB.h
	$(comp) Code11f52.cpp

Code13ea4.o: Code13ea4.cpp stdafx.h UI.h Dispatch.h Data.h CSB.h
	$(comp) Code13ea4.cpp

Code17818.o: Code17818.cpp stdafx.h UI.h Dispatch.h Data.h CSB.h
	$(comp) Code17818.cpp

Code1f9e6.o: Code1f9e6.cpp stdafx.h UI.h Dispatch.h Data.h CSB.h
	$(comp) Code1f9e6.cpp

Code222ea.o: Code222ea.cpp stdafx.h UI.h Dispatch.h Data.h CSB.h
	$(comp) Code222ea.cpp

Code390e.o: Code390e.cpp stdafx.h UI.h Dispatch.h Data.h CSB.h
	$(comp) Code390e.cpp

Codea59a.o: Codea59a.cpp stdafx.h UI.h Dispatch.h Data.h CSB.h
	$(comp) Codea59a.cpp

CSBCode.o: CSBCode.cpp stdafx.h UI.h Dispatch.h Data.h CSB.h
	$(comp) CSBCode.cpp

CSBlinux.o: CSBlinux.cpp stdafx.h UI.h Dispatch.h Data.h CSB.h CSBTypes.h resource.h
	$(comp) CSBlinux.cpp

CSBUI.o: CSBUI.cpp stdafx.h UI.h Dispatch.h Data.h CSB.h resource.h
	$(comp) CSBUI.cpp

DSA.o: DSA.cpp DSA.cpp stdafx.h UI.h Dispatch.h Data.h CSB.h
	$(comp) DSA.cpp

data.o: data.cpp stdafx.h UI.h Dispatch.h Data.h CSB.h
	$(comp) data.cpp

Graphics.o: Graphics.cpp stdafx.h UI.h Dispatch.h Data.h CSB.h
	$(comp) Graphics.cpp

Hint.o: Hint.cpp stdafx.h UI.h Dispatch.h CSB.h
	$(comp) Hint.cpp

WinScreen.o: WinScreen.cpp stdafx.h UI.h Dispatch.h Data.h CSB.h CSBTypes.h Objects.h
	$(comp) WinScreen.cpp

LinCSBUI.o: LinCSBUI.cpp stdafx.h UI.h SoundMixer.h\
            Dispatch.h Data.h CSB.h CSBTypes.h Objects.h
	$(comp) LinCSBUI.cpp

Magic.o: Magic.cpp stdafx.h UI.h Dispatch.h Data.h CSB.h
	$(comp) Magic.cpp

md5C.o: md5C.cpp stdafx.h
	$(comp) md5C.cpp

Menu.o: Menu.cpp stdafx.h UI.h Dispatch.h Data.h CSB.h
	$(comp) Menu.cpp

Monster.o: Monster.cpp stdafx.h UI.h Dispatch.h Data.h CSB.h
	$(comp) Monster.cpp

Mouse.o: Mouse.cpp stdafx.h UI.h Dispatch.h Data.h CSB.h
	$(comp) Mouse.cpp

MoveObject.o: MoveObject.cpp stdafx.h UI.h Dispatch.h Data.h CSB.h
	$(comp) MoveObject.cpp

RC4.o: RC4.cpp stdafx.h
	$(comp) RC4.cpp

Recording.o: Recording.cpp stdafx.h UI.h Dispatch.h Data.h CSB.h Recording.h
	$(comp) Recording.cpp

SaveGame.o: SaveGame.cpp stdafx.h UI.h Dispatch.h Data.h CSB.h
	$(comp) SaveGame.cpp

SmartDiscard.o: SmartDiscard.cpp stdafx.h UI.h Dispatch.h Data.h CSB.h
	$(comp) SmartDiscard.cpp

Sound.o: Sound.cpp stdafx.h UI.h Dispatch.h Data.h CSB.h
	$(comp) Sound.cpp

Statistics.o: Statistics.cpp stdafx.h UI.h Dispatch.h Data.h CSB.h
	$(comp) Statistics.cpp

system.o: system.cpp stdafx.h UI.h Dispatch.h Data.h CSB.h LinuxFileName.inl
	$(comp) system.cpp

Timer.o: Timer.cpp stdafx.h UI.h Dispatch.h Data.h CSB.h
	$(comp) Timer.cpp

Translation.o: Translation.cpp stdafx.h UI.h Dispatch.h CSB.h
	$(comp) Translation.cpp

utility.o: utility.cpp stdafx.h UI.h Dispatch.h CSB.h
	$(comp) utility.cpp

VBL.o: VBL.cpp stdafx.h UI.h Dispatch.h Data.h CSB.h
	$(comp) VBL.cpp

Viewport.o: Viewport.cpp stdafx.h UI.h Dispatch.h Data.h CSB.h
	$(comp) Viewport.cpp
