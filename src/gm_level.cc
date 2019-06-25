// global game data and functions
#include "headers/game.hpp"

// data and functions that pertain to a level
#include "headers/levelcode.hpp"

// functions that return various information about the level
#include "headers/levelinfo.hpp"

// defines a list of event types
#include "headers/event_defines.hpp"

// sound system
#include "headers/sound.hpp"

// tile collision system
#include "headers/tile_collision.hpp"

// camera system
#include "headers/camera.hpp"

// sprite system
#include "headers/sprite_router.hpp"

// event trigger system
#include "headers/triggers.hpp"

// Tcl system
#include "headers/tcl.hpp"

// logging system
#include "headers/log.hpp"

// math functions
#include "headers/math.hpp"

// gamemode headers
#include "headers/gm_level.hpp"

// private types, variables, etc.
#include "private/player_data_def.h"

using namespace std;
using level::ThePlayer;
using level::levelEvents;
using level::update;

// private function prototypes
static int _gm_level_cleanup(GameMode* const, const PROGRAM&);
static int _gm_level_main(GameMode* const, const PROGRAM&);
static int _gm_level_init(GameMode* const, const PROGRAM&);
static uint32_t timerCallback_playLevelMusic(uint32_t, void*);

// priority list of events to process
static StaticDArray<uint8_t,7> _key_sym = {GMLevel_Key_s, GMLevel_Key_Left, GMLevel_Key_Right,
 GMLevel_Key_Up, GMLevel_Key_l, GMLevel_Key_Escape, GMLevel_Key_Buffer};

// input bit flags: each bit corresponds to whether a certain key is being held down
static uint8_t _key_sym_bits = 0;

// list of scancodes
static StaticDArray<SDL_Scancode,9> _scancodes = {
	SDL_SCANCODE_UNKNOWN, SDL_SCANCODE_UNKNOWN, SDL_SCANCODE_UNKNOWN,
	SDL_SCANCODE_UNKNOWN, SDL_SCANCODE_UNKNOWN, SDL_SCANCODE_UNKNOWN,
	SDL_SCANCODE_UNKNOWN, SDL_SCANCODE_UNKNOWN, SDL_SCANCODE_UNKNOWN
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
	// play level music on loop
	Sound_ResumeMusic();

return 0;
}

/* Notes to self: firstly, _key_sym_bits is a field of bits that each correspond to a key being pressed. Mask the bits
   using _key_sym_bitmasks indexed by a value of GMLevel_KeyListSym (header: gm_level_defs.h). That is how you
   can tell what inputs have been inputted. */
