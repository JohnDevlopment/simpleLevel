#ifndef ENDIAN_HPP_INCLUDED
#define ENDIAN_HPP_INCLUDED

#include <iostream>
#include "global_attributes.hpp"

// returns 1 if the system is little endian, 0 otherwise
int is_little_endian() PURE;

// swap a value to be little endian
uint16_t SwapLE16(uint16_t val);
uint32_t SwapLE32(uint32_t val);

#endif /* ENDIAN_HPP_INCLUDED */
