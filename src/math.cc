#include "math.hpp"

/* Returns 1 if the value is zero or positive, -1 otherwise. */
int ReturnSign(int val) {
	int ret = 1;
	
	if (val < 0) ret = -1;
	
return ret;
}

// linear interpolation function
float LinearInterp(float a, float b, float step) {
	return (1.0f - step) * a + (step * b);
}

// approach function
float Approach(float a, float b, const float delta) {
	const float difference = b - a;
	
	if (difference > delta)
	  return a + delta;
	else if (difference < -delta)
	  return a - delta;
	
return b;
}

/* Convert degrees to radians. */
double Deg2Rad(double degrees) {
	return degrees * PI / 180;
}

/*
   Returns the absolute value of an integer
*/
int Abs(int value) {
	return (value < 0) ? value & (INT_MAX ^ INT_MIN) : value;
}

short int Abs(short int value) {
	return (value < 0) ? value & (SHRT_MAX ^ SHRT_MIN) : value;
}

long int Abs(long int value) {
	return (value < 0) ? value & (LONG_MAX ^ LONG_MIN) : value;
}

/* Returns 1 if the value given is less than 1. */
float at_least_one(float base) {
	if (base < 0) {
	  if (base > -1) return base - 1;
	}
	else {
	  if (base < 1) return base + 1;
	}
	
return 0;
}

/*
   Decreases the value by the amount given by 'dec', but
   doesn't let it go below zero.
*/
unsigned int Dec(unsigned int val, const unsigned int dec) {
	if (val < dec)
	  val = 0;
	else
	  val -= dec;
	
return val;
}

/*
   Increases the value by the amount given by 'inc', but doesn't
   let it go above the cap.
*/
unsigned int Inc(unsigned int val, const unsigned int inc, const unsigned int cap) {
	if ( (val + inc) >= cap )
	  val = cap;
	else
	  val += inc;
	
return val;
}
