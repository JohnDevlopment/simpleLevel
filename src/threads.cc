#include "headers/threads.hpp"

using namespace std;

string Thread::c__error;

Thread::Thread(SDL_ThreadFunction fn, const char* name) : c__thread(nullptr), c__name(nullptr), c__fn(fn), c__status(0) {
	// throw an exception if this is invalid
	if (fn == nullptr)
	  throw bad_option ("Thread constructor argument 1 is NULL");
	
	// allocate dynamic string of length len + 1
	size_t len = strlen(name);
	c__name = new char[len + 1];
	
	// copy name to c__name and add NULL terminator
	strcpy(c__name, name);
	c__name[len-1] = 0;
}

Thread::~Thread() {
	detach();
	clear();
}

int Thread::create(void* data) {
	int ret = (int) ThreadSuccess; // default return value
	c__error.clear(); // reset error status
	
	// if a thread had to be deleted
	if (c__thread != nullptr) {
	  wait();
	  ret = (int) ThreadActive;
	}
	
	// create a new thread
	c__thread = SDL_CreateThread(c__fn, c__name, data);
	
	if (c__thread == nullptr) {
	  ret = (int) ThreadError;
	  c__error = string("Unable to create thread \"") + c__name;
	  c__error += "\" : ";
	  c__error += SDL_GetError();
	}
	
return ret;
}

void Thread::wait() {
	if (c__thread != nullptr) {
	  SDL_WaitThread(c__thread, &c__status);
	  c__thread = nullptr;
	}
}

void Thread::clear() {
	wait();            // wait for a thread to finish
	delete[] c__name;  // erase name of thread
	c__name = nullptr; //
	c__error.clear();  // clear error status
	c__fn = nullptr;   // undefine function
	c__status = 0;     // clear status
}

bool Thread::assign(SDL_ThreadFunction fn, const char* name) {
	c__error.clear(); // clear error status
	
	// NULL pointer argument
	if (fn == nullptr) {
	  c__error = "Thread::assign: first argument is NULL";
	  return false;
	}
	
	// NULL string argument
	if (name == nullptr) {
	  c__error = "Thread::assign: second argument is NULL";
	  return false;
	}
	
	// if class data not cleared, clear it
	if (c__name != nullptr || c__thread != nullptr)
	  clear();
	
	// name of thread
	size_t len = strlen(name); // length of name
	
	c__name = new char[len + 1]; // dynamic string allocated
	c__name[len-1] = 0; // NULL terminator
	
	strcpy(c__name, name); // copy name to c__name
	
	
	// thread function
	c__fn = fn;
	
return true;
}
