cmake_minimum_required (VERSION 3.6)
project (new_level DESCRIPTION "Create a new level" LANGUAGES CXX C)

# root directory
set (NEW_LEVEL_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set (PROJECT_ROOT_DIR ${NEW_LEVEL_ROOT_DIR})

# where to build project
set (NEW_LEVEL_BINARY_DIR "${NEW_LEVEL_ROOT_DIR}/build")
set (PROJECT_BINARY_DIR ${NEW_LEVEL_BINARY_DIR})

# cmake module path
set (CMAKE_MODULE_PATH ${NEW_LEVEL_ROOT_DIR}/../cmake/Modules)

# make sure not to build in the source directory
include (NotInSource)

# set build rules
include (BuildSet_new_level)
