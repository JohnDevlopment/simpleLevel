cmake_minimum_required (VERSION 3.6)
project (header_editor DESCRIPTION "Edit the header of a level" LANGUAGES C CXX)

# root dir
set (HEADER_EDITOR_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set (PROJECT_ROOT_DIR ${HEADER_EDITOR_ROOT_DIR})

# where to build target
set (HEADER_EDITOR_BINARY_DIR ${HEADER_EDITOR_ROOT_DIR}/bin)
set (PROJECT_BINARY_DIR ${HEADER_EDITOR_BINARY_DIR})

# cmake module path
set (CMAKE_MODULE_PATH ${HEADER_EDITOR_ROOT_DIR}/../cmake/Modules)

# prevent in-source builds
include (NotInSource)

# set build rules
include (BuildSet_header_editor)
