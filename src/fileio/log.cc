#include "log.hpp"
#include "string.h"

using namespace std;

static string _sError;

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

void Log_SetError(const char* fmt, ...) {
	va_list args;
	va_start(args, fmt);
	
	const size_t szLen = String_strlen(fmt);
//	std::stringstream ssError;
	
//	_sError = "->";
	_sError.clear();
	
	for (size_t x = 0; x < szLen; ++x) {
	  char c = fmt[x];
	  
	  // format specifier
	  if (c == '%') {
	  	char cc = fmt[++x];
	  	switch (cc) {
	  	  case 'd': {
	  	  	int iTemp = va_arg(args, int);
	  	  	//ssError << iTemp;
	  	  	_sError += to_string(iTemp);
	  	  	break;
	  	  }
	  	  
	  	  case 'f': {
	  	  	double fTemp = va_arg(args, double);
	  	  	//ssError << iTemp;
	  	  	_sError += to_string(fTemp);
	  	  	break;
	  	  }
	  	  
	  	  case 'u': {
	  	  	unsigned int uiTemp = va_arg(args, unsigned int);
	  	  	//ssError << iTemp;
	  	  	_sError += to_string(uiTemp);
	  	  	break;
	  	  }
	  	  
	  	  case 'x': {
	  	  	unsigned int uiTemp = va_arg(args, unsigned int);
	  	  	//ssError << "0x" << hex << iTemp << dec;
	  	  	_sError += to_string(uiTemp); // FIXME should be in hex format...
	  	  	break;
	  	  }
	  	  
	  	  case 'p': {
	  	  	void* vpTemp = va_arg(args, void*);
	  	  	ptrdiff_t iAdr = (ptrdiff_t) vpTemp;
	  	  	//ssError << "0x" << hex << iAdr << dec;
	  	  	_sError += to_string(iAdr);
	  	  	break;
	  	  }
	  	  
	  	  case 's': {
	  	  	const char* sTemp = va_arg(args, const char*);
	  	  	//ssError << sTemp;
	  	  	_sError += sTemp;
	  	  	break;
	  	  }
	  	  
	  	  default:
	  	  	//ssError.put(cc);
	  	  	_sError += cc;
	  	  	break;
	  	}
	  	
	  	// next index
	  	continue;
	  }
	  
	  //ssError.put(c);
	  _sError += c;
	}
	
	va_end(args);
	
//	std::string sError = ssError.str();
	
	// resize the vector to make it fit
//	if ( _vct_error.size() < sError.length() )
//	  _vct_error.resize( sError.length() );
	
	// copy translated string to vector
//	String_strlcpy(ptr, sError.c_str(), _vct_error.size());
}

const char* Log_GetError() {
	//return const_cast<const char*>( _vct_error.data() );
	return _sError.c_str();
}

//void Log_Print(std::string fmt, std::ostream& out, ...) {
//	va_list args;
//	
//	// start reading arguments
//	va_start(args, fmt);
//	
//	// print each character one-by-one
//	for (size_t x = 0; x < fmt.size(); ++x) {
//	  char c = fmt[x];
//	  
//	  // a format tag is found
//	  if (c == '%') {
//	  	char cc = fmt[++x];
//	  	
//	  	switch (cc) {
//	  	  case 'd': {
//	  	  	int val = va_arg(args, int);
//	  	  	cerr << val;
//	  	  	break;
//	  	  }
//	  	  
//	  	  case 'f': {
//	  	  	double val = va_arg(args, double);
//	  	  	cerr << val;
//	  	  	break;
//	  	  }
//	  	  
//	  	  case 'u': {
//	  	  	unsigned int val = va_arg(args, unsigned int);
//	  	  	cerr << val;
//	  	  	break;
//	  	  }
//	  	  
//	  	  case 'x': {
//	  	  	unsigned int val = va_arg(args, unsigned int);
//	  	  	cerr << "0x" << hex << val << dec;
//	  	  	break;
//	  	  }
//	  	  
//	  	  case 'p': {
//	  	  	void* ptr = va_arg(args, void*);
//	  	  	ptrdiff_t val = (ptrdiff_t) ptr;
//	  	  	cerr << "0x" << hex << val << dec;
//	  	  	break;
//	  	  }
//	  	  
//	  	  case 's': {
//	  	  	const char* ptr = va_arg(args, const char*);
//	  	  	cerr << ptr;
//	  	  	break;
//	  	  }
//	  	  
//	  	  default:
//	  	  	cerr.put(cc);
//	  	  	break;
//	  	} // end switch()
//	  	
//	  	// skip to next character
//	  	continue;
//	  }
//	  
//	  // print character
//	  cerr.put(c);
//	}
//}

void _Log_Assert(const bool expr, const char* msg, const char* func, const char* file, unsigned int line)
{
	if (! expr) {
	  Log_Error("Assertion failed in function %s on line %u in %s\nmessage: %s\n",
	            func, line, file, msg);
	  
	  abort();
	}
}

void _Log_Test(const bool expr, const char* testname) {
	std::printf("%-20s:", testname);
	std::printf("Expression %s\n", (expr ? "passed" : "FAILED"));
}
