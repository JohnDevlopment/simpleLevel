#ifndef STR_CONST_HPP_INCLUDED
#define STR_CONST_HPP_INCLUDED

#include <iostream>
#include <stdexcept>

class str_const {
	const char* const m_p;
	const size_t m_size;
	
public:
	// construct the string
	template<size_t N>
	constexpr str_const(const char (&a)[N]) : m_p(a), m_size(N-1) {
		// ctor
	}
	
	// offset the array
	constexpr char operator[](size_t n) {
		return n < m_size ? m_p[n] : throw std::out_of_range("");
	}
	
	// returns the string
	constexpr const char* get() const {return m_p;}
	
	// length of the string
	constexpr size_t size() {return m_size;}
};

#endif /* STR_CONST_HPP_INCLUDED */
