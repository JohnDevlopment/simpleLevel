#ifndef GAMEMODE_HPP
#define GAMEMODE_HPP

// standard includes
#include "stdinc.h"

// definition of GameMode
#include "gamemode_def.hpp"

// bad_option exception class
#include <custom/bad_option>

// Image class
#include <custom/image_class.hpp>

// program structure
#include "program.hpp"

// function attributes
#include "global_attributes.hpp"

// event triggers
#include "triggers.hpp"

#define NUMBER_OF_GAMEMODES		5

extern SDL_TimerID gTimer;

// function pointer declaration
typedef int (* GameMode_Function)(void* const, const PROGRAM&);

// get the gamemode pointer
GameMode* GM_GetPointer();

// main gamemode routing function; called every frame
void GM_Main(GameMode* const gm, const PROGRAM& program, int& ret);

// frees the memory used by the gamemode pointer returned by GM_NewObj
void GM_Free();

// returns a pointer to a dynamically allocated GameMode structure
GameMode* GM_NewObj() UNUSED;

// incrementally adds to or subtracts from the rectangle's alpha value depending on the boolean value
uint8_t fade_to_or_from_black(uint8_t rate, bool increment);

// ADD alpha
inline uint8_t fade_to_black(uint8_t rate) {
	return fade_to_or_from_black(rate, true);
}

// SUBTRACT alpha
inline uint8_t fade_from_black(uint8_t rate) {
	return fade_to_or_from_black(rate, false);
}

// set gamemode data pointer
template<typename T>
inline void GM_SetData(GameMode* gm, T* ptr) {
	gm->data = ptr;
	gm->type = MEMBER_DATA_UNDEFINED;
}

template<>
inline void GM_SetData<SDL_Renderer>(GameMode* gm, SDL_Renderer* ptr) {
	gm->data = ptr;
	gm->type = MEMBER_DATA_RENDERER;
}

template<>
inline void GM_SetData<int>(GameMode* gm, int* ptr) {
	gm->data = ptr;
	gm->type = MEMBER_DATA_INT;
}

template<>
inline void GM_SetData<short int>(GameMode* gm, short int* ptr) {
	gm->data = ptr;
	gm->type = MEMBER_DATA_SHORT;
}

template<>
inline void GM_SetData<long int>(GameMode* gm, long int* ptr) {
	gm->data = ptr;
	gm->type = MEMBER_DATA_LONG;
}

template<>
inline void GM_SetData<char>(GameMode* gm, char* ptr) {
	gm->data = ptr;
	gm->type = MEMBER_DATA_CHAR;
}

template<>
inline void GM_SetData<unsigned int>(GameMode* gm, unsigned int* ptr) {
	gm->data = ptr;
	gm->type = MEMBER_DATA_UINT;
}

template<>
inline void GM_SetData<unsigned short int>(GameMode* gm, unsigned short int* ptr) {
	gm->data = ptr;
	gm->type = MEMBER_DATA_USHORT;
}

template<>
inline void GM_SetData<unsigned long int>(GameMode* gm, unsigned long int* ptr) {
	gm->data = ptr;
	gm->type = MEMBER_DATA_ULONG;
}

template<>
inline void GM_SetData<unsigned char>(GameMode* gm, unsigned char* ptr) {
	gm->data = ptr;
	gm->type = MEMBER_DATA_UCHAR;
}

template<>
inline void GM_SetData<Image>(GameMode* gm, Image* ptr) {
	gm->data = ptr;
	gm->type = MEMBER_DATA_IMAGE_CLASS;
}

template<>
inline void GM_SetData<EventTrigger>(GameMode* gm, EventTrigger* ptr) {
	gm->data = ptr;
	gm->type = MEMBER_DATA_EVENT_TRIGGER;
}

// clear the gamemode data pointer
inline void GM_ClearData(GameMode* gm) {
	gm->data = nullptr;
	gm->type = MEMBER_DATA_NULL;
}

// sets the next gamemode with an optional timer
inline void GM_ChangeGamemode(GameMode* gm, uint8_t num, uint8_t tm) {
	// valid gamemode
	if (num < NUMBER_OF_GAMEMODES) {
	  gm->gm_next = num;
	  gm->tm      = tm < 255 ? tm + 1 : 255;
	}
}

#endif /* GAMEMODE_HPP */
