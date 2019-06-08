#ifndef SFXDATA_DEF_H_INCLUDED
#define SFXDATA_DEF_H_INCLUDED

#include <iostream>
#include <bad_option>
#include "stdinc.h"
#include "sdl_incs.h"
#include "sound.hpp"

struct SFXData {
	bool allocated;
	bool free;
	uint8_t* abuf;
	uint32_t alen;
	SDL_AudioSpec specs;
	uint8_t volume;
	int8_t channel;
	int32_t loop;
};

enum {
	SFXChgJump = 0,
	SFXYes,
	SFXNo,
	SFXDiaClose,
	SFXStartGame,
	
	SFXCount
};

#define NUMBER_OF_GLOBAL_SFX SFXCount

inline void SFXData_Clear(SFXData* sfx, int vol) {
	if (sfx) {
	  sfx->allocated = false;
	  sfx->free = false;
	  sfx->abuf = nullptr;
	  sfx->alen = 0;
	  //sfx->specs
	  sfx->volume = vol;
	  sfx->channel = -1;
	  sfx->loop = 0;
	}
}

#endif /* SFXDATA_DEF_H_INCLUDED */
