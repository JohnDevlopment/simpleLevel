#ifndef TMATH_HPP_INCLUDED
#define TMATH_HPP_INCLUDED

#include <iostream>
#include <type_traits>

// returns the absolute value of any integer type
template<typename T>
T tabs(T val) {
//	std::is_unsigned<T> isusint;
	
	// if it's unsigned, return
//	if (isusint) return val;
	
	// return positive integer if negative, or just the value otherwise
	return val < 0 ? -val : val;
}

// returns 1 if the value is negative or 0 if it's not
template<typename T>
int tsign(T val) {
	return val < 0 ? 1 : 0;
}

// four basic math operations
template<typename T>
constexpr T cadd(T op1, T op2) {return op1 + op2;}

template<typename T>
constexpr T csub(T op1, T op2) {return op1 - op2;}

template<typename T>
constexpr T cmul(T op1, T op2) {return op1 * op2;}

template<typename T>
constexpr T cdiv(T op1, T op2) {return op1 / op2;}

// bitwise operations
template<typename T>
constexpr T cxor(T op1, T op2) {return op1 ^ op2;}

template<typename T>
constexpr T cor(T op1, T op2) {return op1 | op2;}

template<typename T>
constexpr T cinvert(T op) {return op ^ (0-1);}

template<typename T>
constexpr T cleft(T op, T bits) {return op << bits;}

template<typename T>
constexpr T cright(T op, T bits) {return op >> bits;}

// conversion routines
constexpr uint32_t sec_to_miliseconds(float sec) {return sec * 1000;}
constexpr uint32_t miliseconds_to_frames(uint32_t ms) {return FPS * ms / 1000;}

#endif /* TMATH_HPP_INCLUDED */
