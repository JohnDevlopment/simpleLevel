#ifndef LOG_HPP_INCLUDED
#define LOG_HPP_INCLUDED

#include <iostream>
#include <fstream>
#include <sstream>
#include <utility>
#include <cstdarg>
#include <cstdio>

#include "global_attributes.hpp"

#define ERROR(func,param)	std::cerr << "Invalid parameter to " << #func << ": " << #param << '\n'
#define ERRORSTR(c,func,param)	std::sprintf(c, "Invalid paramter '%s' to function '%s'.", #func, #param);

// open log files
void Log_Init() COLD;

// log functions
void Log_Cout(std::string str, ...);
void Log_Cerr(std::string str, ...);

#endif /* LOG_HPP_INCLUDED */
