/* User-defined headers */
#include "script.hpp"
#include "log.hpp"
#include "tcl.hpp"

namespace {
	char* _Buffer = nullptr;
	char* _Error = nullptr;
	Tcl_Interp* _Interp = nullptr;
}

/**************************************************/
/* Initialize Tcl scripting engine. Returns 0
on success and -1 on failure. */
/**************************************************/
int PDScript::Init(const PROGRAM& pg) {
	using std::cerr;
	
	_Buffer = Tcl_Alloc(256);
	if (! _Buffer) {
	  cerr << "Failed to allocate buffer\n";
	  return -1;
	}
	
	_Error = Tcl_Alloc(100);
	if (! _Error) {
	  cerr << "Failed to allocate buffer\n";
	  return -1;
	}
	
	// start Tcl interpreter
	_Interp = Tcl_CreateInterp();
	if (! _Interp) {
	  cerr << "Failed to create interpreter\n";
	  return -1;
	}
	
	if ( Tcl_Init(_Interp) != TCL_OK ) {
	  cerr << "Could not load Tcl\n";
	  return -1;
	}
	
return 0;
}

/**************************************************/
/* Shuts down Tcl scripting engine. */
/**************************************************/
void PDScript::Shutdown() {
	Tcl_Free(_Buffer);
	_Buffer = nullptr;
	Tcl_DeleteInterp(_Interp);
}

/**************************************************/
/* Evaluates a command in the scripting
language used. */
/**************************************************/
int PDScript::EvalScript(const char* cmd) {
	int iResult = Tcl_Eval(_Interp, cmd);
	
	if (iResult != TCL_OK) {
	  String_sprintf(_Error, "Error: %s", Tcl_GetStringResult(_Interp));
	}
	
return iResult;
}
