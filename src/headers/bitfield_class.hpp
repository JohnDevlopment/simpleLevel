#ifndef BITFIELD_CLASS_HPP
#define BITFIELD_CLASS_HPP

#include <iostream>
#include <cstring>
#include <vector>

enum bitcount {
	BIT0	= 0,
	BIT8	= 8,
	BIT16	= 16,
	BIT24	= 24,
	BIT32	= 32,
	BIT64	= 64
};

namespace _Bitfield_ns {
	extern bitcount bitnums[];

	void swap(void* ptr1, void* ptr2, const size_t size);
	const char* itostr(long int _val, bitcount _nbits);
	long int strtol(const char* _str, bitcount _nbits);
}

template<typename IntegerType>
class Bitfield {
	IntegerType Bits;
	size_t NumBits;

	IntegerType invert_bits(IntegerType _val) {
		return (0 - 1) ^ _val;
	}

	public:

	typedef IntegerType value_type;

	// default constructor
	Bitfield() : Bits(0), NumBits(sizeof(IntegerType) * 8) {}

	// converts a string of bits to an integer and assigns it to the bitfield
	Bitfield(const char* _str) : Bits(0), NumBits(sizeof(IntegerType) * 8) {
		using _Bitfield_ns::bitnums;

		Bits = _Bitfield_ns::strtol(_str, bitnums[NumBits/8]);
	}

	// assigns an integer directly to the bitfield
	Bitfield(const IntegerType& src) : Bits(src), NumBits(sizeof(IntegerType) * 8) {}

	// copy constructor
	Bitfield(const Bitfield& _copy) : Bits(_copy.Bits),
	 NumBits(sizeof(IntegerType) * 8) {}

	// move constructor
	Bitfield(Bitfield&& _move) : NumBits(sizeof(IntegerType) * 8) {
		_Bitfield_ns::swap(&Bits, &_move.Bits, sizeof(IntegerType));
	}

	~Bitfield() {}

	// accessors //

	// return a specific bit
	IntegerType getbits(const char* bit) const {
		using namespace _Bitfield_ns;
		return (IntegerType) strtol(bit, bitnums[NumBits/8]);
	}

    	// assignment operator with an integer
	Bitfield& operator=(const IntegerType& _rhs) {
		Bits = _rhs;

	return *this;
	}

	// assignment operator with another object
	Bitfield& operator=(const Bitfield& _rhs) {
		Bits = _rhs.Bits;

	return *this;
	}

	// object << int
	Bitfield operator<<(uint8_t _bits) {
		Bitfield<IntegerType> temp;
		temp.Bits = Bits << _bits;

	return temp;
	}

	// object <<= int
	Bitfield& operator<<=(uint8_t _bits) {
		Bits <<= _bits;

	return *this;
	}

	// object >> int
	Bitfield operator>>(uint8_t _bits) {
		Bitfield<IntegerType> temp;
		temp.Bits = Bits >> _bits;

	return temp;
	}

	// object >>= int
	Bitfield& operator>>=(uint8_t _bits) {
		Bits >>= _bits;

	return *this;
	}

	// size of the variable in bytes
	size_t size() const {return NumBits / 8;}

	// size of the variable in bits
	size_t bits() const {return NumBits;}

	// get the current value
	IntegerType get() const {return Bits;}


	// typecast operator
	operator int() const {return *this;}
	operator unsigned int() const {return *this;}

	// set the bits according to a format string
	void set(const char* _str);

	// set the bits using an integer
	void set(const IntegerType& _val);

	// unset the bits specified by a format string
	void unset(const char* _str);

	// unset the bits using an integer
	void unset(const IntegerType& _val);

	// inverts the bits
	void invert() {
		Bits = invert_bits(Bits);
	}


	/* Returns true if the specified bits are set.
	   The first form accepts a string denoting which
	   bits to check. The second form just accepts a value that's
	   AND'ed with the integer.
	*/
	bool mask(const char* _str) const;
	bool mask(const IntegerType& _val) const;
};

// set bits
template<typename IntegerType>
void Bitfield<IntegerType>::set(const char* _str) {
	using _Bitfield_ns::bitnums;

	IntegerType val = _Bitfield_ns::strtol(_str, bitnums[NumBits/8]);

	Bits |= val;
}

template<typename IntegerType>
void Bitfield<IntegerType>::set(const IntegerType& _val) {
	Bits |= _val;
}

// unset bits
template<typename IntegerType>
void Bitfield<IntegerType>::unset(const char* _str) {
	using _Bitfield_ns::bitnums;

	long int val = _Bitfield_ns::strtol(_str, bitnums[NumBits/8]);
	val = invert_bits(val);

	Bits &= val;
}

template<typename IntegerType>
void Bitfield<IntegerType>::unset(const IntegerType& _val) {
	IntegerType mask = invert_bits(_val);
	Bits &= mask;
}

// test for certain bits
template<typename IntegerType>
bool Bitfield<IntegerType>::mask(const char* _str) const {
	using _Bitfield_ns::bitnums;

	IntegerType val = _Bitfield_ns::strtol(_str, bitnums[NumBits/8]);

return (Bits & val) != 0;
}

template<typename IntegerType>
bool Bitfield<IntegerType>::mask(const IntegerType& _val) const {
	return (Bits & _val) != 0;
}

#endif /* BITFIELD_CLASS_HPP */
