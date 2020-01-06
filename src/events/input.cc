#include <bitset>
#include <stack>
#include <unordered_map>
#include <array>
#include <algorithm>

#include "input.hpp"
#include "debug.hpp"
#include "arrays.hpp"
#include "memory.hpp"
#include "pdgamemodemanager.h"
#include "pdinputsym.h"

namespace {
	static std::stack<int> _Unpress;
	static StaticDArray<PDInputSym, PDSym_Last> _Inputs;
	static const uint8_t* _KeyStates = nullptr;
	static int _NumKeys = 0;
	static bool _Init = false;
}

static int str2scancode(const std::string& val) {
	std::string sCopy = val;
	bool bIsDigit = (sCopy[0] >= '0' && sCopy[0] <= '9');
	
	/* if first character is a digit, trim the rest of the string */
	if (bIsDigit) {
	  if ( sCopy.length() > 1 ) sCopy.erase(1);
	}
	
	/* if the string is composed of a single character,
	   see if that character is a letter or a digit */
	if ( sCopy.length() == 1 ) {
	  // character is a letter between A and Z
	  if (sCopy[0] >= 'A' && sCopy[0] <= 'Z') {
	  	std::array<char, 26> aLetters = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J',
	  	                                 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T',
	  	                                 'U', 'V', 'W', 'X', 'Y', 'Z'};
	  	
	  	auto found = std::find_if(aLetters.begin(), aLetters.end(),
	  	                          [&sCopy](char c) -> bool {return c == sCopy[0];});
	  	if (found != aLetters.end()) return static_cast<int>(*found - 'A' + SDL_SCANCODE_A);
	  }
	  else if (bIsDigit) {
	  	char c = sCopy[0];
	  	if (c == '0') c = '9' + 1;
	  	return static_cast<int>((c - '0') + SDL_SCANCODE_1 - 1);
	  }
	}
	
	if (val == "LEFT")   return SDL_SCANCODE_LEFT;
	if (val == "RIGHT")  return SDL_SCANCODE_RIGHT;
	if (val == "UP")     return SDL_SCANCODE_UP;
	if (val == "DOWN")   return SDL_SCANCODE_DOWN;
	if (val == "RETURN") return SDL_SCANCODE_RETURN;
	if (val == "RSHIFT") return SDL_SCANCODE_RSHIFT;
	if (val == "LSHIFT") return SDL_SCANCODE_LSHIFT;
	
	return -1;
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
}

/**************************************************/
/* EnableEvents
   Enables one or more events to be processed by
   SDL.
   
   @param type	An SDL_EventType
*/
/**************************************************/
template<typename... Args>
void EnableEvents(uint32_t type, Args... args) {
	EnableEvents(type);
	EnableEvents(args...);
}

template<> void EnableEvents(uint32_t type) {
	SDL_EventState(type, SDL_ENABLE);
}

/**************************************************/
/* Init
   Initializes the input system.
*/
/**************************************************/
void PDInput::Init() {
//	constexpr int ciScancodes[13] = {
//	  0,
//	  SDL_SCANCODE_A, SDL_SCANCODE_B,
//	  SDL_SCANCODE_X, SDL_SCANCODE_Y,
//	  SDL_SCANCODE_LEFT, SDL_SCANCODE_RIGHT,
//	  SDL_SCANCODE_UP, SDL_SCANCODE_DOWN,
//	  SDL_SCANCODE_Q, SDL_SCANCODE_W,
//	  SDL_SCANCODE_RETURN, SDL_SCANCODE_RSHIFT
//	};
	
	Log_Assert(!_Init, "Already initialized the input system");
	if (_Init) return;
	
	_KeyStates = SDL_GetKeyboardState(&_NumKeys);
	Log_Assert(_KeyStates, "Failed to get pointer to keyboard state");
	DisableEvents(SDL_CLIPBOARDUPDATE, SDL_DROPFILE, SDL_DROPTEXT, SDL_DROPBEGIN, SDL_DROPCOMPLETE);
	_Inputs[PDSym_Unknown].type = -1;
	_Init = true;
}

