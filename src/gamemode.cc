// engine headers
#include "gamemode.hpp"
#include "game.hpp"
#include "textures.h"
#include "log.hpp"
#include "pdresourcemanager.hpp"

// other headers
#include "math.hpp"
#include <new>
#include <stdexcept>
#include "memory.hpp"
#include "lvalue_rvalue_pointers.hpp"

// gamemode headers
#include "gm_splash.hpp"
#include "gm_menu.hpp"
#include "gm_level.hpp"
#include "gm_levelInit.hpp"

using namespace std;

// keeps track of the allocated GameMode pointer
static GameMode* GM_FreeLater = nullptr;

// texture of a black screen
static PDTexture* GM_BlackScreen = nullptr;

GameMode* GM_GetPointer() {
	return GM_FreeLater;
}

void GM_Main(GameMode* const gm, const PROGRAM& program, int& ret) {
	using game::Flags;
	
	static const GameMode_Function functions[NUMBER_OF_GAMEMODES] = {
	  gm_splash_screen,
	  gm_menu,
	  nullptr,
	  nullptr,
	  nullptr
	};
	
	// tm == 1 -> gm init routine
	if (gm->tm == 1)
	  gm->gm_cur = gm->gm_next;
	
	// fade to black if the most significant bit is set
	if (Flags.mask(FADEOUT)) {
	  if (fade_to_black(6) == 255)
	  	Flags.unset(FADEOUT);
	}
	
	// fade from black if bit 6 is set
	else if (Flags.mask(FADEIN)) {
	  if (fade_from_black(6) == 0)
	  	Flags.unset(FADEIN);
	}
	
	// execute gamemode function
	Log_Assert(functions[gm->gm_cur] != nullptr, "Current gamemode function is NULL");
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
	GameMode* ret_ptr;
	
	// if the pointer was already allocated, return
	if (GM_FreeLater) {
	  return GM_FreeLater;
	}
	
	// allocate the pointer
	ret_ptr = new (std::nothrow) GameMode;
	
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

/* Increases or decreases the alpha value of the black screen. */
uint8_t fade_to_or_from_black(uint8_t rate, bool increment) {
	if (! GM_BlackScreen) {
	  GM_BlackScreen = get_pointer( RManager.LoadTexture(FADER, "images/ui/black.bmp", nullptr) );
	}
	
	uint8_t uiAlpha = GM_BlackScreen->GetAlpha();
	
	// increase or decrease alpha value
	if (increment)
	  uiAlpha = Inc(uiAlpha, rate, 255);
	else
	  uiAlpha = Dec(uiAlpha, rate);
	
	GM_BlackScreen->SetAlpha(uiAlpha);
	
return uiAlpha;
}
