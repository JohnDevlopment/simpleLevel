#include "game.hpp"
#include "gm_levelInit.hpp"
#include "levelcode.hpp"
#include "log.hpp"
#include "sound.hpp"
#include "tcl.hpp"
#include "tile_collision.hpp"
#include "triggers.hpp"
#include "pdresourcemanager.hpp"

using namespace std;

int gm_levelInit(void* const vgm, const PROGRAM& pgrm) {
	string filename = "levels/rt1-1.dat";
//	string filename = string("levels/") + level::CurrentLevel;
	GameMode* gm = (GameMode*) vgm;
	
	PDTexture& blackScreen = RManager.LoadTexture(FADER, "images/ui/black.bmp", nullptr);
	
	// change the gamemode if the INIT routine has already went
	if (! gm->tm) {
	  GM_ChangeGamemode(gm, 3, 3);
	  return 0;
	}
	
	// return if the timer is >= 2
	else if (gm->tm >= 2)
	  return 0;
	
	// load the level file into memory
	if ( level::load( std::move(filename), pgrm ) ) {
	  level::unload();
	  return 1;
	}
	
	// reset the background color
	SDL_SetRenderDrawColor(pgrm.renderer, 0, 0, 0, 0xff);
	
return 0;
}
