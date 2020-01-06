#ifndef LOG_HPP_INCLUDED
#define LOG_HPP_INCLUDED

#include "stdinc.h"
#include <fstream>
#include <utility>
#include <cstdarg>

#include "global_attributes.hpp"

/* Defines a macro Log_Assert(). It tests the expression expr and prints message if
   the test fails, before halting program execution. The macro and function will be empty
   if NDEBUG is defined. */
#ifndef NDEBUG
  #define Log_Assert(expr, msg)	_Log_Assert(expr, msg, __FUNCTION__, __FILE__, __LINE__)
  #define Log_Test(expr)	_Log_Test(expr, #expr);
  #define Log_Print(expr)	std::cout << #expr << " : " << expr << std::endl;
  #define Log_printf(fmt, ...)	std::printf(fmt, __VA_ARGS__);
#else
  #define Log_Assert(expr, msg)
  #define Log_Test(expr)
  #define Log_Print(expr)
  #define Log_printf(fmt, ...)
#endif

/* These are kept just so I don't have to go through all the files that have these defines */
#define ERROR(func,param)	std::cerr << "Invalid parameter to " << #func << ": " << #param << '\n'
#define ERRORSTR(c,func,param)	std::sprintf(c, "Invalid paramter '%s' to function '%s'.", #func, #param);
#define BadParam(f,p)		std::cerr << #f << " : invalid parameter '" << #p << "'\n"
#define PrintError(f,m)		std::cerr << #f << " error : " << m << '\n'

// open log files
void Log_Init() COLD;

// log functions
void Log_Cout(std::string str, ...);
void Log_Cerr(std::string str, ...);

void Log_SetError(const char* fmt, ...);
const char* Log_GetError();
#define Log_BadParam(param)	Log_SetError("bad parameter '%s'", param);

// general output functions
#define Log_Error(fmt, ...)	std::printf(fmt, __VA_ARGS__)
//void Log_Error(std::string fmt, ...);

// tests the given expression and aborts if the test fails
void _Log_Assert(const bool expr, const char* msg, const char* func, const char* file, unsigned int line);

// tests the given expression and prints whether the expression is true
void _Log_Test(const bool expr, const char* testname);

#endif /* LOG_HPP_INCLUDED */
