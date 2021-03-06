#ifndef LOG_HPP_INCLUDED
#define LOG_HPP_INCLUDED

#include <iostream>
#include <fstream>
#include <sstream>
#include <utility>
#include <cstdarg>
#include <cstdio>

#include "global_attributes.hpp"

#define Log_Assert(expr, msg)	_Log_Assert(expr, msg, __FUNCTION__, __FILE__, __LINE__)

#define ERROR(func,param)	std::cerr << "Invalid parameter to " << #func << ": " << #param << '\n'
#define ERRORSTR(c,func,param)	std::sprintf(c, "Invalid paramter '%s' to function '%s'.", #func, #param);

#define BadParam(f,p)		std::cerr << #f << " : invalid parameter '" << #p << "'\n"
#define PrintError(f,m)		std::cerr << #f << " error : " << m << '\n'

#ifndef NDEBUG
#define PrintExpr(e)		std::cout << #e << " : " << e << std::endl
#else
#define PrintExpr(e)
#endif

// open log files
void Log_Init() COLD;

// log functions
void Log_Cout(std::string str, ...);
void Log_Cerr(std::string str, ...);

// DEBUG function; only works in debug mode
void _Log_Assert(const bool expr, const char* msg, const char* func, const char* file, unsigned int line);

#endif /* LOG_HPP_INCLUDED */
