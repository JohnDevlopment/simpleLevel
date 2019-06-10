#ifndef TMATH_HPP_INCLUDED
#define TMATH_HPP_INCLUDED

#include <iostream>
#include <ctgmath>
#include <type_traits>

// absolute value
template<typename T>
T tabs(T val) {
    T retval = val;
    std::is_signed<T> issigned;

    // negate the return value if it's negative
    if (issigned) {
        if (val < 0) retval = -val;
    }

return retval;
}

template<typename T>
T findlcm(T a, T b) {
    T lcm = static_cast<T>(std::fmin(a, b));

    while (lcm >= 2) {
        if (!(a % lcm) && !(b % lcm)) break;
        --lcm;
    }

return lcm;
}

// find previous multiple
template<typename T>
T findpm(T v) {
    T pm = v;

    // decrease pm until it cleanly divides v
    do {
        --pm;
        if (pm < 2) break;
    }
    while (v % pm != 0);

return pm;
}

#endif // TMATH_HPP_INCLUDED
