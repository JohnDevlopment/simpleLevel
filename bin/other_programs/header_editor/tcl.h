#ifndef TCL_H_INCLUDED
#define TCL_H_INCLUDED

#include <tcl.h>
#include <tk.h>
#include <iostream>
#include <cstring>
#include <custom/bad_option>
#include <new>
#include <custom/generic_class.hpp>
#include <custom/levelLoad.hpp>

#define Calloc(ptr,n ) AllocChar(&ptr, n)

int InitTcl(Tcl_Interp** interp);
void AllocChar(char** ptr, size_t n);

inline void FreeChar(char*& _ptr) {
    Tcl_Free(_ptr);
    _ptr = nullptr;
}

#endif // TCL_H_INCLUDED
