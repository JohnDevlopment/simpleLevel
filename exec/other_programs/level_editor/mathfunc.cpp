#include "mathfunc.hpp"
#include "tmath.hpp"

PVector<int> simp(int& x, int& y) {
    using std::is_signed;

    // find the least common multiple of x & y
    const int lcm = findlcm(tabs(x), tabs(y));

    // divide both fields by a decrementing lcm
    int lcm_copy = lcm;

    while (lcm_copy >= 2) {
        if (x % lcm != 0 || y % lcm != 0)
            --lcm_copy;
        else {
            x /= lcm_copy;
            y /= lcm_copy;
        }
    }

return Point<int>(x, y);
}