static int _main_normal(GameMode* const gm, const PROGRAM&) {
	using namespace level;
	using game::FrameCounter;
	using game::Flags;

	_player_data->what = 1;

	// update the level
	update(gm, _player_data.getp(), 0);

	// render the black screen if the right flags are set
	if (Flags.mask(FADING | QUITGAME))
	  BG_BLACK.blit();

	// if the user is quitting the game...
	if (Flags.mask(QUITGAME)) {
	  // activate the level's cleanup routine
	  if (! Flags.mask(LEVEL_CLEANUP)) {
	  	// enable the level cleanup code
	  	Flags.set(LEVEL_CLEANUP);
	  	
	  	// change to gamemode 0 after 75 frames (2.5 seconds)
	  	GM_ChangeGamemode(gm, 0, 75);
	  }
	}

	_player_data->what = 2;
	ThePlayer->Main( _player_data.getp() );

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
	if ( Flags.mask(LEVEL_CLEANUP) ) {
	  Sound_FreeMUS(LevelMusic);
	  LevelMusic = nullptr;
	  Flags.unset(LEVEL_CLEANUP);
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
	  _scancodes[SCAN_L]      = SDL_GetScancodeFromKey(SDLK_l);
	  _scancodes[SCAN_ESCAPE] = SDL_GetScancodeFromKey(SDLK_ESCAPE);
	  _scancodes[SCAN_Q]      = SDL_GetScancodeFromKey(SDLK_q);
	  _scancodes[SCAN_W]      = SDL_GetScancodeFromKey(SDLK_w);
	  
	  // just to make sure this statement isn't read twice
	  _scancodes[SCAN_END]    = SDL_GetScancodeFromKey(SDLK_UP);
	}

	// reset all keys
	_key_sym_bits = 0;

	// save data to private struct
	_player_data->KeySymBits = &_key_sym_bits;

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

// event functions //
static int _keyboard_normal(SDL_Event& event, GameMode* const gm, const uint8_t* keystates) {
	using game::Flags;
	using game::MiscTimer;

	// left and right movement speeds (format: right_normal, right_fast, left_normal, left_fast)
	constexpr float _left_right_speeds[4] = {2.5f, 3, -2.5f, -3};

	// parse events
	bool faster = false;

	for (uint8_t x = 0; x < GMLevel_Key_NumOfValues; ++x) {
	  uint8_t index = (uint8_t) faster;

	  // behavior depending on key
	  switch (_key_sym[x]) {
	  	// when the S key is held, after a delay speed up the player
	  	case GMLevel_Key_s:
	  	  if (keystates[_scancodes[SCAN_S]]) {
	  	  	// increment the timer until it reaches 10
	  	  	if (MiscTimer < 10)
	  	  	  ++MiscTimer;
	  	  	
	  	  	// when it reaches 10, let the player move faster
	  	  	else
	  	  	  faster = true;
	  	  }
	  	  else {
	  	  	// decrease timer until it reaches 0
	  	  	if (MiscTimer)
	  	  	  --MiscTimer;
	  	  }
	  	  break;

	  	/* Pressing on the Left Arrow key. Make the player move left. Don't flip
	  	the sprite image; it faces left by default. */
	  	case GMLevel_Key_Left:
	  	  // if the left key is actually being pressed
	  	  if (keystates[_scancodes[SCAN_LEFT]]) {
	  	  	// if the right key isn't being pressed
	  	  	if (! (_key_sym_bits & _key_sym_bitmasks[GMLevel_Key_Right])) {
	  	  	  // assign a negative X speed to the player
	  	  	  ThePlayer->m_obj.xspeed = _left_right_speeds[index+2]; // player move left
	  	  	  
	  	  	  // make the player face left
	  	  	  ThePlayer->m_obj.flip(SDL_FLIP_NONE);
	  	  	  
	  	  	  // this bit signifies the left key was pressed
	  	  	  _key_sym_bits |= _key_sym_bitmasks[GMLevel_Key_Left];
	  	  	}
	  	  }
	  	  else if ( _key_sym_bits & _key_sym_bitmasks[GMLevel_Key_Left] ) {
	  	  	// clear bit
	  	  	_key_sym_bits &= _key_sym_bitmasks[GMLevel_Key_Left] ^ 0xff;
	  	  	
	  	  	// freeze the player
	  	  	ThePlayer->m_obj.xspeed = 0;
	  	  }
	  	  break;

	  	/* Pressing on the Right Arrow Key. Make the player move and face right. Flip the image
	  	horizontally. */
	  	case GMLevel_Key_Right:
	  	  // if the right key is being pressed
	  	  if (keystates[_scancodes[SCAN_RIGHT]]) {
	  	  	// if the left key isn't being pressed
	  	  	if (! (_key_sym_bits & _key_sym_bitmasks[GMLevel_Key_Left])) {
	  	  	  // give the player a positive X speed and make it face right
	  	  	  ThePlayer->m_obj.xspeed = _left_right_speeds[index];
	  	  	  ThePlayer->m_obj.flip(SDL_FLIP_HORIZONTAL);
	  	  	  
	  	  	  // set the bit saying the right key was pressed
	  	  	  _key_sym_bits |= _key_sym_bitmasks[GMLevel_Key_Right];
	  	  	}
	  	  }
	  	  else if ( _key_sym_bits & _key_sym_bitmasks[GMLevel_Key_Right] ) {
	  	  	// clear the bit
	  	  	_key_sym_bits &= _key_sym_bitmasks[GMLevel_Key_Right] ^ 0xff;
	  	  	
	  	  	// freeze the player
	  	  	ThePlayer->m_obj.xspeed = 0;
	  	  }
	  	  break;

	  	/* Pressing down on the Up Arrow key. Makes the player jump. Only responds on the
	  	first frame of it being pressed. */
	  	case GMLevel_Key_Up:
	  	  if (keystates[_scancodes[SCAN_UP]]) {
	  	  	// only respond to the first frame the key is pressed
	  	  	if (! (_key_sym_bits & _key_sym_bitmasks[GMLevel_Key_Up])) {
	  	  	  // causes the player to jump
	  	  	  ThePlayer->Jump();
	  	  	  
	  	  	  // set the bit that says the up key was pressed
	  	  	  _key_sym_bits |= _key_sym_bitmasks[GMLevel_Key_Up];
	  	  	}
	  	  }
	  	  
	  	  // clear the bit
	  	  else {_key_sym_bits &= _key_sym_bitmasks[GMLevel_Key_Up] ^ 0xff;}
	  	  break;

	  	/* Press the L key once. Prints out debug information about where the player is
	  	in the level. Coordinates are in screen units and tiles. Only responds on the
	  	first frame of it being pressed. */
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
	  	
	  	/* Pressing down on the Escape key. Quits the game. Only responds once to the input and never
	  	again. */
	  	case GMLevel_Key_Escape:
	  	  // if the Escape key was detected
	  	  if (keystates[_scancodes[SCAN_ESCAPE]]) {
	  	  	// if the Escape key wasn't already pressed
	  	  	if (! (_key_sym_bits & _key_sym_bitmasks[GMLevel_Key_Escape])) {
	  	  	  // quit the game and fade out
	  	  	  Flags.set(FADEOUT | QUITGAME);
	  	  	  Sound_FadeOutMusic(1000);

	  	  	  // change to gamemode 0 after 60 frames
	  	  	  GM_ChangeGamemode(gm, 0, 60);

	  	  	  // zero out the player's velocity so it smooths to a halt
	  	  	  ThePlayer->m_obj.xspeed = 0;

	  	  	  // set this bit so this block only executes once
	  	  	  _key_sym_bits |= _key_sym_bitmasks[GMLevel_Key_Escape];
	  	  	}
	  	  }
	  	  break;

	  	default: break;
	  } // end switch()
	} // end for()

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
