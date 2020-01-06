#include "pdapp.h"
#define OV_EXCLUDE_STATIC_CALLBACKS
#include "sound.hpp"
#include "pdgamemodemanager.h"
#include "game.hpp"
#include "script.hpp"
#include <bitset>
#include "pdgamemodetypes.h"
#include "pdmanager.h"
#include "input.hpp"

/* Namespace for variables that should be accessible to every source file in the program
   at the global scale.
   
   Flags: bit 10 (0x0000 or 0000 0100 0000 0000) = undefined
          bit  9 (0x0200 or 0000 0010 0000 0000) = texture packs have been loaded if this is set
          bit  8 (0x0100 or 0000 0001 0000 0000) = cleanup certain blocks of memory at the gm_level cleanup routine
          bit  7 (0x0080 or 0000 0000 1000 0000) = fade to black
          bit  6 (0x0040 or 0000 0000 0100 0000) = fade from black
          bit  5 (0x0020 or 0000 0000 0010 0000) = music system initialized
          bit  4 (0x0010 or 0000 0000 0001 0000) = level scroll enabled
          bit  3 (0x0008 or 0000 0000 0000 1000) = trigger system activated
          bit  2 (0x0004 or 0000 0000 0000 0100) = quit game (gm_level)
          bit  1 (0x0002 or 0000 0000 0000 0010) = disable input when set
          bit  0 (0x0001 or 0000 0000 0000 0001) = indicates DLOpen has been called and the resource file is loaded
*/

enum {
	SDL_INITTED = 0x01,
	WINDOW_CREATED = 0x02,
	RENDERER_CREATED = 0x04,
	SDL_IMAGE_STARTED = 0x08,
	SCRIPT_SYSTEM_INITTED = 0x10,
	SOUND_SYSTEM_INITTED = 0x20,
	RESOURCES_LOADED = 0x40
};

#define _print_error(f) std::cerr << #f << " : " << SDL_GetError() << '\n';

PDInitData* PDApp::s_initData = nullptr;
PROGRAM PDApp::s_program;
Bitfield<uint8_t> PDApp::s_flags;
bool PDApp::s_isQuitting = false;

PDResourceManager PDApp::s_manager;

void PDApp::DestroyWindow() {
	// destroy renderer & window
	if ( s_flags.mask(RENDERER_CREATED) )
	  SDL_DestroyRenderer(s_program.renderer);
	
	if ( s_flags.mask(WINDOW_CREATED) )
	  SDL_DestroyWindow(s_program.window);
	
	s_flags.unset(RENDERER_CREATED|WINDOW_CREATED);
}

PDInitData& PDApp::GetInitData() {
	return *s_initData;
}

void PDApp::Init(PDInitData& initData) {
	using std::cerr;
	initData.success = false;
	
	// start SDL
	if ( SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER) < 0 ) {
	  _print_error(SDL_Init);
	  return;
	}
	s_flags.set(SDL_INITTED);
	Log_Cout("Initialized SDL library with the video, audio and timer systems activated\n");
	
	// create window
	s_program.window = SDL_CreateWindow(initData.title.c_str(),
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, initData.width, initData.height, 0);
	if (! s_program.window) {
	  _print_error(SDL_CreateWindow);
	  return;
	}
	s_flags.set(WINDOW_CREATED);
	Log_Cout("Window created with title: \"%s\"\n  Screen width: %d\n  Screen height: %d\n",
	         initData.title.c_str(), initData.width, initData.height);
	
	// create renderer
	s_program.renderer = SDL_CreateRenderer(s_program.window, -1, 0);
	if (! s_program.renderer) {
	  _print_error(SDL_CreateRenderer);
	  return;
	}
	s_flags.set(RENDERER_CREATED);
	{
	  SDL_RendererInfo info;
	  SDL_GetRendererInfo(s_program.renderer, &info);
	  Log_Cout("Rendering context created\n  Name: %s\n  Number of texture formats: %d\n  Max texture width: %d\n  ",
	           info.name, info.num_texture_formats, info.max_texture_width);
	  Log_Cout("Max texture height: %d\n", info.max_texture_height);
	}
	
	// SDL2_image
	const int iInitFlags = IMG_INIT_PNG|IMG_INIT_JPG;
	if ( ( IMG_Init(iInitFlags) & iInitFlags ) != iInitFlags ) {
	  SDL_LogError(SDL_LOG_CATEGORY_ERROR, "IMG_Init : %s\n", IMG_GetError() );
	  return;
	}
	s_flags.set(SDL_IMAGE_STARTED);
	Log_Cout("Initialized the SDL_image library with support for BMP, JPEG and PNG files\n");
	
	//////////////////////////////////////////////
	
	// init scripts
	if ( PDScript::Init() < 0 ) {
	  return;
	}
	s_flags.set(SCRIPT_SYSTEM_INITTED);
	Log_Cout("Initialized the scripting engine\n");
	
	// init sound system
	Log_Cout("Initializing the audio system\n  Frequency: 22050hZ\n  Format: signed 16-bit samples in system byte order\n  Stereo audio\n  Chunk size: 2048 bytes\n");
	if ( ! Sound_Init(22050, AUDIO_S16SYS, 2, 2048) ) {
	  return;
	}
	s_flags.set(SOUND_SYSTEM_INITTED);
	
	// initialize the gamemode and object managers
	video::SetRenderer(s_program.renderer);
	s_initData = &initData;
	
	PDGameData gameData = {
		static_cast<unsigned int>(s_initData->fps),
		static_cast<unsigned int>(1000 / s_initData->fps),
		s_initData->fps / 1000.0f,
		static_cast<unsigned int>(s_initData->width),
		static_cast<unsigned int>(s_initData->height)
	};
	PDGamemodeManager::Init(gameData);
	PDGamemodeManager::SetFirstGamemode(PDGM_Splash);
	
	initData.success = true;
}

void PDApp::Shutdown() {
	PDInput::Shutdown();
	PDObjectManager::Shutdown();
	PDGamemodeManager::Shutdown();
	s_manager.Clear();
	
	// delete every resource in the resource manager
	if ( s_flags.mask(RESOURCES_LOADED) ) {
	  s_manager.Clear();
	  Log_Cout("Cleared the resource manager\n");
	}
	
	// quit sound system
	if ( s_flags.mask(SOUND_SYSTEM_INITTED) ) {
	  Sound_Quit();
	  Log_Cout("Deactivated the sound system\n");
	}
	
	// quit script system
	if ( s_flags.mask(SCRIPT_SYSTEM_INITTED) ) {
	  PDScript::Shutdown();
	  Log_Cout("Deactivated the script engine\n");
	}
	
	// quit SDL2_image
	if ( s_flags.mask(SDL_IMAGE_STARTED) ) {
	  IMG_Quit();
	  Log_Cout("Unloaded SDL_image library\n");
	}
	
	DestroyWindow();
	
	// quit SDL
	if ( s_flags.mask(SDL_INITTED) ) {
	  SDL_Quit();
	  Log_Cout("Deactivated all SDL subsystems\n");
	}
	
	s_flags.unset("11111111");
}

void PDApp::Update() {
	while (! s_isQuitting) {
	  PDGamemodeManager::Update();
	  if ( PDGamemodeManager::IsQuitting() ) {
	  	s_isQuitting = true;
	  }
	}
	
	PDGamemodeManager::Shutdown();
	video::ClearComponents();
}
