# compiler defines for all projects
# inclusion directories
set (TCL_INCLUDE_DIR "/usr/include/tcl8.6" CACHE PATH "Path to Tcl headers")
set (SDL_INCLUDE_DIR "/usr/local/include/SDL2" CACHE PATH "Path to SDL headers")
set (CUSTOM_INCLUDE_DIR "/usr/local/include/custom" CACHE PATH "Path to custom headers")

# install directory
set (INSTALL_PREFIX "${PROJECT_ROOT_DIR}/bin" CACHE PATH "Where to install ${PROJECT_NAME}")

# option for debug or release
option (DEBUG "Debug or Release Build" OFF)

# set C/C++ standard
set (CMAKE_C_STANDARD 99)
set (CMAKE_C_STANDARD_REQUIRED ON)
set (CMAKE_CXX_STANDARD 11)
set (CMAKE_CXX_STANDARD_REQUIRED ON)

# add this to SDL project: target_link_options (<target> PRIVATE "LINKER:-rpath,/usr/local/lib,--enable-new-dtags")

# print information
message (STATUS "=== Information ===")
message (STATUS "${PROJECT_NAME} root directory: ${PROJECT_ROOT_DIR}")
message (STATUS "Build location: ${PROJECT_BINARY_DIR}")
message (STATUS "Install path: ${INSTALL_PREFIX}")
