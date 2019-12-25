#ifndef SCRIPT_HPP_INCLUDED
#define SCRIPT_HPP_INCLUDED

/* System headers */
#include <iostream>

/* User-defined headers */
#include "string.h"
#include "stdinc.h"
#include "pdinifile.h"
#include "program.hpp"

class PDScript {
	PDScript() {}
	
	public:
	
	friend class PDApp;
	
	// default constructor & destructor; disable copy and move
	PDScript(const PDScript& src) = delete;
	PDScript(PDScript&& src) = delete;
	~PDScript() {}
	
	// activate the scripting engine
	static int Init();
	
	// shutdown engine
	static void Shutdown();
	
	// evaluate the script in the scripting language
	static int EvalScript(const char* cmd);
	
	// get the last error message
	static const char* GetError();
};

#endif /* SCRIPT_HPP_INCLUDED */
