# Makefile for tachyon under Unix using GNUmake.
#
# Usage:
#
#    make		Compile tachyon with optimization
#    make debug		Compile with debugging + pointer bounds checking
#    make run		Compile and run the debug version, using DM data.
#    make clean		Remove temporary files (*.o and core)
#    make archive	Create a tar of the source code in the parent directory.
#			(Filename will include version number and today's date.)

#sdlversion = sdl		# Use for old Raspberry Pi
sdlversion = sdl2
CXXFLAGS +=  `pkg-config --cflags glib-2.0 $(sdlversion)`
LDFLAGS += `pkg-config --libs glib-2.0 $(sdlversion)`

CXXFLAGS += -D_LINUX -D RGB655


# By default, 'make' compiles tachyon with optimization level 2.
default: 	CXXFLAGS += -O2
default:	tachyon

# If 'make debug' is run, create debuggable version with hardware pointer bounds
debug:		CXXFLAGS += -g -fcheck-pointer-bounds -mmpx -D _DEBUG
debug:		LDFLAGS += -lmpx -lmpxwrappers
debug:		tachyon

# For convenience, 'make run' will run the debug version with Dungeon Master. 
run:	debug
	(cd games/dm/ && ../../tachyon --extralarge || true)

OBJS = \
        DSA.o       data.o         Code11f52.o  Graphics.o  Magic.o          \
        CSBCode.o   Timer.o        Character.o  system.o    SmartDiscard.o   \
        VBL.o       Mouse.o        Recording.o  LinCSBUI.o  CSBlinux.o       \
        Code51a4.o  CSBUI.o        MoveObject.o Hint.o      Monster.o        \
        Code17818.o Viewport.o     AsciiDump.o  Menu.o      Chaos.o          \
        SaveGame.o  Translation.o  Bitmaps.o    md5C.o      utility.o        \
        WinScreen.o Code13ea4.o    Codea59a.o   RC4.o       Attack.o         \
        Sound.o     Code1f9e6.o    Code390e.o   Code222ea.o Statistics.o     \
        SoundMixer.o

tachyon: $(OBJS)
	$(CXX) -o tachyon $(OBJS) $(LDFLAGS)

stdafx.h: Objects.h  CSBTypes.h
	touch stdafx.h

SoundMixer.o:  SoundMixer.cpp SoundMixer.h

AsciiDump.o: AsciiDump.cpp stdafx.h UI.h Dispatch.h Data.h CSB.h

Attack.o: Attack.cpp stdafx.h UI.h Dispatch.h Data.h CSB.h

Bitmaps.o: Bitmaps.cpp stdafx.h UI.h Dispatch.h Data.h CSB.h

Chaos.o: Chaos.cpp stdafx.h UI.h Dispatch.h Data.h CSB.h

Character.o: Character.cpp stdafx.h UI.h Dispatch.h Data.h CSB.h

Code51a4.o: Code51a4.cpp stdafx.h UI.h Dispatch.h Data.h CSB.h

Code11f52.o: Code11f52.cpp stdafx.h UI.h Dispatch.h Data.h CSB.h

Code13ea4.o: Code13ea4.cpp stdafx.h UI.h Dispatch.h Data.h CSB.h

Code17818.o: Code17818.cpp stdafx.h UI.h Dispatch.h Data.h CSB.h

Code1f9e6.o: Code1f9e6.cpp stdafx.h UI.h Dispatch.h Data.h CSB.h

Code222ea.o: Code222ea.cpp stdafx.h UI.h Dispatch.h Data.h CSB.h

Code390e.o: Code390e.cpp stdafx.h UI.h Dispatch.h Data.h CSB.h

Codea59a.o: Codea59a.cpp stdafx.h UI.h Dispatch.h Data.h CSB.h

CSBCode.o: CSBCode.cpp stdafx.h UI.h Dispatch.h Data.h CSB.h

CSBlinux.o: CSBlinux.cpp stdafx.h UI.h Dispatch.h Data.h CSB.h CSBTypes.h resource.h

CSBUI.o: CSBUI.cpp stdafx.h UI.h Dispatch.h Data.h CSB.h resource.h

DSA.o: DSA.cpp DSA.cpp stdafx.h UI.h Dispatch.h Data.h CSB.h

data.o: data.cpp stdafx.h UI.h Dispatch.h Data.h CSB.h

Graphics.o: Graphics.cpp stdafx.h UI.h Dispatch.h Data.h CSB.h

Hint.o: Hint.cpp stdafx.h UI.h Dispatch.h CSB.h

WinScreen.o: WinScreen.cpp stdafx.h UI.h Dispatch.h Data.h CSB.h CSBTypes.h Objects.h

LinCSBUI.o: LinCSBUI.cpp stdafx.h UI.h SoundMixer.h\
            Dispatch.h Data.h CSB.h CSBTypes.h Objects.h

Magic.o: Magic.cpp stdafx.h UI.h Dispatch.h Data.h CSB.h

md5C.o: md5C.cpp stdafx.h

Menu.o: Menu.cpp stdafx.h UI.h Dispatch.h Data.h CSB.h

Monster.o: Monster.cpp stdafx.h UI.h Dispatch.h Data.h CSB.h

Mouse.o: Mouse.cpp stdafx.h UI.h Dispatch.h Data.h CSB.h

MoveObject.o: MoveObject.cpp stdafx.h UI.h Dispatch.h Data.h CSB.h

RC4.o: RC4.cpp stdafx.h

Recording.o: Recording.cpp stdafx.h UI.h Dispatch.h Data.h CSB.h Recording.h

SaveGame.o: SaveGame.cpp stdafx.h UI.h Dispatch.h Data.h CSB.h

SmartDiscard.o: SmartDiscard.cpp stdafx.h UI.h Dispatch.h Data.h CSB.h

Sound.o: Sound.cpp stdafx.h UI.h Dispatch.h Data.h CSB.h

Statistics.o: Statistics.cpp stdafx.h UI.h Dispatch.h Data.h CSB.h

system.o: system.cpp stdafx.h UI.h Dispatch.h Data.h CSB.h LinuxFileName.inl

Timer.o: Timer.cpp stdafx.h UI.h Dispatch.h Data.h CSB.h

Translation.o: Translation.cpp stdafx.h UI.h Dispatch.h CSB.h

utility.o: utility.cpp stdafx.h UI.h Dispatch.h CSB.h

VBL.o: VBL.cpp stdafx.h UI.h Dispatch.h Data.h CSB.h

Viewport.o: Viewport.cpp stdafx.h UI.h Dispatch.h Data.h CSB.h

clean:
	rm -f *.o core


# Use `make archive` to create an archive of all the source code plus
# a Linux executable in a filename with the current date and version
# number. (e.g., tachyon17.7_SRC_20190620.tar.gz)

dirname = $(shell basename `pwd`)
version = $(shell  sed -rn '/define APPVERSION/ {s/.*"(.*)".*/\1/;p}' < CSBlinux.cpp)
date = $(shell date +%Y%m%d)
archivefile = tachyon$(version)_SRC_$(date)

archive: tachyon clean
	@echo "Tarring up ${archivefile}.tar.gz"
	(cd ..; tar -zcf "${archivefile}.tar.gz" "${dirname}")



