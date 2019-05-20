#include "headers/log.hpp"

using namespace std;

static ofstream ofs_out;

static ofstream ofs_err;

void Log_Init() {
	if ( ! ofs_out.is_open() && ! ofs_err.is_open() ) {
	  ofs_out.exceptions(ofstream::failbit | ofstream::badbit);
	  ofs_err.exceptions(ofstream::failbit | ofstream::badbit);
	  
	  ofs_out.open("log/cout.txt");
	  ofs_err.open("log/cerr.txt");
	}
}

void Log_Cout(string str, ...) {
	va_list args;
	
	// start reading arguments
	va_start(args, str);
	
	// print each character one-by-one
	for (size_t x = 0; x < str.size(); ++x) {
	  char c = str[x];
	  
	  // a format tag is found
	  if (c == '%') {
	  	char cc = str[++x];
	  	
	  	switch (cc) {
	  	  case 'd': {
	  	  	int val = va_arg(args, int);
	  	  	ofs_out << val;
	  	  	break;
	  	  }
	  	  
	  	  case 'f': {
	  	  	double val = va_arg(args, double);
	  	  	ofs_out << val;
	  	  	break;
	  	  }
	  	  
	  	  case 'u': {
	  	  	unsigned int val = va_arg(args, unsigned int);
	  	  	ofs_out << val;
	  	  	break;
	  	  }
	  	  
	  	  case 'x': {
	  	  	unsigned int val = va_arg(args, unsigned int);
	  	  	ofs_out << "0x" << hex << val << dec;
	  	  	break;
	  	  }
	  	  
	  	  case 'p': {
	  	  	void* ptr = va_arg(args, void*);
	  	  	ptrdiff_t val = (ptrdiff_t) ptr;
	  	  	ofs_out << "0x" << hex << val << dec;
	  	  	break;
	  	  }
	  	  
	  	  case 's': {
	  	  	const char* ptr = va_arg(args, const char*);
	  	  	ofs_out << ptr;
	  	  	break;
	  	  }
	  	  
	  	  default:
	  	  	ofs_out.put(cc);
	  	  	break;
	  	} // end switch()
	  	
	  	// skip to next character
	  	continue;
	  }
	  
	  // print character
	  ofs_out.put(c);
	}
	
	// stop reading arguments
	va_end(args);
	
	// flush output in case it was buffered
	ofs_out.flush();
}

void Log_Cerr(string str, ...) {
	va_list args;
	
	// start reading arguments
	va_start(args, str);
	
	// print each character one-by-one
	for (size_t x = 0; x < str.size(); ++x) {
	  char c = str[x];
	  
	  // a format tag is found
	  if (c == '%') {
	  	char cc = str[++x];
	  	
	  	switch (cc) {
	  	  case 'd': {
	  	  	int val = va_arg(args, int);
	  	  	ofs_err << val;
	  	  	break;
	  	  }
	  	  
	  	  case 'f': {
	  	  	double val = va_arg(args, double);
	  	  	ofs_err << val;
	  	  	break;
	  	  }
	  	  
	  	  case 'u': {
	  	  	unsigned int val = va_arg(args, unsigned int);
	  	  	ofs_err << val;
	  	  	break;
	  	  }
	  	  
	  	  case 'x': {
	  	  	unsigned int val = va_arg(args, unsigned int);
	  	  	ofs_err << "0x" << hex << val << dec;
	  	  	break;
	  	  }
	  	  
	  	  case 'p': {
	  	  	void* ptr = va_arg(args, void*);
	  	  	ptrdiff_t val = (ptrdiff_t) ptr;
	  	  	ofs_err << "0x" << hex << val << dec;
	  	  	break;
	  	  }
	  	  
	  	  case 's': {
	  	  	const char* ptr = va_arg(args, const char*);
	  	  	ofs_err << ptr;
	  	  	break;
	  	  }
	  	  
	  	  default:
	  	  	ofs_err.put(cc);
	  	  	break;
	  	} // end switch()
	  	
	  	// skip to next character
	  	continue;
	  }
	  
	  // print character
	  ofs_err.put(c);
	}
	
	// stop reading arguments
	va_end(args);
	
	// flush output in case it was buffered
	ofs_err.flush();
}