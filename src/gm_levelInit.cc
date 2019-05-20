// global game data
#include "headers/game.hpp"

// header for this file
#include "headers/gm_levelInit.hpp"

// level data and functions
#include "headers/levelcode.hpp"

// tile collision code
#include "headers/tile_collision.hpp"

// sound system
#include "headers/sound.hpp"

// Tcl system
#include "headers/tcl.hpp"

// texture packs
#include "headers/texture_packs.hpp"

// event triggers
#include "headers/triggers.hpp"

// ability to log output to file
#include "headers/log.hpp"

using namespace std;

int gm_levelInit(void* const vgm, const PROGRAM& pgrm) {
	// interpret the pointer as one to the gamemode struct
	GameMode* gm = (GameMode*) vgm;

	BG_BLACK.blit();

	// change the gamemode if the INIT routine has already went
	if (! gm->tm) {
	  GM_ChangeGamemode(gm, 3, 3);
	  return 0;
	}

	// return if the timer is >= 2
	else if (gm->tm >= 2)
	  return 0;

	// return value
	int retval;

	// load the level file into memory
	retval = level::load(CurrentLevel, pgrm);
	if (retval) return retval;

	// create a list of TexturePack's, shared graphics for all sprites; sets bit indicating these were loaded
	NewTexturePacks(pgrm.renderer);

	// reset the background color
	SDL_SetRenderDrawColor(pgrm.renderer, 0, 0, 0, 0xff);

	// call script that positions the player and spawns sprites
	retval = Tcl_EvalEx(gInterp, "OpenLevelScript", 15, 0);

	if (retval)
	  std::cerr << "Error from OpenLevelScript: " << Tcl_GetStringResult(gInterp) << '\n';

	// call script that defines event triggers in the level
	retval = Tcl_EvalEx(gInterp, "OpenLevelEventFile", 18, 0);

	if (retval)
	  std::cerr << "Error from OpenLevelEventFile: " << Tcl_GetStringResult(gInterp) << '\n';

	// call script that loads screen exits from the file
	retval = Tcl_EvalEx(gInterp, "OpenLevelExitFile", 0, 0);

	if (retval != TCL_OK)
	  std::cerr << "Error from OpenLevelExitFile: " << Tcl_GetStringResult(gInterp) << '\n';
	else {
	  // number of items in the list
	  int iLen;

	  // list of pointers in the same list
	  Tcl_Obj** dpObjv = nullptr;

	  // the interpreter result is a list; extract that list into an array of pointers
	  retval = Tcl_ListObjGetElements(gInterp, Tcl_GetObjResult(gInterp), &iLen, &dpObjv);

	  // check if the operation failed
	  if (retval != TCL_OK) {
	  	std::cerr << "Return value of OpenLevelExitFile is not a valid list: " \
	  	<< Tcl_GetStringResult(gInterp) << '\n';
	  }

	  // in this case, the operation worked
	  else {
	  	// this array holds the names of files attached to each screen exit in the level
	  	// resize the array to account for the number of items in the Tcl list above
	  	using level::screenExits;
	  	screenExits.resize(iLen);

	  	// assign to each element in the array a string of the file name
	  	for (int x = 0; x < iLen; ++x) {
	  	  // retrieve the string from the list
	  	  const char* sFile = Tcl_GetStringFromObj(dpObjv[x], &iLen);

	  	  // reference to the screen exit
	  	  auto itrExit = screenExits.begin() + x;
	  	  itrExit->assign(iLen + 1, '\0');

	  	  // copy string to array
	  	  std::strcpy(itrExit->data(), sFile);
	  	}
	  } // end else
	} // end else

return 0;
}
