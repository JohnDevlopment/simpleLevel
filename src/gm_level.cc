#include "game.hpp"
#include "levelcode.hpp"
#include "levelinfo.hpp"
#include "event_defines.hpp"
#include "sound.hpp"
#include "tile_collision.hpp"
#include "camera.hpp"
#include "sprite_router.hpp"
#include "triggers.hpp"
#include "tcl.hpp"
#include "sprites/sprite000_def.h"

#define NDEBUG
#include "log.hpp"
#include "math.hpp"
#include "gm_level.hpp"

#include "private/player_data_def.h"
#include "particle_sprite_def.h"

using namespace std;
using level::ThePlayer;
using level::levelEvents;
using level::update;
using game::HeapStack;

// private function prototypes
static int _gm_level_cleanup(GameMode* const, const PROGRAM&);
static int _gm_level_main(GameMode* const, const PROGRAM&);
static int _gm_level_init(GameMode* const, const PROGRAM&);

// priority list of events to process
static const uint8_t _key_sym[] = {
	GMLevel_Key_s, GMLevel_Key_Left, GMLevel_Key_Right,
	GMLevel_Key_Up, GMLevel_Key_Down, GMLevel_Key_l, GMLevel_Key_Escape,
	GMLevel_Key_Buffer
};

// list of scancodes
static StaticDArray<SDL_Scancode,10> _scancodes = {
	SDL_SCANCODE_UNKNOWN, SDL_SCANCODE_UNKNOWN, SDL_SCANCODE_UNKNOWN,
	SDL_SCANCODE_UNKNOWN, SDL_SCANCODE_UNKNOWN, SDL_SCANCODE_UNKNOWN,
	SDL_SCANCODE_UNKNOWN, SDL_SCANCODE_UNKNOWN, SDL_SCANCODE_UNKNOWN,
	SDL_SCANCODE_UNKNOWN
};

// data to forward to Player::Main
static generic_class<_PlayerData> _player_data;

// level music
static MusData* LevelMusic = nullptr;

void _gm_level_saveMusData(MusData* mus) {
	LevelMusic = mus;
}

MusData* _gm_level_getMusData() {return LevelMusic;}

//////////////// functions to help: gm_level MAIN routine /////////////////////
static uint32_t timerCallback_playLevelMusic(uint32_t interval, void* data) {
	
	if ( Sound_PlayingMusic() ) {
	  if ( Sound_PausedMusic() ) {
	  	Sound_ResumeMusic();
	  }
	}
	else {
	  if (LevelMusic) {
	  	Sound_PlayMusic(LevelMusic, -1);
	  }
	}

return 0;
}

/* Notes to self: firstly, uipKeyBits is a field of bits that each correspond to a key being pressed. Mask the bits
   using _key_sym_bitmasks indexed by a value of GMLevel_KeyListSym (header: gm_level_defs.h). That is how you
   can tell what inputs have been inputted. */
static int _main_normal(GameMode* const gm, const PROGRAM&) {
	using namespace level;
	using game::FrameCounter;
	using game::Flags;
	
	// update the level
	update(gm, _player_data.getp(), 0);
	
	// render the black screen if the right flags are set
	if (Flags.mask(FADING | QUITGAME))
	  BG_BLACK.blit();
	
	// if the user is quitting the game...
	if (Flags.mask(QUITGAME)) {
	  // activate the level's cleanup routine
	  if (! Flags.mask(LEVEL_CLEANUP)) {
	  	Flags.set(LEVEL_CLEANUP);
	  	GM_ChangeGamemode(gm, 0, 75);
	  }
	}
	
return 0;
}

static int _main_dialogue(GameMode* const gm, const PROGRAM& program) {
//	using level::update;
//	using game::Flags;

//	// update the level
//	update(gm, 1);

//	// render the dialogue box
//	BG_DIALOGUEBOX.blit(0, 328);

//	auto* pEvent = (EventTrigger*) gm->data;
//	const DialogueEvent& rDia = pEvent->dialogue;

//	// render the text above the dialogue box
//	game::Bitmap->render(program.renderer, rDia.name, 14, 340);
//	game::Bitmap->render(program.renderer, rDia.text[rDia.off], 14, 365, 71);

//	// render the black screen if fading
//	if ( Flags.mask(0xC1) )
//	  game::Graphics->blit();

return 0;
}

