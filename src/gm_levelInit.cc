#include "game.hpp"
#include "gm_levelInit.hpp"
#include "levelcode.hpp"
#include "tile_collision.hpp"
#include "sound.hpp"
#include "tcl.hpp"
#include "texture_packs.hpp"
#include "triggers.hpp"
#include "log.hpp"

using namespace std;

int gm_levelInit(void* const vgm, const PROGRAM& pgrm) {
	GameMode* gm = (GameMode*) vgm;
	int retval;
	
	BG_BLACK.blit();
	
	// change the gamemode if the INIT routine has already went
	if (! gm->tm) {
	  GM_ChangeGamemode(gm, 3, 3);
	  return 0;
	}
	
	// return if the timer is >= 2
	else if (gm->tm >= 2)
	  return 0;
	
	// load the level file into memory
	retval = level::load(level::CurrentLevel, pgrm);
	if (retval)
	  return retval;
	
	// reset the background color
	SDL_SetRenderDrawColor(pgrm.renderer, 0, 0, 0, 0xff);
	
return 0;
}
