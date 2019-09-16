cmake_minimum_required (VERSION 3.14)
project (level_editor DESCRIPTION "Edit the layout a level" LANGUAGES C CXX)

# root directory
set (LEVEL_EDITOR_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set (PROJECT_ROOT_DIR ${LEVEL_EDITOR_ROOT_DIR})

# cmake module path
set (CMAKE_MODULE_PATH ${LEVEL_EDITOR_ROOT_DIR}/../cmake/Modules)

# binary directory
include (OSDetect)
set (LEVEL_EDITOR_BINARY_DIR ${LEVEL_EDITOR_ROOT_DIR}/${PLATFORM_DIR})
set (PROJECT_BINARY_DIR ${LEVEL_EDITOR_BINARY_DIR})

# prevents in-source build
include (NotInSource)

# sets build rules
include (BuildSet_level_editor)
