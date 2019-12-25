#ifndef MATH_HPP_INCLUDED
#define MATH_HPP_INCLUDED

// math functions
#include <cmath>

// limits of integers
#include <climits>

// iostream and utility functions
#include <iostream>
#include <utility>
#include <sstream>

// points and vectors
#include "pointvector"

// function attributes
#include "global_attributes.hpp"

namespace math {
	template<typename T>
	T abs(T value) {
		return (value < 0) ? static_cast<T>(-value) : value;
	}
	
	template<typename T>
	T sign(T value) {
		return (value < 0) ? -1 : 1;
	}
	
	// creeps the value of A up to B in increments of delta and returns the result
	float approach(float a, float b, float delta);
	
	// lineraly interpolates between a & b
	INLINE int8_t lerp(int8_t a, int8_t b, float c) {
		return static_cast<int8_t>(a + (b - a) * c);
	}
	
	INLINE float lerp(float a, float b, float c) {
		return a + (b - a) * c;
	}
	
	// converts angular degrees into radians
	double deg2rad(double degrees);

	// converts angular units radians to degrees
	double rad2deg(double radians);

	// returns val subtracted by dec without it underflowing
	unsigned int dec(unsigned val, unsigned dec); // TODO delete function

	// returns val added to inc without it going over the cap
	unsigned int inc(unsigned val, unsigned inc, unsigned cap); // TODO delete function
	
	// forces the value between a high and a low range
	template<typename T>
	T clamp(T val, T low, T high) {
		if (val < low) return low;
		else if (val > high) return high;
		
		return val;
	}
	
	// change a string to an integer
	INLINE int atoi(const std::string& str) {
		return std::atoi( str.c_str() );
	}
	INLINE int atoi(const char* str) {
		return std::atoi(str);
	}
	
	// converts seconds to milliseconds
	INLINE int sec2ms(float ms) {
		return static_cast<int>(ms * 1000);
	}
	
	// converts milliseconds to frames
	INLINE int ms2frames(int ms, int fps = FPS) {
		return fps * ms / 1000;
	}
}

// converts miliseconds to frames
//INLINE int Ms2Frames(int ms) {
//	return FPS * ms / 1000;
//}

//// calculates a delta
//INLINE float CalcDelta(float fps, float acc) {
//	return (1.0f / fps) * (acc + 1);
//}

#endif /* MATH_HPP_INCLUDED */
