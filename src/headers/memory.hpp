#ifndef MEMORY_HPP_INCLUDED
#define MEMORY_HPP_INCLUDED

// standard library includes
#include "stdinc.h"

// new, delete
#include <new>

// memory functions
#include <memory>

#define memblk(type, ptr, off)	reinterpret_cast<type*>(ptr + off)

// allocate an array memory
template<class T>
int alloc_mem(T*& ptr, const size_t size) {
	ptr = new (std::nothrow) T[size];
	
return ptr != nullptr ? 1 : 0;
}

// allocate a single block of memory
template<class T>
int alloc_mem(T*& ptr) {
	ptr = new (std::nothrow) T;
	
return ptr != nullptr ? 1 : 0;
}

// copy memory block
template<class T>
void copy_blk(const T* src, T* dst) {
	size_t bytes = sizeof(T);
	
	for (size_t x = 0; x < bytes; ++x) {
	  reinterpret_cast<char*>(dst)[x] = ((const char*) src)[x];
	}
}

/* Fills in the pointer *addr* with the n'th byte of the memory block found
in *blk*, where n equals *off*. */
template<class Type>
void addr_from_blk(Type*& addr, void* blk, size_t off) {
	char* pBlk = (char*) blk + off;
	addr = reinterpret_cast<Type*>(pBlk);
}

#endif /* MEMORY_HPP_INCLUDED */
