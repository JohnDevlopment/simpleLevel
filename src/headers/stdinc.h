#ifndef STDINC_H_INCLUDED
#define STDINC_H_INCLUDED

// C++ standard IO
#include <iostream>

// standard C library
#include <cstdlib>

// standard C IO functions
#include <cstdio>

// standard C library string-related functions
#include <cstring>

// StaticDArray class; good for statically defined but dynamically allocated arrays
#include "arrays.hpp"

// assertions
#include <cassert>

/* memory header: explicitly defines new and delete,
   and also enables nothrow (std namespace) */
#include <memory>

// size and min/max value of athrihmetic and pointer types
#include <climits>

// standard types used in standard C library
#include <cstdint>

// standard exceptions
#include <stdexcept>

// cast to another type
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
