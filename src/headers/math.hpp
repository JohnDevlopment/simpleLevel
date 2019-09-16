#ifndef MATH_HPP_INCLUDED
#define MATH_HPP_INCLUDED

// math functions
#include <cmath>

// limits of integers
#include <climits>

// iostream and utility functions
#include <iostream>
#include <utility>

// points and vectors
#include <pointvector>

// function attributes
#include "global_attributes.hpp"

// converts miliseconds to frames
INLINE int Ms2Frames(int ms) {
	return FPS * ms / 1000;
}

// return -1 if negative and 0 if positive
int ReturnSign(int val);

// applies linear interpolation to point a in increments of step toward point b and returns the result
float LinearInterp(float a, float b, float step);

// creeps the value of A up to B in increments of delta and returns the result
float Approach(float a, float b, const float delta);

// converts angular degrees into radians
double Deg2Rad(double degrees) ATTR_CONST;

// return the absolute value of an integer
int Abs(int value);
short int Abs(short int value);
long int Abs(long int value);

// returns 1 if the value is less than 1
float at_least_one(float base);

// returns val subtracted by dec without it underflowing
unsigned int Dec(unsigned int val, const unsigned int dec);

// returns val added to inc without it going over the cap
unsigned int Inc(unsigned int val, const unsigned int inc, const unsigned int cap);

// reverses the order of bytes in a single 32-bit integer
uint32_t Swap32bit(uint32_t value);

// reverses the order of four 8-bit integers in an array as if it were a 32-bit integer
void Swap32bit(uint8_t* pValues);

// calculates a delta
INLINE float CalcDelta(float fps, float acc) {
	return (1.0f / fps) * (acc + 1);
}

#endif /* MATH_HPP_INCLUDED */
