// standard headers
#include "stdinc.h"
#include "sdl_incs.h"
#include "memory.hpp"
#include "check_global_defines.hpp"

// engine headers
#include "script.hpp"
#include "game.hpp"
#include "tcl.hpp"
#include "res.hpp"
#include "log.hpp"
#include "gamemode.hpp"
#include "windows_or_linux.hpp"
#include "levelcode.hpp"
#include "event.hpp"
/* Includes definitions for SFXData and MusData.
   Includes function declarations and links to
   the headers for the Ogg Vorbis library.
   OV_EXCLUDE_STATIC_CALLBACKS is defined to stop
   vorbis.h from defining static globals in this file.
*/
#define OV_EXCLUDE_STATIC_CALLBACKS
#include "sound.hpp"
#include "pdresourcemanager.hpp"

// external code headers
#include "timer_class.hpp"

// resource manager
PDResourceManager RManager;

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

namespace game {
	Bitfield<uint16_t> Flags;
	uint8_t FrameCounter = 0;
	
	#define HASHFUNC_H_IMPLEMENTATION 1
	#include "hashfunc.h"
	#undef HASHFUNC_H_IMPLEMENTATION
}

int main (int argc, char* argv[]) {
	Timer fps;
	generic_class<SDL_Event> event;
	int ret = 0;
	
	atexit(quit);
	
	// initialize Log output
	Log_Init();
	
	// load SDL library
	Log_Cout("Initializing SDL Library\n");
	if (loadSDL() < 0)
	  return 1;
	
	// load scripting engine
	Log_Cout("Initializing Scripting Engine\n");
	if (PDScript::Init(program) < 0)
	  return 1;
	
	// initialize the sound engine
	Log_Cout("Initializing Sound Engine:\n    Initialized Ogg and WAV decoders\n");
	if (! Sound_Init(22050, AUDIO_S16SYS, 2, 2048))
	  return 1;
	
	// register renderer
	video::SetRenderer(program.renderer);
	
	// gamemode object
	GameMode* gm = GM_NewObj();
	if (gm == nullptr)
	  return 1;
	
	gm->tm = 1; // current gamemode init routine
	
	// set background color black
	SDL_SetRenderDrawColor(program.renderer, 0, 0, 0, 255);
	
	Log_Cout("Initialization stage complete...now entering main loop\n");
	
	// event loop
	while (! (ret & 1)) {
	  unsigned int ticks;
	  
	  // start the timer
	  fps.start();
	  
	  // parse all events
	  while (SDL_PollEvent(event.getp())) {
	  	// user clicks on the 'X' button on the titlebar
	  	if (event->type == SDL_QUIT) {
	  	  ret = 1;
	  	  
	  	  // initiate level cleanup and quit the game
	  	  game::Flags.set(LEVEL_CLEANUP|QUITGAME);
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
	  
	  // increment frame counter
	  ++game::FrameCounter;
	  
	  // how many ticks have passed this frame?
	  ticks = fps.get();
	  
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
	int iReturn;
	
	// initialize SDL
	iReturn = SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO|SDL_INIT_TIMER);
	if (iReturn) {
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
	
	Log_Cout("    Created window; pointer to window data: %p\n    Created rendering context: address starts at %p\n",
	         program.window, program.renderer);
	
	// sdl2_image
	{
	  const int iFlags = IMG_INIT_PNG|IMG_INIT_JPG;
	  
	  iReturn = IMG_Init(iFlags);
	  if ((iReturn & iFlags) != iFlags) {
	  	cerr << "IMG_Init: " << IMG_GetError() << '\n';
	  	return -1;
	  }
	}

	// log to file what the window`s dimensions are
	{
	  //unique_ptr<int[]> vals(new int[4]);
	  int vals[4];
	  
	  SDL_GetWindowPosition(program.window, &vals[0], &vals[1]);
	  SDL_GetWindowSize(program.window, &vals[2], &vals[3]);
	  
	  Log_Cout("    Started video, audio, and timer subsystems\n");
	  Log_Cout("    Created window with title: \"%s\". Location: %d, %d. Size: %dx%d\n",
	           SDL_GetWindowTitle(program.window), vals[0], vals[1], vals[2], vals[3]);
	}

return 0;
}

void quit() {
	RManager.Clear(); // free textures from manager
	Sound_Quit(); // quit sound system
	PDScript::Shutdown(); // close script engine
	GM_Free(); // free gamemode structure
	destroyProgram(program); // destroy window
	IMG_Quit(); // stop SDL2_image
	SDL_Quit(); // stop SDL2
}
