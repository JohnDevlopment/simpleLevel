#include "input.hpp"
#include <bitset>
#include <stack>
#include "debug.hpp"
#include <unordered_map>
#include "arrays.hpp"

namespace {
	static const uint8_t* _KeyStates = nullptr;
	static std::stack<PDKeyCode> _Unpress;
	static bool _Init = false;
	static int _NumKeys = 0;
}

/**************************************************/
/* DisableEvents
   Disables one or more events from being processed
   by SDL.
   
   @param type	An SDL_EventType
*/
/**************************************************/
template<typename... Args>
void DisableEvents(uint32_t type, Args... args) {
	DisableEvents(type);
	DisableEvents(args...);
}

template<> void DisableEvents(uint32_t type) {
	SDL_EventState(type, SDL_IGNORE);
	std::cout << type << '\n';
}

/**************************************************/
/* ClearKeyStack
   Clears the unpress stack.
*/
/**************************************************/
INLINE void ClearKeyStack() {
	while (! _Unpress.empty() ) {
	  _Unpress.pop();
	}
}

/**************************************************/
/* Init
   Initializes the input system.
*/
/**************************************************/
void PDInput::Init() {
	Log_Assert(!_Init, "Already initialized the input system");
	if (_Init) return;
	
	_KeyStates = SDL_GetKeyboardState(&_NumKeys);
	Log_Assert(_KeyStates, "Failed to get pointer to keyboard state");
	_Init = true;
	
	DisableEvents(SDL_CLIPBOARDUPDATE, SDL_DROPFILE, SDL_DROPTEXT, SDL_DROPBEGIN, SDL_DROPCOMPLETE);
}

/**************************************************/
/* Shutdown
   Deactivates the input system.
*/
/**************************************************/
void PDInput::Shutdown() {
	if (_Init) {
	  _Init = false;
	  _KeyStates = nullptr;
	  _NumKeys = 0;
	}
}

/**************************************************/
/* Reset
   Resets the inputs.
   
   @param dt	Time since the last frame in
   		seconds
*/
/**************************************************/
void PDInput::Reset(float dt) {
	ClearKeyStack();
}

/**************************************************/
/* Updates the input data by processing events.
   
   @param dt	Time since the last frame in
   		seconds
*/
/**************************************************/
void PDInput::Update(float dt) {
	Log_Assert(_Init, "updating inputs when system not initialized");
	SDL_PumpEvents();
}

/*
SDL_Scancode PDInput::TranslateKey(PDKeyCode code) {
	constexpr int c_iDiff = SDL_SCANCODE_A - PDKey_A;
	
	SDL_Scancode sc = SDL_SCANCODE_UNKNOWN;
	
	if (code >= PDKey_A && code <= PDKey_0) {
	  int iKey = (int) code + c_iDiff;
	  sc = (SDL_Scancode) iKey;
	}
	else {
	  switch (code) {
	  	case PDKey_Left:
	  	  sc = SDL_SCANCODE_LEFT;
	  	  break;
	  
	  	case PDKey_Right:
	  	  sc = SDL_SCANCODE_RIGHT;
	  	  break;
	  
	  	case PDKey_Up:
	  	  sc = SDL_SCANCODE_UP;
	  	  break;
	  
	  	case PDKey_Down:
	  	  sc = SDL_SCANCODE_DOWN;
	  	  break;
	  }
	}
	
return sc;
}*/

