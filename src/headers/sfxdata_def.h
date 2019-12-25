#ifndef SFXDATA_DEF_H_INCLUDED
#define SFXDATA_DEF_H_INCLUDED

#include <iostream>
#include "bad_option"
#include "stdinc.h"
#include "sdl_incs.h"
#include "sound.hpp"

/* SFXData: holds audio data loaded from a WAV file. */
struct SFXData {
	bool allocated; // internal
	bool free; // internal
	uint8_t* abuf; // internal
	uint32_t alen; // internal
	SDL_AudioSpec specs; // internal
	uint8_t volume;
	int8_t channel;
	int32_t loop; // internal
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

INLINE void SFXData_SetVolume(SFXData* sfx, uint8_t vol) {
	sfx->volume = vol;
}

INLINE void SFXData_SetChannel(SFXData* sfx, int8_t ch) {
	sfx->channel = ch;
}

INLINE void SFXData_Clear(SFXData* sfx, uint8_t vol) {
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
