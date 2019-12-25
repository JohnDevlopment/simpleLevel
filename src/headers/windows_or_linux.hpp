#ifndef WINDOWS_OR_LINUX_HPP
#define WINDOWS_OR_LINUX_HPP

#if defined(LINUX)
  #include "os/linux/res.h"
#elif defined(WIN32)
  #include "os/windows/res.h"
#else
  #error "This operating system has no headers for it"
#endif

#endif /* WINDOWS_OR_LINUX_HPP */
