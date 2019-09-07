#include <iostream>
#include <cstring>
#include <vector>
#include <map>
#include <algorithm>

typedef unsigned long int HashULong;
typedef unsigned int HashUInt;

#ifndef HASHFUNC_H_DECLARATION
    #undef HASHFUNC_H_DECLARATION
HashULong makeHash(const char* str, int len, const int max, const int p);
#endif /* HASHFUNC_H_DECLARATION */

/* Before including this file, #define HASHFUNC_H_IMPLEMENTATION to source the implementation of makeHash.
   And before including this file, #define HASHFUNC_H_NS to prepend the #define`d value to makeHash as a
   namespace qualifier. */

// define String_strlen if it is not defined
#ifndef String_strlen
#define String_strlen(x)   std::strlen(x)
#endif /* String_strlen */

#ifdef HASHFUNC_H_IMPLEMENTATION
    #undef HASHFUNC_H_IMPLEMENTATION
    
    /* If HASHFUNC_H_NS is defined and equal to one of: game...then
       prepend the namespace qualifier to the function name. */
    #ifdef HASHFUNC_H_NS
        #if HASHFUNC_H_NS == game
HashULong game::makeHash(const char* str, int len, const int max, const int p)
        #endif // HASHFUNC_H_NS == game
    #else
HashULong makeHash(const char* str, int len, const int max, const int p)
    #endif // HASHFUNC_H_
{
    HashULong ulRetval = 0;
    
    // generate hash value for string
    len = (len >= 0) ? len : String_strlen(str);
    for (int x = 0; x < len; ++x) {
        ulRetval += static_cast<HashULong>((int) str[x] * (p + (x + 1)));
    }
    
return max ? (ulRetval % max) : ulRetval;
}
#endif // HASHFUNC_H_IMPLEMENTATION defined
