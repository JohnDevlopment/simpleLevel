#ifndef GENERIC_CLASS_HPP
#define GENERIC_CLASS_HPP

#include <iostream>

namespace generic_class_ns {
	template<class T>
	void alloc(T*& __ptr) {
		__ptr = new T;
	}
	
	template<class T>
	void dealloc(T*& __ptr) {
		delete __ptr;
		__ptr = NULL;
	}
}

/* How to use: declare an object of this class with a template
   specification of which type you want. A pointer of this type will be created;
   this pointer is allocated memory using new. Alternatively, you can call the
   constructor with an object of the same type and template parameter to either
   copy or move the value of said object. Better yet, you can call the constructor
   with a value to set the pointer's value to.
*/
template<class SomeType>
class generic_class {
	SomeType* Variable;
	size_t Size;

	public:

	generic_class() : Size(sizeof(SomeType)) {
		generic_class_ns::alloc<SomeType>(Variable);
	}

	generic_class(const SomeType& T) : Size(sizeof(SomeType)) {
		generic_class_ns::alloc<SomeType>(Variable);
		*Variable = T;
	}

	~generic_class() {
		generic_class_ns::dealloc<SomeType>(Variable);
		Size = 0;
	}

	// copy constructor
	generic_class(const generic_class& _rhs) : Size(sizeof(SomeType)) {
		generic_class_ns::alloc<SomeType>(Variable);
		
		// assign the right hand's value to the left hand
		*Variable = *(_rhs.Variable);
	}

	// move constructor
	generic_class(generic_class&& _rhs) : Size(sizeof(SomeType)) {
		SomeType*& _variable = _rhs.Variable;

		// set the pointer and clear the other
		Variable = _rhs.Variable;
		_rhs.Variable = nullptr;
		
		// clear other's size value
		_rhs.Size = 0;
	}


	// operators //

	// copy assignment operator
	generic_class& operator=(const generic_class& _rhs) {
		*Variable = *(_rhs.Variable);
		return *this;
	}

	// copy object into class
	generic_class& operator=(const SomeType& _val) {
		*Variable = _val;
		return *this;
	}

	// move assignment operator
	generic_class& operator=(generic_class&& _rhs) {
		SomeType*& _variable = _rhs.Variable;

		// set this pointer and clear the other
		Variable = _rhs.Variable;
		_rhs.Variable = nullptr;

		// clear other size
		_rhs.Size = 0;

		return *this;
	}

	// pointer arrow access operator
	SomeType* operator->() {
		return Variable;
	}


	// get functions //

	// reference to the underlying value
	SomeType& get() const {return *Variable;}

	// constant reference to the underlying value
	const SomeType& getc() const {return *Variable;}

	// pointer to the underlying type
	SomeType* getp() const {return Variable;}

	// constant pointer to the underlying type
	const SomeType* getcp() const {return Variable;}

	// size of the object underneath
	size_t size() const {return Size;}
};

#endif /* GENERIC_CLASS_HPP */
