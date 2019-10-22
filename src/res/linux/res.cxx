#include "res.hpp"

using namespace std;

/* Included by res.cc. This file contains the Linux implementation of DLOpen, DLClose, and DLPtr. */
void* DLOpen(const char* file) {
	void* pdl = dlopen(file, RTLD_LAZY);

	if (pdl == nullptr)
	  cerr << "DLOpen error: " << dlerror() << '\n';

return pdl;
}

void* DLOpenLib(const char* file) {
	void* pdl = dlopen(file, RTLD_NOW|RTLD_LOCAL);
	
	if (pdl == nullptr)
	  cerr << "DLOpenLib error: " << dlerror() << '\n';
	
return pdl;
}

void* DLFunction(void* ptr, const char* sym) {
	void* pdl = dlsym(ptr, sym);
	if (pdl == nullptr) {
	  std::string name = "_";
	  name += sym;
	  pdl = dlsym(ptr, name.c_str());
	  if (pdl == nullptr)
	  	cerr << "DLFunction error: " << dlerror() << '\n';
	}
	
return pdl;
}

void DLClose(void* ptr) {
	if (ptr != nullptr)
	  dlclose(ptr);
}

void* DLPtr(void* ptr, const char* sym, size_t* psize) {
	void* ret;
	
	// attempt to get the symbol named by the argument
	ret = dlsym(ptr, sym);

	if (ret == nullptr) {
	  cerr << "DLPtr error: " << dlerror() << '\n';
	  return nullptr;
	}

	// take the original string, remove 5 characters, and append "end" to it
	string end = sym;
	end.erase(end.size()-5, 5);
	end += "end";

	// find the end of the memory block marked by the string class
	void* vpend = dlsym(ptr, end.c_str());

	// generate an error because we couldn't find the end symbol
	if (vpend == nullptr)
	  cerr << "DLPtr 2: " << dlerror() << '\n';

	// dereference the pointer to tell it the size of the memory block
	else if (psize != nullptr)
	  *psize = (size_t) vpend - (size_t) ret;

return ret;
}
