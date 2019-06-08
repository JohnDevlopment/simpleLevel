#include "string.h"
#include "tmath.hpp"

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
