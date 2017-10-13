# Locate SDL2 library
# This module defines
# SDL2_LIBRARY, the name of the library to link against
# SDL2_FOUND, if false, do not try to link to SDL2
# SDL2_INCLUDE_DIR, where to find SDL.h
#
# This module responds to the the flag:
# SDL2_BUILDING_LIBRARY
# If this is defined, then no SDL2main will be linked in because
# only applications need main().
# Otherwise, it is assumed you are building an application and this
# module will attempt to locate and set the the proper link flags
# as part of the returned SDL2_LIBRARY variable.
#
# Don't forget to include SDLmain.h and SDLmain.m your project for the
# OS X framework based version. (Other versions link to -lSDL2main which
# this module will try to find on your behalf.) Also for OS X, this
# module will automatically add the -framework Cocoa on your behalf.
#
#
# Additional Note: If you see an empty SDL2_LIBRARY_TEMP in your configuration
# and no SDL2_LIBRARY, it means CMake did not find your SDL2 library
# (SDL2.dll, libsdl2.so, SDL2.framework, etc).
# Set SDL2_LIBRARY_TEMP to point to your SDL2 library, and configure again.
# Similarly, if you see an empty SDL2MAIN_LIBRARY, you should set this value
# as appropriate. These values are used to generate the final SDL2_LIBRARY
# variable, but when these values are unset, SDL2_LIBRARY does not get created.
#
#
# $SDL2DIR is an environment variable that would
# correspond to the ./configure --prefix=$SDL2DIR
# used in building SDL2.
# l.e.galup  9-20-02
#
# Modified by Eric Wing.
# Added code to assist with automated building by using environmental variables
# and providing a more controlled/consistent search behavior.
# Added new modifications to recognize OS X frameworks and
# additional Unix paths (FreeBSD, etc).
# Also corrected the header search path to follow "proper" SDL guidelines.
# Added a search for SDL2main which is needed by some platforms.
# Added a search for threads which is needed by some platforms.
# Added needed compile switches for MinGW.
#
# On OSX, this will prefer the Framework version (if found) over others.
# People will have to manually change the cache values of
# SDL2_LIBRARY to override this selection or set the CMake environment
# CMAKE_INCLUDE_PATH to modify the search paths.
#
# Note that the header path has changed from SDL2/SDL.h to just SDL.h
# This needed to change because "proper" SDL convention
# is #include "SDL.h", not <SDL2/SDL.h>. This is done for portability
# reasons because not all systems place things in SDL2/ (see FreeBSD).

#=============================================================================
# Copyright 2003-2009 Kitware, Inc.
#
# Distributed under the OSI-approved BSD License (the "License");
# see accompanying file Copyright.txt for details.
#
# This software is distributed WITHOUT ANY WARRANTY; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
# See the License for more information.
#=============================================================================
# (To distribute this file outside of CMake, substitute the full
#  License text for the above reference.)


if(MSVC)


if(NOT SDL2_TTF_INCLUDE_DIR AND SDL2TTF_INCLUDE_DIR)
  set(SDL2_TTF_INCLUDE_DIR ${SDL2TTF_INCLUDE_DIR} CACHE PATH "directory cache entry initialized from old variable name")
endif()
find_path(SDL2_TTF_INCLUDE_DIR SDL_TTF.h
  HINTS
    ENV SDL2TTFDIR
    ENV SDL2DIR
    ${SDL2_DIR}
  PATH_SUFFIXES SDL2
                # path suffixes to search inside ENV{SDL2DIR}
                include/SDL2 include
)

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
  set(VC_LIB_PATH_SUFFIX lib/x64)
else()
  set(VC_LIB_PATH_SUFFIX lib/x86)
endif()

if(NOT SDL2_TTF_LIBRARY AND SDL2TTF_LIBRARY)
  set(SDL2_TTF_LIBRARY ${SDL2TTF_LIBRARY} CACHE FILEPATH "file cache entry initialized from old variable name")
endif()
find_library(SDL2_TTF_LIBRARY
  NAMES SDL2_TTF
  HINTS
    ENV SDL2TTFDIR
    ENV SDL2DIR
    ${SDL2_DIR}
  PATH_SUFFIXES lib ${VC_LIB_PATH_SUFFIX}
)