/**************************************************/
/* Shutdown
   Deactivates the input system.
*/
/**************************************************/
void PDInput::Shutdown() {
	if (_Init) {
	  _KeyStates = nullptr;
	  _NumKeys = 0;
	  EnableEvents(SDL_CLIPBOARDUPDATE, SDL_DROPFILE, SDL_DROPTEXT, SDL_DROPBEGIN, SDL_DROPCOMPLETE);
	  _Init = false;
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
	// if keys are marked for unpressing, unpress them
	while ( _Unpress.size() > 0 ) {
	  int iIdx = _Unpress.top();
	  _Unpress.pop();
	  _Inputs[iIdx].pressed = false;
	  _Inputs[iIdx].triggered = false;
	}
}

/**************************************************/
/* Updates the input data by processing events.
   
   @param dt	Time since the last frame in
   		seconds
*/
/**************************************************/
void PDInput::Update(float dt) {
	SDL_Event event;
	Log_Assert(_Init, "updating inputs when system not initialized");
	while ( SDL_PollEvent(&event) ) {
	  switch (event.type) {
	  	case SDL_QUIT:
	  	  PDGamemodeManager::Quit();
	  	  break;
	  	
	  	default: break;
	  }
	}
	
	// check the status of each virtual key
	for (size_t i = 1; i < PDSym_Last; ++i) {
	  PDInputSym& key = _Inputs[i];
	  switch (key.type) {
	  	case -1:
	  	  break;
	  	
	  	case 0:
	  	case 1:
	  	  SetPressed(i, static_cast<int>(_KeyStates[key.sym]));
	  	  break;
	  	
	  	default:
	  	  Log_SetError("%d is an invalid type member for virtual input %u", (int) key.type, i);
	  	  std::runtime_error e( Log_GetError() );
	  	  throw e;
	  	  break;
	  }
	}
}

void PDInput::FromFile(PDIniFile& ini) {
	std::stringstream ssOptNames("A B X Y DLEFT DRIGHT DUP DDOWN LBUTTON RBUTTON START SELECT");
	std::string sOpt;
	std::string sVal;
	int i = 0;
	
	while (ssOptNames >> sOpt) {
	  size_t szColon;
	  
	  // get option value and split it at the ':'
	  ini.GetValue(sOpt, sVal); // format: word:number (example: 'key:100')
	  szColon = sVal.find_first_of(':'); // szColon would be 3 in example's case
	  Log_Assert(szColon > 0, "Invalid key mapping, must be 'key = word:number'");
	  sOpt = sVal.substr(0, szColon); // sOpt == "key" in example
	  sVal.erase(0, szColon+1); // sVal: before = "key:100" after = "100"
	  
	  PDInputSym& inSym = _Inputs[i++];
	  if (sOpt == "key") {
	  	inSym.type = 0;
	  	inSym.sym = str2scancode(sVal);
	  }
	  else if (sOpt == "joy") {
	  	inSym.type = 1;
	  	inSym.sym = math::strtoi(sVal);
	  }
	  else {
	  	sVal = "invalid option '" + std::move(sOpt);
	  	sVal += "' in inputs.ini";
	  	std::invalid_argument e(sVal);
	  	throw e;
	  }
	}
}

//////////////////////////////////////////public functions//////////////////////////////////////////
void PDInput::SetPressed(int key, int status) {
	Log_SetError("invalid key code %d", key);
	Log_Assert(key >= PDSym_A && key < PDSym_Last, Log_GetError());
	
	if (status) {
	  PDInputSym& keysym = _Inputs[key];
	  keysym.triggered = (! keysym.pressed);
	  keysym.pressed = true;
	}
	else {
	  if (_Inputs[key].pressed) _Unpress.push(key);
	}
}

bool PDInput::IsPressed(int key) {
	Log_Assert(key >= PDSym_A && key < PDSym_Last, Log_GetError());
	return _Inputs[key].pressed;
}

bool PDInput::IsTriggered(int key) {
	Log_Assert(key >= PDSym_A && key < PDSym_Last, Log_GetError());
	return _Inputs[key].triggered;
}
