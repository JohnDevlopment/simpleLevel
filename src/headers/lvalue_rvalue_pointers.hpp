#ifndef LVALUE_RVALUE_POINTERS
#define LVALUE_RVALUE_POINTERS

#include <utility>

// returns a pointer to any valid lvalue
template<typename ReferenceType>
ReferenceType* get_pointer(ReferenceType& _lval) {return &_lval;}

template<typename ReferenceType>
const ReferenceType* get_const_pointer(const ReferenceType& _lval) {return (const ReferenceType*) &_lval;}

template<typename ReferenceType>
ReferenceType** get_dpointer(ReferenceType& _lval) {
	ReferenceType* temp = &_lval;
	return &temp;
}

template<typename PointerType>
PointerType** get_dpointer(PointerType* _ptr) {
	return &_ptr;
}

// getting lvalue references from pointers
template<typename PointerType>
PointerType& get_lval(PointerType* _ptr) {return *_ptr;}

template<typename PointerType>
const PointerType& get_lval(const PointerType* _ptr) {return *_ptr;}

// getting rvalue references from pointers
template<typename PointerType>
PointerType&& get_rval(PointerType* _ptr) {return std::move(*_ptr);}

#endif /* LVALUE_RVALUE_POINTERS */
