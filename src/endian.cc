#include "endian.hpp"
#include "sdl_incs.h"

constexpr int __byte_order = 1;

int is_little_endian() {
	return (* (char*) &__byte_order == 1);
}

uint16_t SwapLE16(uint16_t val) {
	return is_little_endian() ? val : SDL_SwapLE16(val);
}

uint32_t SwapLE32(uint32_t val) {
	return is_little_endian() ? val : SDL_SwapLE32(val);
}
