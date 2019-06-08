#ifndef ENDIAN_HPP_INCLUDED
#define ENDIAN_HPP_INCLUDED

#include <iostream>
#include "sdl_incs.h"

static constexpr int __check_byte_order = 1;

// returns 1 if the system is little endian, 0 otherwise
inline int is_little_endian() {
	int i = 1;
	
return ( *(char*) &i == 1 );
}

constexpr int is_little_endian_const() {
	return ( *(const char*) &__check_byte_order == 1 );
}

inline uint16_t SwapLE16(uint16_t val) {
	if ( is_little_endian_const() ) return val;
	
return SDL_SwapLE16(val);
}

inline uint32_t SwapLE32(uint32_t val) {
	if ( is_little_endian_const() ) return val;
	
return SDL_SwapLE32(val);
}

#endif /* ENDIAN_HPP_INCLUDED */
