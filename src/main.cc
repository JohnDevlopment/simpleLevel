#include "check_global_defines.hpp"
#include "stdinc.h"
#include "sdl_incs.h"
#include "memory.hpp"
#include "game.hpp"
#include "tcl.hpp"
#include "res.hpp"
#include "levelcode.hpp"
#include "gamemode.hpp"
#include "event.hpp"
#include "log.hpp"
#include "windows_or_linux.hpp"
#include <timer_class.hpp>
#include <vector>
#include <algorithm>

/* Includes definitions for SFXData and MusData.
   Includes function declarations and links to
   the headers for the Ogg Vorbis library.
   OV_EXCLUDE_STATIC_CALLBACKS is defined to stop
   vorbis.h from defining static globals in this file.
*/
#define OV_EXCLUDE_STATIC_CALLBACKS
#include "sound.hpp"

// private
static PROGRAM program;

// private function declarations
static int loadSDL() COLD;
static void quit() COLD;

/*
Defines set in makefile: WIDTH, HEIGHT, TILE_WIDTH, TILE_HEIGHT, FPS
*/

#define TICKS		1000/FPS
#define CHUNKSIZE	4096

using namespace std;

int main (int argc, char* argv[]) {
	atexit(quit);

	// initialize Log output
	Log_Init();

	// load Tcl library
	if (TclCC_Init(&program)) return 1;

	// load SDL library
	if (loadSDL() < 0)
	 return 1;

	// load global game resources
	if (! game::loadMedia(program.renderer))
	  return 1;

	// initialize the sound engine
	if (! Sound_Init(22050, AUDIO_S16SYS, 2, 2048)) return 1;

	// gamemode object
	GameMode* gm = GM_NewObj();

	if (gm == nullptr)
	  return 1;

	gm->tm = 1; // current gamemode init routine

	// set background color black
	SDL_SetRenderDrawColor(program.renderer, 0, 0, 0, 0xff);

	// event loop
	Timer fps;
	generic_class<SDL_Event> event;
	int ret = 0;

	while (! (ret & 1)) {
	  // start the timer
	  fps.start();

	  // parse all events
	  while (SDL_PollEvent(event.getp()) != 0) {
	  	// user clicks on the 'X' button on the titlebar
	  	if (event->type == SDL_QUIT) {
	  	  ret = 1;
	  	  game::Flags.set(LEVEL_CLEANUP | QUITGAME);
	  	  break;
	  	}

	  	// parse events in each gamemode
	  	ret = Event_Main(event.get(), gm, 0);
	  }

	  // parse events a second time to check different keyboard inputs
	  if (gm->gm_cur == 3 && ret != 1)
	  	ret = Event_Main(event.get(), gm, 1);

	  // clear screen
	  SDL_RenderClear(program.renderer);

	  // control all gamemodes
	  GM_Main(gm, program, ret);

	  // render graphics
	  SDL_RenderPresent(program.renderer);

	  // how many ticks have passed this frame?
	  const unsigned int ticks = fps.get();

	  // increment frame counter
	  ++game::FrameCounter;

	  // wait for the rest of the frame
	  if (ticks < TICKS) SDL_Delay(TICKS - ticks);
	}

	// execute the current gamemode's cleanup routine
	gm->tm = 2;
	GM_Main(gm, program, ret);

return 0;
}

// video: https://www.youtube.com/watch?v=4nShTeUEJIQ
//
/*float rsqrt(float base) {
	long int i;
	float x2, y;
	const float threehalfs = 1.5f;

	x2 = base * 0.5f;
	y  = base;
	i  = * (long int*) &y;
	i  = 0x5f3759df - (i >> 1);
	y  = * (float*) &i;
	y  = y * (threehalfs - (x2 * y * y));
//	y  = y * (threehalfs - (x2 * y * y));

return y;
}*/

// private functions //
int loadSDL() {
	// initialize SDL
	int res = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER);
	if (res != 0) {
	  cerr << "SDL_Init: " << SDL_GetError() << '\n';
	  return -1;
	}

	// create a new window
	program.window = SDL_CreateWindow("Project Dewpsi Development", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		WIDTH, HEIGHT, 0);

	if (program.window == nullptr) {
	  cerr << "SDL_CreateWindow: " << SDL_GetError() << '\n';
	  return -1;
	}

	// create a new renderer
	program.renderer = SDL_CreateRenderer(program.window, -1, 0);

	if (program.renderer == nullptr) {
	  cerr << "SDL_CreateRenderer: " << SDL_GetError() << '\n';
	  return -1;
	}

	// sdl2_image
	{
	  const int flags = IMG_INIT_PNG|IMG_INIT_JPG;
	  res = IMG_Init(flags);
	  if ((res & flags) != flags) {
	  	cerr << "IMG_Init: " << IMG_GetError() << '\n';
	  	return -1;
	  }
	}

	// 
	{
	  unique_ptr<int[]> vals(new int[4]);
	  
	  SDL_GetWindowPosition(program.window, vals.get(), vals.get() + 1);
	  SDL_GetWindowSize(program.window, vals.get() + 2, vals.get() + 3);
	  
	  Log_Cout("Created window with title: \"%s\". Location: %d, %d. Size: %dx%d\n",
	  	SDL_GetWindowTitle(program.window), vals[0], vals[1], vals[2], vals[3]);
	}

return 0;
}

void quit() {
	// clear data

	// delete the player object
	delete level::ThePlayer;

	// free the gamemode pointer that was created earlier
	GM_Free();

	// free globally shared data that's used by the program
	game::free();


	// quit game systems

	// shutdown the sound system
	Sound_Quit();

	// destroy the Tcl interpreter
	TclCC_Quit();


	// quit central systems

	// destroy window and data associated with it
	destroyProgram(program);

	// quit SDL2_image
	IMG_Quit();

	// quit SDL2 proper
	SDL_Quit();
}
