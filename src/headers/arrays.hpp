#ifndef ARRAYS_HPP_INCLUDED
#define ARRAYS_HPP_INCLUDED

#include <iostream>
#include <custom/bad_option>
#include <utility>
#include <sstream>
#include <initializer_list>
#include <type_traits>
#include <algorithm>

template<class Type, size_t Num>
class StaticDArray {
	static_assert(Num > 0, "'Num' parameter cannot be zero");

	Type* Elements;

	// allocate the array
	void alloc();

	public:

	typedef Type* iterator;
	typedef const Type* const_iterator;

	typedef Type value_type;

	// construct an array of Num elements and initialize them to their default values
	StaticDArray() : Elements(nullptr) {
//		if (Num <= 1) {
//		  throw bad_option ("StaticDArray: 0 or 1 elements defined in template parameter 2");
//		}

		// allocate the array
		alloc();
	}

	// construct an array of Num elements and intialize them to the arguments provided
	StaticDArray(std::initializer_list<Type> args) : Elements(nullptr) {
//		if (! Num) {
//		  throw bad_option ("StaticDArray: 0 or 1 elements defined in template parameter 2");
//		}

		// allocate the array
		alloc();

		// transfer arguments to the array
		size_t x = 0;
		for (auto&& itr : args)
		  Elements[x++] = std::move(itr);
	}

	~StaticDArray() {
		delete[] Elements;
	}

	// basic info functions //

	// returns the length of the array
	size_t size() const {return Num;}


	// modifiable references //

	// returns a reference to the n'th element of the array
	Type& operator[](size_t _i) {return Elements[_i];}

	// returns a reference to the n'th element in the array
	Type& at(size_t i);


	// read-only references //

	// returns a read-only constant reference to the n'th element in the array
	const Type& operator[](size_t _i) const {return Elements[_i];}

	// returns a reference to the n'th element in the array
	const Type& at(size_t i) const;


	// iterator functions //

	// both return an iterator to the beginning of the array, the second is constant
	iterator begin() {return Elements;}
	const_iterator cbegin() const {return (const_iterator) Elements;}

	// both return an iterator to the end of the array, the second is constant
	iterator end() {return Elements + Num;}
	const_iterator cend() const {return (const_iterator) Elements + Num;}

	// just return a pointer to the beginning of the array; an alias of begin()
	iterator data() const {return Elements;}
};

// public member functions //
template<class Type, size_t Num>
Type& StaticDArray<Type, Num>::at(size_t i) {
	if (i >= Num) {
	  std::stringstream ss;
	  ss << i << " is invalid index; array holds " << Num << " elements.";
	  bad_option exc ( ss.str().c_str() );
	  throw exc;
	}

	return Elements[i];
}

template<class Type, size_t Num>
const Type& StaticDArray<Type, Num>::at(size_t i) const {
	if (i >= Num) {
	  std::stringstream ss;
	  ss << i << " is invalid index; array holds " << Num << " elements.";
	  bad_option exc ( ss.str().c_str() );
	  throw exc;
	}

	return Elements[i];
}

// private member functions //
template<class Type, size_t Num>
void StaticDArray<Type, Num>::alloc() {
	// allocate array of Type, count Num
	Elements = new Type[Num];

	std::is_pod<Type> isMyTypePOD;
	std::is_class<Type> isMyTypeClass;

	// whether this type is POD or a class/struct
	if (isMyTypePOD || isMyTypeClass)
	  std::memset(Elements, 0, sizeof(Type) * Num);
}

#endif /* ARRAYS_HPP_INCLUDED */
