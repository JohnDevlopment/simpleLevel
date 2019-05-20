#ifndef THREADS_HPP_INCLUDED
#define THREADS_HPP_INCLUDED

#include <iostream>
#include <SDL.h>
#include <SDL_thread.h>
#include <sstream>
#include <utility>
#include <cstring>
#include <custom/bad_option>

enum {
	ThreadError	= -1,
	ThreadSuccess	= 0,
	ThreadActive	= 1
};

class Thread {
	SDL_Thread*		c__thread;
	char*			c__name;
	SDL_ThreadFunction	c__fn;
	int			c__status;
	
	static std::string	c__error;
	
	// disable copy constructor and operator
	Thread& operator=(const Thread& rhs);
	
	public:
	
	/* How to initialize this class: the first argument is the name of the function the class
	should use every time it creates a thread. The second argument is a string denoting the name
	of the thread. */
	Thread(SDL_ThreadFunction fn, const char* name); // ctor
	
	~Thread(); // dtor
	
	// move ctor
	Thread(Thread&& _rhs) {}
	
	// initialize thread
	int create(void* data = nullptr);
	
	// wait for a thread to finish
	void wait();
	
	// clear all data
	void clear();
	
	// assign new function and name to the class
	bool assign(SDL_ThreadFunction fn, const char* name);
	
	// detach the thread and clear its pointer
	void detach() {
		SDL_DetachThread(c__thread);
		c__thread = nullptr;
	}
	
	// get the current error message
	static const char* get_error() {return c__error.c_str();}
};

#endif /* THREADS_HPP_INCLUDED */