if(SDL2_TTF_INCLUDE_DIR AND EXISTS "${SDL2_TTF_INCLUDE_DIR}/SDL_TTF.h")
  file(STRINGS "${SDL2_TTF_INCLUDE_DIR}/SDL_TTF.h" SDL2_TTF_VERSION_MAJOR_LINE REGEX "^#define[ \t]+SDL_TTF_MAJOR_VERSION[ \t]+[0-9]+$")
  file(STRINGS "${SDL2_TTF_INCLUDE_DIR}/SDL_TTF.h" SDL2_TTF_VERSION_MINOR_LINE REGEX "^#define[ \t]+SDL_TTF_MINOR_VERSION[ \t]+[0-9]+$")
  file(STRINGS "${SDL2_TTF_INCLUDE_DIR}/SDL_TTF.h" SDL2_TTF_VERSION_PATCH_LINE REGEX "^#define[ \t]+SDL_TTF_PATCHLEVEL[ \t]+[0-9]+$")
  string(REGEX REPLACE "^#define[ \t]+SDL_TTF_MAJOR_VERSION[ \t]+([0-9]+)$" "\\1" SDL2_TTF_VERSION_MAJOR "${SDL_TTF_VERSION_MAJOR_LINE}")
  string(REGEX REPLACE "^#define[ \t]+SDL_TTF_MINOR_VERSION[ \t]+([0-9]+)$" "\\1" SDL2_TTF_VERSION_MINOR "${SDL_TTF_VERSION_MINOR_LINE}")
  string(REGEX REPLACE "^#define[ \t]+SDL_TTF_PATCHLEVEL[ \t]+([0-9]+)$" "\\1" SDL2_TTF_VERSION_PATCH "${SDL_TTF_VERSION_PATCH_LINE}")
  set(SDL2_TTF_VERSION_STRING ${SDL2_TTF_VERSION_MAJOR}.${SDL2_TTF_VERSION_MINOR}.${SDL2_TTF_VERSION_PATCH})
  unset(SDL2_TTF_VERSION_MAJOR_LINE)
  unset(SDL2_TTF_VERSION_MINOR_LINE)
  unset(SDL2_TTF_VERSION_PATCH_LINE)
  unset(SDL2_TTF_VERSION_MAJOR)
  unset(SDL2_TTF_VERSION_MINOR)
  unset(SDL2_TTF_VERSION_PATCH)
endif()

set(SDL2_TTF_LIBRARY ${SDL2_TTF_LIBRARY})
set(SDL2_TTF_INCLUDE_DIRS ${SDL2_TTF_INCLUDE_DIR})

FIND_PACKAGE_HANDLE_STANDARD_ARGS(SDL2_TTF
                                  REQUIRED_VARS SDL2_TTF_LIBRARY SDL2_TTF_INCLUDE_DIRS
                                  VERSION_VAR SDL2_TTF_VERSION_STRING)

# for backward compatibility
set(SDL2TTF_LIBRARY ${SDL2_TTF_LIBRARY})
set(SDL2TTF_INCLUDE_DIR ${SDL2_TTF_INCLUDE_DIRS})
set(SDL2TTF_FOUND ${SDL2_TTF_FOUND})

mark_as_advanced(SDL2_TTF_LIBRARY SDL2_TTF_INCLUDE_DIR)
else()
  
SET(SDL2TTF_SEARCH_PATHS
  ~/Library/Frameworks
  /Library/Frameworks
  /usr/local
  /usr
  /sw # Fink
  /opt/local # DarwinPorts
  /opt/csw # Blastwave
  /opt
  ${SDL2_DIR}
)

FIND_PATH(SDL2TTF_INCLUDE_DIR SDL_ttf.h
  HINTS
  $ENV{SDL2TTFDIR}
  PATH_SUFFIXES include/SDL2 include
  PATHS ${SDL2TTF_SEARCH_PATHS}
)

FIND_LIBRARY(SDL2TTF_LIBRARY_TEMP
  NAMES SDL2_ttf
  HINTS
  $ENV{SDL2TTFDIR}
  PATH_SUFFIXES lib64 lib
  PATHS ${SDL2TTF_SEARCH_PATHS}
)

