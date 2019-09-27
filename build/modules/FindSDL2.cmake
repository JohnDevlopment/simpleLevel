# find module for SDL2

#[============================================================[.rst:
FindSDL2
--------

Locates SDL library version 2.0.1 and later

Prerequisites
^^^^^^^^^^^^^

Set enviroment variable SDLDIR to the prefix where all of SDL2's files are located. Call `sdl2-config --prefix` on the shell to get it.

Result variables
^^^^^^^^^^^^^^^^

::

  SDL2_LIBRARY
    The library itself
  SDL2_INCLUDE_DIR 
    The location of SDL2's headers
  SDL2_FOUND
    Will be TRUE if the library has been found, FALSE otherwise

Note
^^^^

This module was heavily inspired of the SDL find module that comes with CMake 3.14 as it's compiled.
#]============================================================]
if (NOT SDL2_FIND_QUIETLY)
	message (STATUS "===FindSDL2 module loaded===")
	message (STATUS "Locating SDL2 headers")
endif ()

# locate headers
find_path (SDL2_INCLUDE_DIR SDL.h
            HINTS
              ENV SDLDIR
            PATH_SUFFIXES
              include/SDL2 include
            DOC
              "Location of SDL2's headers under the prefix"
          )

# locate the library file
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
  set(VC_LIB_PATH_SUFFIX lib/x64)
else()
  set(VC_LIB_PATH_SUFFIX lib/x86)
endif()

find_library (SDL2_LIBRARY_TEMP
               NAMES SDL2
               HINTS
                 ENV SDLDIR
               PATH_SUFFIXES
                 lib ${VC_LIB_PATH_SUFFIX}
             )

# render this entry invisible to the user
set_property (CACHE SDL2_LIBRARY_TEMP PROPERTY TYPE INTERNAL)

# if the system is not OSX, then attempt to find SDL2main
if (NOT SDL2_INCLUDE_DIR MATCHES ".framework")
	find_library (SDL2MAIN_LIBRARY SDL2main
	               HINTS
	                 ENV SDLDIR
	               PATH_SUFFIXES
	                 lib ${VC_LIB_PATH_SUFFIX}
	               PATHS
	                 /sw /opt/local /opt/csw /opt
	             )
endif ()

# if OS is not Apple, then find Threads
if (NOT APPLE)
	find_package (Threads)
endif ()

# if using MINGW compiler
if (MINGW)
	set(MINGW32_LIBRARY mingw32 "-mwindows" CACHE STRING "link flags for MinGW")
endif ()

# if the SDL2 library was found
if (SDL2_LIBRARY_TEMP)
	# if SDL2main was located, prepend it to SDL2_LIBRARY
	if (SDL2MAIN_LIBRARY)
	  list (FIND SDL2_LIBRARY_TEMP "${SDL2MAIN_LIBRARY}" _SDL2MAIN_LIBRARY_INDEX)
	  if (_SDL2MAIN_LIBRARY_INDEX EQUAL -1)
	  	set (SDL2_LIBRARY_TEMP "${SDL2MAIN_LIBRARY}" "${SDL2_LIBRARY_TEMP}")
	  endif ()
	  unset (_SDL2MAIN_LIBRARY_INDEX)
	endif ()
	
	# Apple's OS X SDL uses Cocoa as a backend, so include it
	if (APPLE)
	  set (SDL2_LIBRARY_TEMP ${SDL2_LIBRARY_TEMP} "-framework Cocoa")
	endif ()
	
	# do not attempt to link Threads with this on Apple OS X
	if (NOT APPLE)
	  set (SDL2_LIBRARY_TEMP ${SDL2_LIBRARY_TEMP} ${CMAKE_THREAD_LIBS_INIT})
	endif ()
	
	# add Mingw32 library on that compiler
	if (MINGW)
	  set (SDL2_LIBRARY_TEMP ${MINGW32_LIBRARY} ${SDL2_LIBRARY_TEMP})
	endif ()
	
	# set the final string
	set (SDL2_LIBRARY ${SDL2_LIBRARY_TEMP})
endif ()

# read the version header, if it exists, and create a version string from it
if (SDL2_INCLUDE_DIR AND EXISTS "${SDL2_INCLUDE_DIR}/SDL_version.h")
	file (STRINGS "${SDL2_INCLUDE_DIR}/SDL_version.h" _SDL2_VERSION_MAJOR_LINE REGEX "^#define[ \t]+SDL_MAJOR_VERSION[ \t]+[0-9]+$")
	file (STRINGS "${SDL2_INCLUDE_DIR}/SDL_version.h" _SDL2_VERSION_MINOR_LINE REGEX "^#define[ \t]+SDL_MINOR_VERSION[ \t]+[0-9]+$")
	file (STRINGS "${SDL2_INCLUDE_DIR}/SDL_version.h" _SDL2_PATCHLEVEL_LINE REGEX "^#define[ \t]+SDL_PATCHLEVEL[ \t]+[0-9]+$")
	string (REGEX REPLACE "^#define[ \t]+SDL_MAJOR_VERSION[ \t]+([0-9]+)$" "\\1" _SDL2_MAJOR_VERSION "${_SDL2_VERSION_MAJOR_LINE}")
	string (REGEX REPLACE "^#define[ \t]+SDL_MINOR_VERSION[ \t]+([0-9]+)$" "\\1" _SDL2_MINOR_VERSION "${_SDL2_VERSION_MINOR_LINE}")
	string (REGEX REPLACE "^#define[ \t]+SDL_PATCHLEVEL[ \t]+([0-9]+)$" "\\1" _SDL2_PATCHLEVEL "${_SDL2_VERSION_MINOR_LINE}")
	set (SDL2_VERSION_STRING ${_SDL2_MAJOR_VERSION}.${_SDL2_MINOR_VERSION}.${_SDL2_PATCHLEVEL})
	unset (_SDL2_VERSION_MAJOR_LINE)
	unset (_SDL2_VERSION_MINOR_LINE)
	unset (_SDL2_PATCHLEVEL_LINE)
	unset (_SDL2_MAJOR_VERSION)
	unset (_SDL2_MINOR_VERSION)
	unset (_SDL2_PATCHLEVEL)
endif ()

# handle standard arguments to this module
include (FindPackageHandleStandardArgs)
find_package_handle_standard_args (SDL2
                                    REQUIRED_VARS
                                      SDL2_LIBRARY SDL2_INCLUDE_DIR
                                    VERSION_VAR
                                      SDL2_VERSION_STRING
                                  )
