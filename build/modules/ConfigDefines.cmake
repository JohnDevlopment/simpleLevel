# compiler defines for all projects
# inclusion directories
set (TCL_INCLUDE_DIR "/usr/include/tcl8.6" CACHE PATH "Path to Tcl headers")
set (SDL_INCLUDE_DIR "/usr/local/include/SDL2" CACHE PATH "Path to SDL headers")
set (CUSTOM_INCLUDE_DIR "/usr/local/include/custom" CACHE PATH "Path to custom headers")

# install directory
set (INSTALL_PREFIX "${PROJECT_ROOT_DIR}/exec" CACHE PATH "Where to install ${PROJECT_NAME}")

# option for debug or release
option (DEBUG "Debug or Release Build" OFF)

# set C/C++ standard
set (CMAKE_C_STANDARD 99)
set (CMAKE_C_STANDARD_REQUIRED ON)
set (CMAKE_CXX_STANDARD 14)
set (CMAKE_CXX_STANDARD_REQUIRED ON)
