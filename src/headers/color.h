#ifndef COLOR_H_INCLUDED
#define COLOR_H_INCLUDED

// engine headers
#include "stdinc.h"
#include "endian.hpp"
#include "macros.h"

// color bytes
#define RED_CHANNEL(val)			(val & 0xff000000)
#define BLUE_CHANNEL(val)			(val & 0x00ff0000)
#define GREEN_CHANNEL(val)			(val & 0x0000ff00)
#define ALPHA_CHANNEL(val)			((char) val)

struct JColor {
	uint8_t red;
	uint8_t green;
	uint8_t blue;
	uint8_t alpha;
};

inline JColor ReturnJColor(uint32_t color) {
	uint8_t* rgba = (uint8_t*) &color;
	JColor retval = {rgba[0], rgba[1], rgba[2], rgba[3]};
	
return retval;
}

inline JColor ReturnJColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
	JColor retval = {r, g, b, a};
	
return retval;
}

inline uint32_t JColor2Int(const JColor& color) {
	uint8_t rgba[] = {color.red, color.green, color.blue, color.alpha};
	
return *reinterpret_cast<uint32_t*>(rgba);
}

#endif /* COLOR_H_INCLUDED */
