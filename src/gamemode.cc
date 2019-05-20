// header file
#include "headers/gamemode.hpp"

// global game data
#include "headers/game.hpp"

// math functions
#include "headers/math.hpp"

// defines operator "new" and its options, like std::nothrow
#include <new>

// standard exceptions
#include <stdexcept>

// gamemode headers
#include "headers/gm_splash.hpp"
#include "headers/gm_menu.hpp"
#include "headers/gm_level.hpp"
#include "headers/gm_levelInit.hpp"

using namespace std;

// keeps track of the allocated GameMode pointer
static GameMode* GM_FreeLater = nullptr;

GameMode* GM_GetPointer() {
	return GM_FreeLater;
}

void GM_Main(GameMode* const gm, const PROGRAM& program, int& ret) {
	// list of pointers to gamemode functions
	static const GameMode_Function functions[NUMBER_OF_GAMEMODES] = {
	  gm_splash_screen,
	  gm_menu,
	  gm_levelInit,
	  gm_level,
	  nullptr
	};
	
	// tm == 1 -> gm init routine
	if (gm->tm == 1)
	  gm->gm_cur = gm->gm_next;
	
	// fade to black if the most significant bit is set
	using game::Flags;
	
	if (Flags.mask(FADEOUT)) {
	  if (fade_to_black(5) == 255)
	  	Flags.unset(FADEOUT);
	}
	
	// fade from black if bit 6 is set
	if (Flags.mask(FADEIN)) {
	  if (fade_from_black(5) == 0)
	  	Flags.unset(FADEIN);
	}
	
	// execute gamemode function
	ret |= functions[gm->gm_cur](gm, program);
	
	// decrement timer until it reaches zero
	if (gm->tm > 0)
	  --(gm->tm);
}

void GM_Free() {
	delete GM_FreeLater;
	GM_FreeLater = nullptr;
}

GameMode* GM_NewObj() {
	// if the pointer was already allocated, return
	if (GM_FreeLater != nullptr) {
	  throw std::runtime_error("GM_NewObj was called more than once...prohibited");
	}
	
	// allocate the pointer
	GameMode* ret_ptr = new (std::nothrow) GameMode;
	
	if (ret_ptr == nullptr) {
	  cerr << "could not allocate memory to GameMode pointer\n";
	}
	else {
	  // initialize values
	  ret_ptr->gm_next = 0;
	  ret_ptr->gm_cur  = 0;
	  ret_ptr->tm      = 0;
	  ret_ptr->data    = nullptr;
	  
	  // keep track of pointer
	  GM_FreeLater = ret_ptr;
	}
	
return ret_ptr;
}

uint8_t fade_to_or_from_black(uint8_t rate, bool increment) {
	// current alpha value
	uint8_t alpha = BG_BLACK.get_alpha();
	
	// increment alpha value to a cap of 255
	if (increment)
	  alpha = Inc(alpha, rate, 255);
	
	// decrement alpha value to zero
	else
	  alpha = Dec(alpha, rate);
	
	// set new alpha value
	BG_BLACK.set_alpha(alpha);
	
return alpha;
}