/*
SDL_Scancode PDInput::TranslateKey(PDKeyCode code) {
	SDL_Scancode sc;
	
	switch (code) {
	  case PDKey_0:
	  	sc = SDL_GetScancodeFromKey(SDLK_0);
	  	break;

	  case PDKey_1:
	  	sc = SDL_GetScancodeFromKey(SDLK_1);
	  	break;

	  case PDKey_2:
	  	sc = SDL_GetScancodeFromKey(SDLK_2);
	  	break;

	  case PDKey_3:
	  	sc = SDL_GetScancodeFromKey(SDLK_3);
	  	break;

	  case PDKey_4:
	  	sc = SDL_GetScancodeFromKey(SDLK_4);
	  	break;

	  case PDKey_5:
	  	sc = SDL_GetScancodeFromKey(SDLK_5);
	  	break;

	  case PDKey_6:
	  	sc = SDL_GetScancodeFromKey(SDLK_6);
	  	break;

	  case PDKey_7:
	  	sc = SDL_GetScancodeFromKey(SDLK_7);
	  	break;

	  case PDKey_8:
	  	sc = SDL_GetScancodeFromKey(SDLK_8);
	  	break;

	  case PDKey_9:
	  	sc = SDL_GetScancodeFromKey(SDLK_9);
	  	break;

	  case PDKey_A:
	  	sc = SDL_GetScancodeFromKey(SDLK_a);
	  	break;

	  case PDKey_B:
	  	sc = SDL_GetScancodeFromKey(SDLK_b);
	  	break;

	  case PDKey_C:
	  	sc = SDL_GetScancodeFromKey(SDLK_c);
	  	break;

	  case PDKey_D:
	  	sc = SDL_GetScancodeFromKey(SDLK_d);
	  	break;

	  case PDKey_E:
	  	sc = SDL_GetScancodeFromKey(SDLK_e);
	  	break;

	  case PDKey_F:
	  	sc = SDL_GetScancodeFromKey(SDLK_f);
	  	break;

	  case PDKey_G:
	  	sc = SDL_GetScancodeFromKey(SDLK_g);
	  	break;

	  case PDKey_H:
	  	sc = SDL_GetScancodeFromKey(SDLK_h);
	  	break;

	  case PDKey_I:
	  	sc = SDL_GetScancodeFromKey(SDLK_i);
	  	break;

	  case PDKey_J:
	  	sc = SDL_GetScancodeFromKey(SDLK_j);
	  	break;

	  case PDKey_K:
	  	sc = SDL_GetScancodeFromKey(SDLK_k);
	  	break;

	  case PDKey_L:
	  	sc = SDL_GetScancodeFromKey(SDLK_l);
	  	break;

	  case PDKey_M:
	  	sc = SDL_GetScancodeFromKey(SDLK_m);
	  	break;

	  case PDKey_N:
	  	sc = SDL_GetScancodeFromKey(SDLK_n);
	  	break;

	  case PDKey_O:
	  	sc = SDL_GetScancodeFromKey(SDLK_o);
	  	break;

	  case PDKey_P:
	  	sc = SDL_GetScancodeFromKey(SDLK_p);
	  	break;

	  case PDKey_Q:
	  	sc = SDL_GetScancodeFromKey(SDLK_q);
	  	break;

	  case PDKey_R:
	  	sc = SDL_GetScancodeFromKey(SDLK_r);
	  	break;

	  case PDKey_S:
	  	sc = SDL_GetScancodeFromKey(SDLK_s);
	  	break;

	  case PDKey_T:
	  	sc = SDL_GetScancodeFromKey(SDLK_t);
	  	break;

	  case PDKey_U:
	  	sc = SDL_GetScancodeFromKey(SDLK_u);
	  	break;

	  case PDKey_V:
	  	sc = SDL_GetScancodeFromKey(SDLK_v);
	  	break;

	  case PDKey_W:
	  	sc = SDL_GetScancodeFromKey(SDLK_w);
	  	break;

	  case PDKey_X:
	  	sc = SDL_GetScancodeFromKey(SDLK_x);
	  	break;

	  case PDKey_Y:
	  	sc = SDL_GetScancodeFromKey(SDLK_y);
	  	break;

	  case PDKey_Z:
	  	sc = SDL_GetScancodeFromKey(SDLK_z);
	  	break;
	  
	  case PDKey_Left:
	  	sc = SDL_SCANCODE_LEFT;
	  	break;
	  
	  case PDKey_Right:
	  	sc = SDL_SCANCODE_RIGHT;
	  	break;
	  
	  case PDKey_Up:
	  	sc = SDL_SCANCODE_UP;
	  	break;
	  
	  case PDKey_Down:
	  	sc = SDL_SCANCODE_DOWN;
	  	break;
	  
	  default:
	  	sc = SDL_SCANCODE_ESCAPE;
	  	break;
	}
	
	return sc;
}
*/
