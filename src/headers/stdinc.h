#ifndef STDINC_H_INCLUDED
#define STDINC_H_INCLUDED

// C++ standard IO
#include <iostream>

// C standard IO
#include <cstdio>

// standard C library main
#include <cstdlib>

// C standard library string functions
#include <cstring>

// StaticDArray class; good for statically defined but dynamically allocated arrays
#include <arrays.hpp>

// assertions
#include <cassert>

// memory header
#include <memory>

// sizes of basic types
#include <climits>

// other types defined here
#include <cstdint>

#define totype(src, type) reinterpret_cast<type>(src)

#if defined(__cplusplus)
  #if __cplusplus >= 201103L
    #define cast(src, type) reinterpret_cast<type>(src)
  #else
    #define cast(src, type) ((type) src)
  #endif
#else
  #define cast(src, type) ((type) src)
#endif /* __cplusplus */

#endif /* STDINC_H_INCLUDED */
