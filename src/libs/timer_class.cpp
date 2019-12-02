#include "timer_class.hpp"

void Timer::start() {
	// set values
	StartTime = SDL_GetTicks();
	PausedTime = 0;

	// start timer
	Started = true;
	Paused = false;
}

void Timer::stop() {
	// clear values
	StartTime = 0;
	PausedTime = 0;

	// stop timer
	Started = false;
	Paused = false;
}

void Timer::pause() {
	if (Started) {
	  Paused = true;
	  
	  PausedTime = SDL_GetTicks() - StartTime;
	  StartTime = 0;
	}
}

void Timer::resume() {
	if (Started && Paused) {
	  // reset values
	  StartTime = SDL_GetTicks() - PausedTime;
	  PausedTime = 0;

	  // resume timer
	  Paused = false;
	}
}

bool operator>(const Timer& _lhs, const int _rhs) {return _lhs.get() > _rhs;}
