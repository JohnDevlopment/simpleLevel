#ifndef POINTVECTOR_DECL_HPP_INCLUDED
#define POINTVECTOR_DECL_HPP_INCLUDED

#include <iostream>
#include <cmath>

template<class ValueType> class PVector;

template<class ValueType>
class PVector {
public:
	/* First constructor constructs object. Second one is where you set
	   the vector's X and Y components. */
	PVector() : _x(0), _y(0) {}
	PVector(ValueType _x2, ValueType _y2) : _x(_x2), _y(_y2) {}

	// destroy the object
	~PVector() {}

	/* First one copies the vector object. Second one copies the vector
	   object and nullifies the original. */
	PVector(const PVector& _rhs) : _x(_rhs._x), _y(_rhs._y) {}
	PVector(PVector&& _rhs) : _x(_rhs._x), _y(_rhs._y) {}

	// accessors //
	ValueType x() const {return _x;}
	ValueType y() const {return _y;}

	// return a reference to x (_i == 0) or y (_i == 1)
	ValueType& operator[](size_t _i) {
		return ((ValueType*) &_x)[_i];
	}

	// constant reference
	const ValueType& operator[](size_t _i) const {
		return ((ValueType*) &_x)[_i];
	}

	// pass X & Y values by reference to function
	PVector transform(PVector (*func)(ValueType&,ValueType&));

	// type cast/conversion operations //
	operator PVector<float>() {
		return PVector<float>((float) _x, (float) _y);
	}

	// math operations //
	PVector operator+(const PVector& rhs) const;
	PVector operator-(const PVector& rhs) const;
	PVector operator*(const PVector& rhs) const;
	PVector operator/(const PVector& rhs) const;
	PVector operator%(const PVector& rhs) const;

	PVector operator+(const ValueType& rhs) const;
	PVector operator-(const ValueType& rhs) const;
	PVector operator*(const ValueType& rhs) const;
	PVector operator/(const ValueType& rhs) const;

	// compound math operations //

	// add the vector to this one
	PVector& operator+=(const PVector& rhs) {
		_x = _x + rhs._x;
		_y = _y + rhs._y;

	return *this;
	}

	// comparison operators //

	// equal to vector, equal to value
	bool operator==(const PVector& rhs) const {
		return _x == rhs._x && _y == rhs._y;
	}
	bool operator==(const ValueType& rhs) const {
		return _x == rhs && _y == rhs;
	}

	// not equal to vector, not equal to value
	bool operator!=(const PVector& rhs) const {
		return _x != rhs._x && _y != rhs._y;
	}
	bool operator!=(const ValueType& rhs) const {
		return _x != rhs && _y != rhs;
	}

	// less than or less/equal to
	bool operator<(const PVector& rhs) const {
		return _x < rhs._x && _y < rhs._y;
	}
	bool operator<(const ValueType& rhs) const {
		return _x < rhs && _y < rhs;
	}
	bool operator<=(const PVector& rhs) const {
		return _x <= rhs._x && _y <= rhs._y;
	}

	// greater than or greater/equal to
	bool operator>(const PVector& rhs) const {
		return _x > rhs._x && _y > rhs._y;
	}
	bool operator>(const ValueType& rhs) const {
		return _x > rhs && _y > rhs;
	}
	bool operator>=(const PVector& rhs) const {
		return _x >= rhs._x && _y >= rhs._y;
	}
	bool operator>=(const ValueType& rhs) const {
		return _x >= rhs && _y >= rhs;
	}

	// misc operations

	// returns a vector with swapped X & Y fields
	PVector swap() const;

	// creates a new instance of PVector
	PVector create(ValueType x, ValueType y) const;

	// returns a vector with its components switch between positive and negative
	PVector negative() const;

	// assignment
	PVector& operator=(const PVector& rhs);

	// return the length of this vector
	float length() const;

	// return a normalized/normal-length/unit-length vector
	PVector normalized() const {
		return (*this) / this->length();
	}

	// returns an orthogonal vector
	PVector ortho();

	// allows PVector to be passed to cout
	template<class _valuetype>
	friend std::ostream& operator<<(std::ostream& out, const PVector<_valuetype>& mypvector);

	// allows PVector to be passed to cin
	template<class _valuetype>
	friend std::istream& operator>>(std::istream& in, PVector<_valuetype>& mypvector);

protected:
	ValueType _x;
	ValueType _y;
};

template<typename T>
PVector<float> FloatFromVector(const PVector<T>& _v) {
	return PVector<float>((float) _v.x(), (float) _v.y());
}

// returns the dot product of two normalized vectors
inline float DotProduct(const PVector<float>& A, const PVector<float>& B)
{
    //
    //
    return (A.x() * B.x()) + (A.y() * B.y());
}

//inline float Angle()

#endif /* POINTVECTOR_DECL_HPP_INCLUDED */
