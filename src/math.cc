#include "headers/math.hpp"

// return -1 or 1 depending on the sign
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

// degrees to radians
double Deg2Rad(double degrees) {
	return degrees * PI / 180;
}

// absolute value
int Abs(int value) {
	return (value < 0) ? value & (INT_MAX ^ INT_MIN) : value;
}

short int Abs(short int value) {
	return (value < 0) ? value & (SHRT_MAX ^ SHRT_MIN) : value;
}

long int Abs(long int value) {
	return (value < 0) ? value & (LONG_MAX ^ LONG_MIN) : value;
}
// end

float at_least_one(float base) {
	if (base < 0) {
	  if (base > -1) return base - 1;
	}
	else {
	  if (base < 1) return base + 1;
	}
	
return 0;
}

// incremental
unsigned int Dec(unsigned int val, const unsigned int dec) {
	if (val < dec)
	  val = 0;
	else
	  val -= dec;
	
return val;
}

unsigned int Inc(unsigned int val, const unsigned int inc, const unsigned int cap) {
	if ( (val + inc) >= cap )
	  val = cap;
	else
	  val += inc;
	
return val;
}
// end

uint32_t Swap32bit(uint32_t value) {
	uint32_t retval = 0;
	uint8_t* array = (uint8_t*) &retval;
	const uint8_t* carray = (const uint8_t*) &value;
	
	array[0] = carray[3];
	array[1] = carray[2];
	array[2] = carray[1];
	array[3] = carray[0];
	
return retval;
}

void Swap32bit(uint8_t* pValues) {
	if (pValues == nullptr) return;
	
	uint8_t temp[] = {pValues[0], pValues[1], pValues[2], pValues[3]};
	int y = 3;
	for (int x = 0; x < 4; ++x)
	  pValues[x] = temp[y--];
}
