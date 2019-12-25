#ifndef GAMEMODE_HPP
#define GAMEMODE_HPP

// engine headers
#include "stdinc.h"
#include "gamemode_def.hpp"
#include "program.hpp"
#include "global_attributes.hpp"
#include "triggers.hpp"
#include "sound.hpp"
#include "textures.h"

// other headers
#include "bad_option"
#include "generic_class.hpp"

#define NUMBER_OF_GAMEMODES 5

extern SDL_TimerID gTimer;

// function pointer declaration
typedef int (* GameMode_Function)(void* const, const PROGRAM&);



#endif /* GAMEMODE_HPP */
