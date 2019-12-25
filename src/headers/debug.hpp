#ifndef DEBUG_HPP_INCLUDED
#define DEBUG_HPP_INCLUDED

// engine headers
#include "stdinc.h"
#include "log.hpp"

// should be included in one source file at a time, in order to debug
#ifdef USE_DEBUG_PRINT
static auto DEBUG_PRINT = [](const char* func, const char* message, int ln) -> void {
	std::cout << "### DEBUG OUTPUT ###\nFunction: ";
	
	// function name provided
	if (func != nullptr)
	  std::cout << func << '\n';
	else
	  std::cout << "undefined.\n";
	
	// print line number
	std::cout << "Line Number: " << ln << std::endl;
	
	// print message
	if (message != nullptr)
	  std::cout << message << std::endl;
	else
	  std::cout << "No message has been provided." << std::endl;
};
#endif

#ifndef NDEBUG
 #define PRINT_LINE(os) std::os << "File: " << __FILE__ << "\nLine: " << __LINE__ << '\n'
 #define PRINT_EXPR(os,e) std::os << #e << " = " << e << '\n'
 #define CHECK_FUNC(x) static int s_iCounter = 0; \
                       ++s_iCounter; \
                       Log_Assert(s_iCounter == (x), "This function should only be called once");
#else
 #define PRINT_LINE(os)
 #define PRINT_EXPR(os,e)
 #define CHECK_FUNC(x)
#endif

#endif /* DEBUG_HPP_INCLUDED */
