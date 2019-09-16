#ifndef CFUNC_HPP_INCLUDED
#define CFUNC_HPP_INCLUDED

#include <iostream>

template<typename T>
constexpr T cmul(T op1, T op2) {return op1 * op2;}

template<typename T>
constexpr T cdiv(T divisor, T dividend) {return divisor / dividend;}

template<typename T>
constexpr T cadd(T op1, T op2) {return op1 + op2;}

#endif // CFUNC_HPP_INCLUDED
