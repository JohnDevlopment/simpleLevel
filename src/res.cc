// game-wide global data
#include "headers/game.hpp"

// resource file functions
#include "headers/res.hpp"

using namespace std;

#ifdef LINUX_OS
  #include "os/dlopen_linux.cxx"
#elif defined(WINDOWS_OS)
  #error "Windows is not supported here."
#else
  #error "Unknown operating system."
#endif
