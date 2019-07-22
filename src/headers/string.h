#ifndef STRING_H_INCLUDED
#define STRING_H_INCLUDED

#include <iostream>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <cctype>

/*#ifdef __has_include
  #if __has_include(<cctype>)
    #include <cctype>
    #define HAS_CCTYPE 1
  #endif
#else
  #warning __has_include is not supported
#endif*/ /* HAS_CCTYPE */

// convert a lowercase character to uppercase
int String_ToUpper(int x);

// compare two strings with case-insensitive comparisons
int String_IsEqual(const char* str1, const char* str2);

// copy no more than <maxlen> characters from src to dst
int String_strlcpy(char* dst, const char* src, size_t maxlen);

// append src to dst respective of the maximum length <maxlen>
size_t String_strlcat(char* dst, const char* src, size_t maxlen);

#endif /* STRING_H_INCLUDED */
