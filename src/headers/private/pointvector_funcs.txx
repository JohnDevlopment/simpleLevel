#ifndef POINTVECTOR_FUNCS_TXX_INCLUDED
#define POINTVECTOR_FUNCS_TXX_INCLUDED

#include <custom/pointvector>

// apply a function to the x and y fields to manipulate their values
template<class _tp>
PVector<_tp> PVector<_tp>::transform(PVector<_tp> (*func)(_tp&,_tp&))
{
    PVector<_tp> retval = func(_x, _y);

return retval;
}

// add another vector to this one and return the result
template<class _tp>
PVector<_tp> PVector<_tp>::operator+(const PVector<_tp>& rhs) const
{
    PVector<_tp> temp;

    temp._x = _x + rhs._x;
    temp._y = _y + rhs._y;

return temp;
}

// subtract another vector from this one and return the result
template<class _tp>
PVector<_tp> PVector<_tp>::operator-(const PVector<_tp>& rhs) const
{
    PVector<_tp> temp;

    temp._x = _x - rhs._x;
    temp._y = _y - rhs._y;

return temp;
}

// multiply another vector with this one and return the result
template<class _tp>
PVector<_tp> PVector<_tp>::operator*(const PVector<_tp>& rhs) const
{
    PVector<_tp> temp;

    temp._x = _x * rhs._x;
    temp._y = _y * rhs._y;

return temp;
}

// divide this vector by another one and return the result
template<class _tp>
PVector<_tp> PVector<_tp>::operator/(const PVector<_tp>& rhs) const
{
    PVector<_tp> temp;

    temp._x = _x / rhs._x;
    temp._y = _y / rhs._y;

return temp;
}

// divide this vector by another one and return the remainder of each quotient
template<class _tp>
PVector<_tp> PVector<_tp>::operator%(const PVector<_tp>& rhs) const
{
    PVector<_tp> temp;

    temp._x = _x % rhs._x;
    temp._y = _y % rhs._y;

return temp;
}

// add a value to this vector's x and y fields and return the result
template<class _tp>
PVector<_tp> PVector<_tp>::operator+(const _tp& rhs) const
{
    PVector<_tp> temp = *this;

    temp._x += rhs;
    temp._y += rhs;

return temp;
}

// subtract a value from this vector's x and y fields and return the result
template<class _tp>
PVector<_tp> operator-(const PVector<_tp>& lhs, const PVector<_tp>& rhs)
{
    PVector<_tp> temp;
    temp._x = lhs._x = rhs._x;
    temp._y = lhs._y = rhs._y;

return temp;
}

// multiply a value with this vector's x and y fields and return the result
template<class _tp>
PVector<_tp> PVector<_tp>::operator*(const _tp& rhs) const
{
    PVector<_tp> temp = *this;

    temp._x *= rhs;
    temp._y *= rhs;

return temp;
}

// divide this vector's x and y fields using a value and return the result
template<class _tp>
PVector<_tp> PVector<_tp>::operator/(const _tp& rhs) const
{
    PVector<_tp> temp = *this;

    temp._x /= rhs;
    temp._y /= rhs;

return temp;
}

// assign another vector's values to this one and return a reference to the lvalue
template<class _tp>
PVector<_tp>& PVector<_tp>::operator=(const PVector<_tp>& rhs)
{
    _x = rhs._x;
    _y = rhs._y;

return *this;
}

// calculates the length of the line
template<class _tp>
float PVector<_tp>::length() const
{
    float length = sqrt(_x*_x + _y*_y);

return length;
}

// non-member function: outputs the x and y value of the vector/point
template<class _tp>
std::ostream& operator<<(std::ostream& out, const PVector<_tp>& mypvector)
{
    out << '(' << mypvector._x << ',' << mypvector._y << ')';

return out;
}

// non-member function: takes two separate "words" into the vector's X and Y components
template<class _tp>
std::istream& operator>>(std::istream& in, PVector<_tp>& mypvector)
{
	in >> mypvector._x;
	in >> mypvector._y;
	return in;
}

// returns a copy of the vector with its X and Y values swapped
template<class _tp>
PVector<_tp> PVector<_tp>::swap() const {
	PVector<_tp> temp = *this;

	// backup the X value
	_tp buffer = temp._x;

	// swap Y for X and X for Y
	temp._x = temp._y;
	temp._y = buffer;

return temp;
}

// returns a new instance of a vector of the same type as the one called upon
template<class _tp>
PVector<_tp> PVector<_tp>::create(_tp x, _tp y) const {
	PVector<_tp> temp(x, y);

return temp;
}

// returns a vector with its components switch between positive and negative
template<class _tp>
PVector<_tp> PVector<_tp>::negative() const
{
    PVector<_tp> temp = *this;
    temp._x = -temp._x;
    temp._y = -temp._y;

return temp;
}

// returns a vector that's perpendicular to this
template<class _tp>
PVector<_tp> PVector<_tp>::ortho()
{
    const PVector& A = *this;

    return PVector<_tp>(A._y, -A._x);
}

#endif /* POINTVECTOR_FUNCS_TXX_INCLUDED */