static int _main_camera(GameMode* const gm, const PROGRAM& program) {
//	using game::Flags;

//	// update the level
//	level::update(gm, 1);

//	// move the camera
//	auto* pEvent = (EventTrigger*) gm->data;

//	if (pEvent->camera.increments >= std::abs(pEvent->camera.speed) ) {
//	  camera::scroll(pEvent->camera.speed);
//	  pEvent->camera.increments -= std::abs(pEvent->camera.speed);
//	}
//	else {
//	  // -1 is camera speed is negative
//	  const float fMult = pEvent->camera.speed < 0 ? -1 : 1;
//	  
//	  // 
//	  camera::scroll( fMult * pEvent->camera.increments );
//	  GM_ClearData(gm);
//	  Triggers_ClearEvent(*pEvent);
//	  levelEvents = GMLevel_Normal;
//	}

return 0;
}

////////////////////////// gm_level MAIN routine /////////////////////////
int _gm_level_main(GameMode* const gm, const PROGRAM& program) {
	// list of pointers to subfunctions
	typedef int (*InputMode_Function)(GameMode* const, const PROGRAM&);

	constexpr InputMode_Function functions[5] = {
	  _main_normal, _main_camera, _main_normal, _main_dialogue, _main_normal
	};

return functions[levelEvents](gm, program);
}

//////////////////////// gm_level CLEANUP routine ///////////////////////
int _gm_level_cleanup(GameMode* const gm, const PROGRAM& program) {
	using level::unload;
	using game::Flags;
	
	// unload the level
	level::unload();
	UnloadBackgroundObj();
	
	// reset the background color
	JColor black = {0, 0, 0, 255};
	game::bgcolor(black);
	
	// clean up the level data
{
	int iFlags = 0;
	
	if ( Flags.mask(LEVEL_CLEANUP) ) {
	  Sound_FreeMUS(LevelMusic);
	  LevelMusic = nullptr;
	  iFlags |= LEVEL_CLEANUP;
	}
	iFlags |= QUITGAME;
	Flags.unset(iFlags);
}
		
return 0;
}

/////////////////////// gm_level INIT routine //////////////////////////
int _gm_level_init(GameMode* const gm, const PROGRAM& program) {
	// retrieve the scancode of each key defined here
	if (! _scancodes[SCAN_END]) {
	  _scancodes[SCAN_S]      = SDL_GetScancodeFromKey(SDLK_s);
	  _scancodes[SCAN_LEFT]   = SDL_GetScancodeFromKey(SDLK_LEFT);
	  _scancodes[SCAN_RIGHT]  = SDL_GetScancodeFromKey(SDLK_RIGHT);
	  _scancodes[SCAN_UP]     = SDL_GetScancodeFromKey(SDLK_UP);
	  _scancodes[SCAN_DOWN]   = SDL_GetScancodeFromKey(SDLK_DOWN);
	  _scancodes[SCAN_L]      = SDL_GetScancodeFromKey(SDLK_l);
	  _scancodes[SCAN_ESCAPE] = SDL_GetScancodeFromKey(SDLK_ESCAPE);
	  _scancodes[SCAN_Q]      = SDL_GetScancodeFromKey(SDLK_q);
	  _scancodes[SCAN_W]      = SDL_GetScancodeFromKey(SDLK_w);
	  
	  // just to make sure this statement isn't read twice
	  _scancodes[SCAN_END]    = SDL_GetScancodeFromKey(SDLK_UP);
	}

	// save data to private struct
	_player_data->KeySymBits = reinterpret_cast<uint16_t*>(HeapStack + (int) HS_KeyBits);
	_player_data->KeySymBitsFirstFrame = reinterpret_cast<uint16_t*>(HeapStack);

	// reset all keys
	* const_cast<uint16_t*>(_player_data->KeySymBits) = 0;
	* const_cast<uint16_t*>(_player_data->KeySymBitsFirstFrame) = 0;

	// after a certain amount of time, play the level's music
	SDL_TimerID timer = SDL_AddTimer(2000, timerCallback_playLevelMusic, nullptr);
	
	if (! timer) {
	  Log_Cerr("Failed to initialize timer in gm_level: %s\n", SDL_GetError());
	  Sound_HaltMusic();
	  Sound_FreeMUS(LevelMusic);
	  LevelMusic = nullptr;
	}

	game::Flags.set(FADEIN);
	level::ThePlayer->Init();
	BG_BLACK.blit();
	camera::correctCamera();

return 2;
}

