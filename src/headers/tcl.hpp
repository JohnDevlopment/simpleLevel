#ifndef TCL_HPP
#define TCL_HPP

// global game data
#include "game.hpp"

#include <tcl.h>
#include "global_attributes.hpp"
#include "program.hpp"

#include "tcl/ptr_wraps.h"

//#define Tcl_Str2Obj(str)	Tcl_NewStringObj(str, nullptr)

inline Tcl_Obj* Tcl_Str2Obj(const char* str, int len = -1) {
	return Tcl_NewStringObj(str, len);
}

inline char* Tcl_Obj2Str(Tcl_Obj* objPtr, int* len = nullptr) {
	return Tcl_GetStringFromObj(objPtr, len);
}

// Tcl interpreter
extern Tcl_Interp* gInterp;

// current level
extern char* CurrentLevel;

// initialize Tcl
int TclCC_Init(PROGRAM* const program) COLD;

// quit Tcl
void TclCC_Quit();

// allocate a character array with Tcl functions
char* TclCC_AllocChar(size_t numBytes) UNUSED;

int TclCC_AddToDict(Tcl_Interp* interp, Tcl_Obj** dict, const char* key, const char* val);

#endif /* TCL_HPP */