IF(NOT SDL2TTF_BUILDING_LIBRARY)
  IF(NOT ${SDL2TTF_INCLUDE_DIR} MATCHES ".framework")
    # Non-OS X framework versions expect you to also dynamically link to
    # SDL2TTFmain. This is mainly for Windows and OS X. Other (Unix) platforms
    # seem to provide SDL2TTFmain for compatibility even though they don't
    # necessarily need it.
    FIND_LIBRARY(SDL2TTFMAIN_LIBRARY
      NAMES SDL2_ttf
      HINTS
      $ENV{SDL2TTFDIR}
      PATH_SUFFIXES lib64 lib
      PATHS ${SDL2TTF_SEARCH_PATHS}
    )
  ENDIF(NOT ${SDL2TTF_INCLUDE_DIR} MATCHES ".framework")
ENDIF(NOT SDL2TTF_BUILDING_LIBRARY)

# SDL2TTF may require threads on your system.
# The Apple build may not need an explicit flag because one of the
# frameworks may already provide it.
# But for non-OSX systems, I will use the CMake Threads package.
IF(NOT APPLE)
  FIND_PACKAGE(Threads)
ENDIF(NOT APPLE)

# MinGW needs an additional library, mwindows
# It's total link flags should look like -lmingw32 -lSDL2TTFmain -lSDL2TTF -lmwindows
# (Actually on second look, I think it only needs one of the m* LIBRARY.)
IF(MINGW)
  SET(MINGW32_LIBRARY mingw32 CACHE STRING "mwindows for MinGW")
ENDIF(MINGW)

IF(SDL2TTF_LIBRARY_TEMP)
  # For SDL2TTFmain
  IF(NOT SDL2TTF_BUILDING_LIBRARY)
    IF(SDL2TTFMAIN_LIBRARY)
      SET(SDL2TTF_LIBRARY_TEMP ${SDL2TTFMAIN_LIBRARY} ${SDL2TTF_LIBRARY_TEMP})
    ENDIF(SDL2TTFMAIN_LIBRARY)
  ENDIF(NOT SDL2TTF_BUILDING_LIBRARY)

  # For OS X, SDL2TTF uses Cocoa as a backend so it must link to Cocoa.
  # CMake doesn't display the -framework Cocoa string in the UI even
  # though it actually is there if I modify a pre-used variable.
  # I think it has something to do with the CACHE STRING.
  # So I use a temporary variable until the end so I can set the
  # "real" variable in one-shot.
  IF(APPLE)
    SET(SDL2TTF_LIBRARY_TEMP ${SDL2TTF_LIBRARY_TEMP} "-framework Cocoa")
  ENDIF(APPLE)

  # For threads, as mentioned Apple doesn't need this.
  # In fact, there seems to be a problem if I used the Threads package
  # and try using this line, so I'm just skipping it entirely for OS X.
  IF(NOT APPLE)
    SET(SDL2TTF_LIBRARY_TEMP ${SDL2TTF_LIBRARY_TEMP} ${CMAKE_THREAD_LIBS_INIT})
  ENDIF(NOT APPLE)

  # For MinGW library
  IF(MINGW)
    SET(SDL2TTF_LIBRARY_TEMP ${MINGW32_LIBRARY} ${SDL2TTF_LIBRARY_TEMP})
  ENDIF(MINGW)

  # Set the final string here so the GUI reflects the final state.
  SET(SDL2TTF_LIBRARY ${SDL2TTF_LIBRARY_TEMP} CACHE STRING "Where the SDL2TTF Library can be found")
  # Set the temp variable to INTERNAL so it is not seen in the CMake GUI
  SET(SDL2TTF_LIBRARY_TEMP "${SDL2TTF_LIBRARY_TEMP}" CACHE INTERNAL "")
ENDIF(SDL2TTF_LIBRARY_TEMP)

INCLUDE(FindPackageHandleStandardArgs)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(SDL2TTF REQUIRED_VARS SDL2TTF_LIBRARY SDL2TTF_INCLUDE_DIR)
endif()