// public gamemode function //
int gm_level(void* const gamemode_void, const PROGRAM& program) {
	typedef int (* FPI_OO)(GameMode* const, const PROGRAM&);

	static const FPI_OO functions[] = {
	  _gm_level_main,
	  _gm_level_init,
	  _gm_level_cleanup
	};

	GameMode* gm = (GameMode*) gamemode_void;

	// jump to function using index
	const uint16_t index = (gm->tm < 3) ? gm->tm : 0;

return functions[index](gm, program);
}

static void eventBits(uint16_t& A, uint16_t& B, const int bit) {
	if (! ((A | B) & bit)) {
	  A |= bit;
	}
	else {
	  if (! (B & bit)) {
	  	A &= bit ^ 0xffff;
	  	B |= bit;
	  }
	}
}

// event functions //
static int _keyboard_normal(SDL_Event& event, GameMode* const gm, const uint8_t* keystates) {
	using game::Flags;
	using game::MiscTimer;
	
	uint16_t* uipKeyBits = reinterpret_cast<uint16_t*>(HeapStack + (int) HS_KeyBits);
	uint16_t* uipKeyFirst = reinterpret_cast<uint16_t*>(HeapStack);
	
	for (uint8_t x = 0; x < GMLevel_Key_NumOfValues; ++x) {
	  uint16_t uiFirst = *uipKeyFirst;
	  uint16_t uiBits  = *uipKeyBits;
	  
	  // list of keys being parsed
	  switch (_key_sym[x]) {
	  	default: break;
	  	
	  	case GMLevel_Key_s:
	  	  if (keystates[_scancodes[SCAN_S]]) {
	  	  	eventBits(uiFirst, uiBits, 1);
	  	  }
	  	  else {
	  	  	uiFirst &= 0xFFFE;
	  	  	uiBits &= 0xFFFE;
	  	  }
	  	  break;
	  	
	  	case GMLevel_Key_Left:
	  	  if (keystates[_scancodes[SCAN_LEFT]]) {
	  	  	eventBits(uiFirst, uiBits, 2);
	  	  }
	  	  else {
	  	  	uiFirst &= 0xFFFD;
	  	  	uiBits &= 0xFFFD;
	  	  }
	  	  break;
	  	
	  	case GMLevel_Key_Right:
	  	  if (keystates[_scancodes[SCAN_RIGHT]]) {
	  	  	eventBits(uiFirst, uiBits, 4);
	  	  }
	  	  else {
	  	  	uiFirst &= 0xFFFB;
	  	  	uiBits &= 0xFFFB;
	  	  }
	  	  break;
	  	
	  	case GMLevel_Key_Up:
	  	  if (keystates[_scancodes[SCAN_UP]]) {
	  	  	eventBits(uiFirst, uiBits, 8);
	  	  }
	  	  else {
	  	  	uiFirst &= 0xFFF7;
	  	  	uiBits &= 0xFFF7;
	  	  }
	  	  break;
	  	
	  	case GMLevel_Key_Down:
	  	  if ( keystates[_scancodes[SCAN_DOWN]] ) {
	  	  	eventBits(uiFirst, uiBits, 16);
	  	  }
	  	  else {
	  	  	uiFirst &= 0xFFEF;
	  	  	uiBits &= 0xFFEF;
	  	  }
	  	  break;
	  	
	  	case GMLevel_Key_l:
	  	  if (keystates[_scancodes[SCAN_L]]) {
	  	  	eventBits(uiFirst, uiBits, 32);
	  	  }
	  	  else {
	  	  	uiFirst &= 0xFFDF;
	  	  	uiBits &= 0xFFDF;
	  	  }
	  	  break;
	  	
	  	case GMLevel_Key_Escape:
	  	  if (keystates[_scancodes[SCAN_ESCAPE]]) {
	  	  	if ( Flags.mask(FADEOUT | QUITGAME) ) {
	  	  	  Sound_FadeOutMusic(1000);
	  	  	  GM_ChangeGamemode(gm, 0, 60);
	  	  	  ThePlayer->m_obj.xspeed = 0;
	  	  	  ThePlayer->m_obj.frame = 0;
	  	  	  Flags.set(FADEOUT | QUITGAME);
	  	  	}
	  	  }
	  	  break;
	  }

	  *uipKeyFirst = uiFirst;
	  *uipKeyBits = uiBits;

	  // behavior depending on key
	  /*switch (_key_sym[x]) {
	  	Press the L key once. Prints out debug information about where the player is
	  	in the level. Coordinates are in screen units and tiles. Only responds on the
	  	first frame of it being pressed.
	  	case GMLevel_Key_l:
	  	  // the L key was detected
	  	  if (keystates[_scancodes[SCAN_L]]) {
	  	  	// the L key wasn't already pressed
	  	  	if (! (_key_sym_bits & _key_sym_bitmasks[GMLevel_Key_l])) {
	  	  	  // print X and Y position of tile the player is in
	  	  	  printf("Player's location: (%d,%d) pixels; (%d,%d) tiles\nTile number: %d\n", ThePlayer->m_obj.get_x(),
	  	  	         ThePlayer->m_obj.get_y(), ThePlayer->m_obj.get_x() / TILE_WIDTH,
	  	  	         ThePlayer->m_obj.get_y() / TILE_HEIGHT, (int) GetTile( ThePlayer->m_obj.get_x(), ThePlayer->m_obj.get_y() ));
	  	  	  
	  	  	  // activate bit saying the L key was pressed
	  	  	  _key_sym_bits |= _key_sym_bitmasks[GMLevel_Key_l];
	  	  	}
	  	  }
	  	  
	  	  // reset bit that says the L key was pressed
	  	  else {_key_sym_bits &= _key_sym_bitmasks[GMLevel_Key_l] ^ 0xff;}
	  	  break;

	  	default: break;
	  } // end switch()*/
	}

return 0;
}

