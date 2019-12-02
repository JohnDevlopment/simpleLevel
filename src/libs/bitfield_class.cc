#include <custom/bitfield_class.hpp>

bitcount _Bitfield_ns::bitnums[] = {BIT0, BIT8, BIT16, BIT0, BIT32, BIT0, BIT0, BIT0, BIT64};

static std::vector<char> Format;

void _Bitfield_ns::swap(void* ptr1, void* ptr2, const size_t size) {
	char *cp1 = reinterpret_cast<char*>(ptr1);
	char* cp2 = reinterpret_cast<char*>(ptr2);
	
	if (size == 1) {
	  char c = *cp1;
	  
	  *cp1 = *cp2;
	  *cp2 = c;
	}
	else if (size > 1) {
	  char c;
	  
	  for (size_t x = 0; x < size; ++x) {
	  	c = cp1[x];
	  	cp1[x] = cp2[x];
	  	cp2[x] = c;
	  }
	}
}

const char* _Bitfield_ns::itostr(long int _val, bitcount _nbits) {
	long int b = 1;
	
	// resize vector to contain all bits
	Format.resize(_nbits);
	for (auto itr = Format.rbegin(); itr != Format.rend(); ++itr) {
	  if (b & _val) *itr = '1';
	  else *itr = '0';
	  b <<= 1;
	}
	
	Format.push_back('\0');
	
return (const char*) Format.data();
}

long int _Bitfield_ns::strtol(const char* _str, bitcount _nbits) {
	typedef unsigned short int ushort;
	typedef unsigned int uint;
	typedef unsigned char uchar;
	
	long int b	= 1 << (_nbits - 1);
	long int retval	= 0;
	int diff	= 0;
	
	if (_nbits == BIT0) return 0;
	
	// check length of format string
	{
	  const size_t temp = std::strlen(_str);
	  
	  if (temp != _nbits)
	  	diff = temp - _nbits;
	}
	
	// for loop
	int off = diff;
	for (uchar x = 0; x < _nbits; ++x) {
	  if (diff < 0) {
	  	++diff;
	  	b >>= 1;
	  	continue;
	  }
	  if (_str[x+off] == '1') retval |= b;
	  b >>= 1;
	}
	
return retval;
}
