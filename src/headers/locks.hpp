#ifndef LOCKS_HPP_INCLUDED
#define LOCKS_HPP_INCLUDED

#include <iostream>
#include <stdexcept>
#include <utility>
#include <SDL.h>
#include <SDL_thread.h>

class Lock {
	Lock& operator=(const Lock& rhs);
	
	protected:
	
	static std::string c__error;
	
	public:
	
	Lock() {}
	~Lock() {}
	
	int aquire() {return 0;}
	int release() {return 0;}
};

class Mutex : public Lock {
	SDL_mutex* c__mutex;
	
	Mutex& operator=(const Mutex& rhs);
	
	void move(Mutex&& src);
	
	public:
	
	Mutex();
	~Mutex();
	Mutex(const Mutex& _rhs) {  }
	Mutex(Mutex&& _rhs) {this->move( std::move(_rhs) );}
	
	// aquire a lock
	int aquire();
	
	// release the lock
	int release();
	
	// return the lock itself
	SDL_mutex* get() const {return c__mutex;}
};

class Condition : public Lock {
	SDL_cond*	c__cond;
	SDL_bool	c__signal;
	
	Condition& operator=(const Condition& rhs);
	
	void move(Condition&& src);
	
	public:
	
	Condition();
	~Condition();
	Condition(Condition&& _rhs) {this->move(std::move(_rhs));}
	
	// wait until a signal arrives
	void wait(SDL_mutex* mutex, uint32_t timeout = SDL_MUTEX_MAXWAIT);
	
	// broadcast a signal that restarts one thread depending on this lock
	int signal();
	
	// broadcase a signal that restarts all threads depending on this lock
	
	// dummy functions
	int aquire() {return 0;}
	int release() {return 0;}
};

struct MutexCond {
	Mutex	  lock;
	Condition cond;
	SDL_bool  flag;
};

#endif /* LOCKS_HPP_INCLUDED */