static int _keyboard_dialogue(SDL_Event& event, GameMode* const gm, const uint8_t* keystates) {
	using game::MiscTimer;

//	// block input if the timer BlkTm is set
//	if (MiscTimer) {
//	  // keep player from moving
//	  player->set_x_vel(0);

//	  // decrease timer
//	  --MiscTimer;
//	  return 0;
//	}

//	// event structure
//	EventTrigger* const pTrigger = (EventTrigger*) gm->data;

//	// check if either the s or d keys have been pressed
//	const uint8_t inmask = (keystates[scancode_q] << 4) | keystates[scancode_w];

//	if (inmask & 0xf0) {
//	  // the left key was pressed:

//	  // if the array offset is zero, play a sound effect and set BlkTm to 1 second
//	  if (! pTrigger->dialogue.off) {
//	  	Sound_PlaySFX(SFXNo, 1);
//	  	MiscTimer = miliseconds_to_frames(1200);
//	  }
//	  else {
//	  	--(pTrigger->dialogue.off); // not zero, decrement it
//	  	Sound_PlaySFX(SFXYes, 1);
//	  	MiscTimer = miliseconds_to_frames(910);
//	  }
//	}
//	else if (inmask & 0x0f) {
//	  // the right key was pressed:

//	  // increment AOff; if the string is NULL, change IOMode to normal
//	  ++(pTrigger->dialogue.off);
//	  if (pTrigger->dialogue.off == pTrigger->dialogue.size) {
//	  	Sound_PlaySFX(SFXDiaClose, 1);

//	  	// go back to the original IOMode for this gamemode
//	  	levelEvents = GMLevel_Normal;

//	  	// remove the event from both the gamemode data and the event queue
//	  	GM_ClearData(gm);
//	  	Triggers_ClearEvent(*pTrigger);

//	  	// clear the timer
//	  	MiscTimer = 0;
//	  }
//	  else {
//	  	Sound_PlaySFX(SFXYes, 1);
//	  	MiscTimer = miliseconds_to_frames(910);
//	  }
//	}

return 0;
}

int gm_level_keyboard(SDL_Event& event, GameMode* const gm, uint8_t entry_point) {
	using game::Flags;

	/*
	Return from the function if: the function is not instructed to collect keyboard states;
	if the player is stunned from an enemy's attack; or if the game is either fading in in or out, or the
	input-disabling flag is set.
	*/
	if (entry_point != EVENT_USE_KEYSTATES || ThePlayer->m_StunTimer > 0 || Flags.mask(FADING | DISABLE_INPUT))
	  return 0;

	// jump to different routines to capture input
	typedef int (*InputMode_Func)(SDL_Event&,GameMode* const,const uint8_t*);

	constexpr InputMode_Func _functions[5] = {
	  _keyboard_normal, _keyboard_normal, _keyboard_normal, _keyboard_dialogue, _keyboard_normal
	};

return _functions[levelEvents](event, gm, SDL_GetKeyboardState(nullptr));
}
