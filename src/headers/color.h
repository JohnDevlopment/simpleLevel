#ifndef COLOR_H_INCLUDED
#define COLOR_H_INCLUDED

#include <iostream>
#include <custom/bits/image_class_defines.hpp>

struct JColor {
	uint8_t red;
	uint8_t green;
	uint8_t blue;
	uint8_t alpha;
};

inline JColor ReturnJColor(uint32_t color) {
	JColor retval = {0};
	retval.red    = RED_CHANNEL(color);
	retval.green  = GREEN_CHANNEL(color);
	retval.blue   = BLUE_CHANNEL(color);
	retval.alpha  = ALPHA_CHANNEL(color);
	
return retval;
}

inline JColor ReturnJColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
	JColor retval = {r, g, b, a};
	
return retval;
}

inline uint32_t JColor2Int(const JColor& color) {
	return RGBTOUINT32(color.red, color.green, color.blue);
}

#endif /* COLOR_H_INCLUDED */
