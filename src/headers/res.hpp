#ifndef RES_HPP_INCLUDED
#define RES_HPP_INCLUDED

#include <iostream>
#include <cstdlib>

#include "global_attributes.hpp"
#include "windows_or_linux.hpp"

/* Opens a shared object (.so) file and returns
   a pointer to the part in memory it's located at. Returns NULL
   upon error. A pointer returned from this must be passed to DLClose
   once it is no longer needed.
   
   DLOpenLib should be called if you want to use a shared library.
    */
void* DLOpen(const char* file);
void* DLOpenLib(const char* file);

/* Closes the file referenced by the pointer argument. The pointer is no longer valid afterward.
    */
void DLClose(void* ptr);

/* Returns a pointer to a section in memory that is denoted by the string given or NULL upon
   failure. If a pointer is provided at the third argument, the size of that section in memory
   is put into it.
    */
void* DLPtr(void* ptr, const char* sym, size_t* psize);
void* DLFunction(void* ptr, const char* sym);

#endif /* RES_HPP_INCLUDED */
