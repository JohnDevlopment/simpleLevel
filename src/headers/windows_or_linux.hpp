#ifndef WINDOWS_OR_LINUX_HPP
#define WINDOWS_OR_LINUX_HPP

#ifdef __unix__
	#define LINUX_OS
	#include "os/linux.h"
#elif defined(_WIN32) || defined(WIN32)
	#define WINDOWS_OS
	#include "os/windows.h"
#else
	#error "Unknown operating system"
#endif

#endif /* WINDOWS_OR_LINUX_HPP */
