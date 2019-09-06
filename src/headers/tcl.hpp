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

// appends the value indicated by <val> to the dictionary <dict> under the key <key>
int TclCC_AddToDict(Tcl_Interp* interp, Tcl_Obj** dict, const char* key, const char* val);

/* Sets the string argument as the interpreter's string result. <len> is supposed to be
the length of the string argument; but if it's negative, it will be set to the number of characters
counted up to the NULL terminator. */
void TclCC_SetErrorResult(Tcl_Interp* interp, const char* str, int len);

#endif /* TCL_HPP */
