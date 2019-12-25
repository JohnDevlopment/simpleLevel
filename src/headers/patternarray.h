#ifndef PATTERNARRAY_H_INCLUDED
#define PATTERNARRAY_H_INCLUDED

#include "stdinc.h"
#include "string.h"

struct PatternArray {
	uint8_t* bytes;
	int width;
	int height;
	int count;
	
	/* Default constructor, destructor frees byte array. Move constructor invalidates
	source object by stealing its byte offset. No copy. */
	PatternArray() : bytes(nullptr), width(0), height(0), count(0) {}
	~PatternArray() {
		delete bytes;
		bytes = nullptr;
	}
	PatternArray(PatternArray&& src) : bytes(src.bytes), width(src.width), height(src.height),
	                                   count(src.count)
	{
		src.bytes = nullptr;
	}
	PatternArray(const PatternArray& src) : bytes(src.bytes), width(src.width), height(src.height),
	                                        count(src.count)
	{
		// copy ctor
	}
	
	// calculates an offset into the byte array
	int offset(int x, int y) const {
		return ((y % height) * width) + (x % width);
	}
};

inline PatternArray make_pattern(int w, int h) {
	PatternArray ret;
	
	ret.width = w;
	ret.height = h;
	ret.count = w * h;
	ret.bytes = new uint8_t[ret.count];
	String_memset(ret.bytes, 0, ret.count);
	
	return std::move(ret);
}

#endif /* PATTERNARRAY_H_INCLUDED */
