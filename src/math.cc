#include "math.hpp"

namespace math {

// approach function
float approach(float a, float b, float delta) {
	const float difference = b - a;
	
	if (difference > delta)
	  return a + delta;
	else if (difference < -delta)
	  return a - delta;
	
return b;
}

/* Convert degrees to radians. */
double deg2rad(double degrees) {
	return degrees * PI / 180.0;
}

/* Convert radians to degrees */
double rad2deg(double radians) {
	return radians * 180.0 / PI;
}

/*
   Decreases the value by the amount given by 'dec', but
   doesn't let it go below zero.
*/
unsigned int dec(unsigned val, unsigned dec) {
	return (val < dec) ? 0 : (val - 1);
}

/*
   Increases the value by the amount given by 'inc', but doesn't
   let it go above the cap.
*/
unsigned int inc(unsigned val, unsigned inc, unsigned cap) {
	return ((val + inc) >= cap) ? cap : (val + inc);
}

} // end namespace math
