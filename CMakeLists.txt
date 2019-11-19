cmake_minimum_required(VERSION 3.14)

# define project name
project (simpleLevel DESCRIPTION "Project Dewpsi proof of concept" LANGUAGES C CXX)

# source directory (where this file is)
set (SIMPLELEVEL_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set (PROJECT_ROOT_DIR ${SIMPLELEVEL_ROOT_DIR})

# binary directory
set (SIMPLELEVEL_BINARY_DIR ${PROJECT_ROOT_DIR}/exec)
set (PROJECT_BINARY_DIR ${SIMPLELEVEL_BINARY_DIR})

# path of cmake modules
set (CMAKE_MODULE_PATH ${PROJECT_ROOT_DIR}/build/modules)
include (readobjfile)
include (ConfigDefines)
include (OSDetect)


# set requirements for executable #

# read list from obj_main.txt
readobjfile (${PROJECT_ROOT_DIR}/build/config/obj_main.txt SOURCE_FILES PREFIX ${PROJECT_ROOT_DIR}/src)

# read list from obj_sound.txt
readobjfile (${PROJECT_ROOT_DIR}/build/config/obj_sound.txt temp PREFIX ${PROJECT_ROOT_DIR}/src/sound)
string (APPEND SOURCE_FILES ";${temp}")

# read list from obj_components.txt
readobjfile (${PROJECT_ROOT_DIR}/build/config/obj_components.txt temp
             PREFIX ${PROJECT_ROOT_DIR}/src/components
            )
string (APPEND SOURCE_FILES ";${temp}")

# read list from obj_fileio.txt
readobjfile (${PROJECT_ROOT_DIR}/build/config/obj_fileio.txt temp PREFIX ${PROJECT_ROOT_DIR}/src/fileio)
string (APPEND SOURCE_FILES ";${temp}")

# read list from obj_video.txt
readobjfile (${PROJECT_ROOT_DIR}/build/config/obj_video.txt temp PREFIX ${PROJECT_ROOT_DIR}/src/video)
string (APPEND SOURCE_FILES ";${temp}")

# add pdcommand.cc
string (APPEND SOURCE_FILES ";${PROJECT_ROOT_DIR}/src/commands/pdcommand.cc")

# read list from obj_gamemodes.txt
readobjfile (${PROJECT_ROOT_DIR}/build/config/obj_gamemodes.txt temp PREFIX ${PROJECT_ROOT_DIR}/src/gamemodes)
string (APPEND SOURCE_FILES ";${temp}")

# clear temp variables
unset (temp)


# build executable
include (BuildSet_simpleLevel)
