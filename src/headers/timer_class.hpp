#ifndef _TIMER_H_INCLUDED
#define _TIMER_H_INCLUDED

#include <SDL.h>

class Timer {
	int StartTime, PausedTime;
	bool Paused, Started;

	public:

	Timer() : StartTime(0), PausedTime(0) {
		Paused = false;
		Started = false;
	}
	
	~Timer() {  }

	// returns the current time; if paused, returns those ticks instead
	int get() const {
	  if ( ! Started ) { return 0; }
	  return ( Paused ) ? PausedTime : SDL_GetTicks()-StartTime;;
	}

	bool is_started() const { return Started; }
	bool is_paused() const { return Started && Paused; }

	void start();
	void stop();
	void pause();
	void resume();

	friend bool operator>(const Timer& _lhs, const int _rhs);
};



#endif // _TIMER_H_INCLUDED
