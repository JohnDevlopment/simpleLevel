#include "headers/locks.hpp"

using std::string;

string Lock::c__error;

// class Mutex
Mutex::Mutex() {
	c__mutex = SDL_CreateMutex();
	
	if (c__mutex == nullptr) {
	  string error = string("Error creating mutex for class Mutex: ") + SDL_GetError();
	  throw std::runtime_error (error);
	}
}

Mutex::~Mutex() {
	SDL_DestroyMutex(c__mutex);
}

void Mutex::move(Mutex&& src) {
	c__mutex = src.c__mutex;
	src.c__mutex = nullptr;
}

int Mutex::aquire() {
	c__error.clear();
	
	int res = SDL_LockMutex(c__mutex);
	
	if (res < 0) {
	  c__error = "Failed to aquire lock: ";
	  c__error += SDL_GetError();
	}
	
return res;
}

int Mutex::release() {
	c__error.clear();
	
	int res = SDL_UnlockMutex(c__mutex);
	
	if (res < 0) {
	  c__error = "Failed to release lock: ";
	  c__error += SDL_GetError();
	}
	
return res;
}

// class Condition
Condition::Condition() : c__signal(SDL_FALSE) {
	// create the condition variable or throw an exception if it fails
	c__cond = SDL_CreateCond();
	
	if (c__cond == nullptr) {
	  string error = "Runtime error on Condition constructor: cannot create condition -- ";
	  error += SDL_GetError();
	  throw std::runtime_error (error);
	}
}

Condition::~Condition() {
	SDL_DestroyCond(c__cond);
}

void Condition::move(Condition&& src) {
	c__cond = src.c__cond;
	src.c__cond = nullptr;
	
	c__signal = src.c__signal;
	src.c__signal = SDL_FALSE;
}

void Condition::wait(SDL_mutex* mutex, uint32_t timeout) {
	// reset condition
//	c__signal = SDL_FALSE;
	
	// wait for condition to be met
//	while (! c__signal)
	  SDL_CondWaitTimeout(c__cond, mutex, timeout);
}

int Condition::signal() {
	c__error.clear(); // clear error status
	
//	c__signal = SDL_TRUE; // condition is met
	
	int res = SDL_CondSignal(c__cond);
	
	if (res < 0) {
	  c__error = "Error broadcasting a signal for this condition variable";
	  c__error += SDL_GetError();
	}
	
return res;
}
