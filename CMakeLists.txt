
# ROQUEN - First rough draft 

project(CSBWin)
cmake_minimum_required(VERSION 2.4.0)

set(PACKAGE_NAME    csb)
set(PACKAGE_VERSION 11.1.0)
set(PACKAGE_STRING "${PACKAGE_NAME} ${PACKAGE_VERSION}")
set(PACKAGE_BUGREPORT "some-form-thread-link")


# Do not allow building at the root of the source tree.
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR AND NOT MSVC_IDE)
  message(FATAL_ERROR "In-source builds are not allowed.
   CMake would overwrite the files distributed with CSBWin.
   Please create a directory and run cmake from there, passing the path
   to this source directory as the last argument.
   This process created the file `CMakeCache.txt' and the directory `CMakeFiles'.
   Please delete them.")
endif(CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR AND NOT MSVC_IDE)


set(CSB_MAIN_SRC_DIR  ${CMAKE_CURRENT_SOURCE_DIR})
set(CSB_BINARY_DIR    ${CMAKE_CURRENT_BINARY_DIR})
set(CMAKE_C_FLAGS     "-g -fsigned-char -O1")
set(CMAKE_CXX_FLAGS   "-g -fsigned-char -O1")
set(CMAKE_LD_FLAGS    -g)


if(WIN32)
  MESSAGE("WIN32")
  if(CYGWIN)
    set(CSB_ON_WIN32 0)
    set(CSB_ON_UNIX  1)
    add_definitions(-D_LINUX)
    MESSAGE("got here")
  else(CYGWIN)
    MESSAGE("got win32")
    set(CSB_ON_WIN32 1)
    set(CSB_ON_UNIX  0)
  endif(CYGWIN)
else(WIN32)
 
  if(UNIX)
    set(CSB_ON_WIN32 0)
    set(CSB_ON_UNIX  1)
    add_definitions(-D_LINUX)
  else(UNIX)
    MESSAGE(SEND_ERROR "Unable to determine platform")
  endif(UNIX)
endif(WIN32)


# BLAH -- 
if(MSVC)
  add_definitions(-D_MSVC_INTEL -D_MIDI -D_MOVIE)
  add_definitions(-D_CRT_SECURE_NO_DEPRECATE -D_CRT_SECURE_NO_WARNINGS)
  set(PLATFORM_SRC CSBwin.cpp)
else(MSVC)
  add_definitions(-D_LINUX)
  if(APPLE)
    set(PLATFORM_SRC MacScreen.cpp CSBMac.cpp)
  else(APPLE)
    set(PLATFORM_SRC LinScreen.cpp CSBlinux.cpp LinCSBUI.cpp)
  endif(APPLE)
endif(MSVC)

#if(CSB_USE_SDL)
#  find_path(SDL_PATH)
#endif(CSB_USE_SDL)

# NO! NO! NO! --- mucking around
if (CMAKE_COMPILER_IS_GNUCXX)
   MESSAGE("GCC")
    add_definitions(-D_LINUX)
endif (CMAKE_COMPILER_IS_GNUCXX)

#CSBUICE2002.cpp
#CSBWinCE2002.cpp
#WinScreenCE2002.cpp
#HintCE.cpp
#Win32_SoundMixer.cpp

# The "WIN32" thing is ignored under other OSes
add_executable(CsbWin WIN32
    ${PLATFORM_SRC}
    AsciiDump.cpp
    Attack.cpp
    Bitmaps.cpp
    CSBCode.cpp
    CSBUI.cpp
    Chaos.cpp
    Character.cpp
    Code11f52.cpp
    Code13ea4.cpp
    Code17818.cpp
    Code1f9e6.cpp
    Code222ea.cpp
    Code390e.cpp
    Code51a4.cpp
    Codea59a.cpp
    DSA.cpp
    Graphics.cpp
    Hint.cpp
    Magic.cpp
    Menu.cpp
    Monster.cpp
    Mouse.cpp
    MoveObject.cpp
    NewBugs.cpp
    OldBugs.cpp
    RC4.cpp
	Recording.cpp
    SaveGame.cpp
    SmartDiscard.cpp
    Sound.cpp
    Statistics.cpp
    Stdafx.cpp
    Timer.cpp
    Translation.cpp
    VBL.cpp
    Viewport.cpp
    WinScreen.cpp
    data.cpp
    md5C.cpp
    system.cpp
    utility.cpp
)

if(UNIX)
	FIND_PACKAGE(GTK2)
	if(GTK2_FOUND)
		message("GTK found")
		include_directories(${GTK2_INCLUDE_DIRS})
		target_link_libraries(CsbWin ${GTK2_LIBRARIES})
		#add_definitions(-DUSE_OLD_GTK)
	else(GTK2_FOUND)
		message("no GTK")
	endif(GTK2_FOUND)

	FIND_PACKAGE(SDL REQUIRED)
	if(SDL_FOUND)
		message("SDL found")
		include_directories(${SDL_INCLUDE_DIRS})
		target_link_libraries(CsbWin ${SDL_LIBRARIES})
	else(SDL_FOUND)
		message(FATAL_ERROR "libSDL is required")
	endif(SDL_FOUND)

	FIND_PACKAGE(SDL_mixer)
	if(SDLMIXER_FOUND)
		message("SDL mixer found")
		include_directories(${SDLMIXER_INCLUDE_DIR})
		target_link_libraries(CsbWin ${SDLMIXER_LIBRARY})
		add_definitions(-DSOUND__SDLMIXER)
	endif(SDLMIXER_FOUND)
endif(UNIX)

if (CSB_ON_WIN32)
  target_link_libraries(CsbWin winmm.lib)
endif(CSB_ON_WIN32)


