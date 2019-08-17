#ifndef TCL_HPP
#define TCL_HPP

#include "game.hpp"
#include <tcl.h>
#include "global_attributes.hpp"
#include "program.hpp"

// Tcl interpreter
extern Tcl_Interp* gInterp;

// initialize Tcl
int TclCC_Init(PROGRAM* const program) COLD;

// quit Tcl
void TclCC_Quit();

// allocate a character array with Tcl functions
char* TclCC_AllocChar(size_t numBytes) UNUSED;

// TODO add description
int TclCC_AddToDict(Tcl_Interp* interp, Tcl_Obj** dict, const char* key, const char* val);

#endif /* TCL_HPP */
