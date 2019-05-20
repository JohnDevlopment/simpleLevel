// checks to make sure certain marcos are defined
#include "headers/check_global_defines.hpp"

// includes headers that all source files need in this project
#include "headers/stdinc.h"

// game namespace: holds program-wide resources
#include "headers/game.hpp"

// includes the timer class declaration
#include <custom/timer_class.hpp>

// std::vector
#include <vector>

// functions that modify or query containers
#include <algorithm>

// includes all the necessary SDL headers
#include "headers/sdl_incs.h"

/* Includes all the necessary includes, defines, and function declarations
   for the loading, building, and interactions with levels.
   Includes gamemode.hpp, program.hpp and global_attributes.hpp */
#include "headers/levelcode.hpp"

// includes Tcl headers. Also declares functions that use Tcl
#include "headers/tcl.hpp"

// functions to interact with a binary resource file
#include "headers/res.hpp"

// sound and music functions: initialization, playback, etc.
#include "headers/sound.hpp"

// game modes, the primary subdivision of this game's functionality
#include "headers/gamemode.hpp"

// logging functions
#include "headers/log.hpp"

// includes headers specific to the current operating system
#include "headers/windows_or_linux.hpp"

// event parser
#include "headers/event.hpp"

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

	// gamemode object
	GameMode* gm = GM_NewObj();

	if (gm == nullptr)
	  return 1;

	gm->tm = 1; // current gamemode init routine

	// initialize the sound engine
//	if (! Sound_Init()) return 1;

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
	int res = SDL_Init(SDL_INIT_VIDEO /*| SDL_INIT_AUDIO*/ | SDL_INIT_TIMER);
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

	// sdl2_mixer
//	{
//	  res = Mix_Init(MIX_INIT_OGG);

//	  if ( (res & MIX_INIT_OGG) != MIX_INIT_OGG ) {
//	  	cerr << "Mix_Init: " << Mix_GetError() << endl;
//	  	return -1;
//	  }

//	  // open audio channel
//	  res = Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 2, CHUNKSIZE);

//	  if (res < 0) {
//	  	cerr << "Mix_OpenAudio: " << Mix_GetError() << endl;
//	  	return -1;
//	  }
//	}

	// log results of this function for debugging purposes
	Log_Cout("Initialized SDL2, SDL2_image and SDL2_mixer: SDL2 with a window size of %dx%d pixels; SDL2_image with PNG and JPEG support; ",
	 WIDTH, HEIGHT);
	
	Log_Cout("and SDL2_mixer with a frequency of %u hz, 2 channels, and a chunk size of %d bytes.\n",
	 MIX_DEFAULT_FREQUENCY, CHUNKSIZE);

	Log_Cout("SDL2 initialized with video, audio and timer subsystems activated\n");

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
//	Sound_Quit();

	// destroy the Tcl interpreter
	TclCC_Quit();


	// quit central systems

	// destroy window and data associated with it
	destroyProgram(program);

	// close the audio channel and quit SDL2_mixer
//	Mix_CloseAudio();
//	Mix_Quit();

	// quit SDL2_image
	IMG_Quit();

	// quit SDL2 proper
	SDL_Quit();
}
