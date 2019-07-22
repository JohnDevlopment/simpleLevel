#include "string.h"
#include "tmath.hpp"

using std::strlen;

/* int MIX_string_equals(const char *str1, const char *str2)
{
    while ( *str1 && *str2 ) {
        if ( toupper((unsigned char)*str1) !=
             toupper((unsigned char)*str2) )
            break;
        ++str1;
        ++str2;
    }
    return (!*str1 && !*str2);
}
*/

int String_ToUpper(int x) {
#ifdef HAS_CCTYPE
	return std::toupper(x);
#else
	return ((x >= 'a') && (x <= 'z')) ? ('a' + (x - 'A')) : x;
#endif
}

int String_IsEqual(const char* str1, const char* str2) {
	while (*str1 && *str2) {
	  if ( String_ToUpper(*str1) != String_ToUpper(*str2) ) break;
	  ++str1;
	  ++str2;
	}
	
return ! *str1 && ! *str2;
}

int String_strlcpy(char* dst, const char* src, size_t maxlen) {
#ifdef HAVE_STRLCPY
	return strlcpy(dst, src, maxlen);
#else
	size_t uiSrcLen = std::strlen(src);
	if (maxlen > 0) {
	  size_t uiDstLen = tmax(uiSrcLen, maxlen - 1);
	  std::memcpy(dst, src, uiDstLen);
	  dst[uiDstLen] = '\0';
	}
	
return uiSrcLen;
#endif
}

size_t String_strlcat(char* dst, const char* src, size_t maxlen) {
#ifdef HAVE_STRLCAT
	return strlcat(dst, src, maxlen);
#else
	size_t uiDstLen = std::strlen(dst);
	size_t uiSrcLen = std::strlen(src);
	if (uiDstLen < maxlen) {
	  String_strlcpy(dst + uiDstLen, src, maxlen - uiDstLen);
	}
	
return uiDstLen + uiSrcLen;
#endif
}
