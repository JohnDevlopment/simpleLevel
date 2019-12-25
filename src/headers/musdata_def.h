#ifndef MUSDATA_DEF_H_INCLUDED
#define MUSDATA_DEF_H_INCLUDED

#include <iostream>
#include "bad_option"
#include "stdinc.h"
#include "sdl_incs.h"
#include "sound/sound_wav.h"
#include "sound/sound_ogg.h"

enum MUS_Type {
	MUS_TYPE_NONE,
	MUS_TYPE_WAV,
	MUS_TYPE_OGG,
	MUS_TYPE_CMD
};

enum MUS_FadingStatus {
	MUS_FADE_NONE,
	MUS_FADE_IN,
	MUS_FADE_OUT
};

/* MusData: holds music audio data */
struct MusData;
struct MusData {
	MUS_Type type; // internal
	int error; // internal
	int loops; // internal
	int playing; // internal
	int fade_status; // internal
	int fade_step; // internal
	int fade_steps; // internal
	union {
	  OGGMusic* ogg;
	  WAVStream* wav;
	  void* cmd;
	}; // internal
	double lpos;
	uint8_t volume; // internal
	MusData* head; // internal
	MusData* next; // internal
};

enum {
	LOOP_NO,
	LOOP_YES
};

inline void MusData_Clear(MusData* mus, int vol) {
	mus->type = MUS_TYPE_NONE;
	mus->error = 0;
	mus->loops = 0;
	mus->playing = 0;
	mus->fade_status = MUS_FADE_NONE;
	mus->fade_step = 0;
	mus->fade_steps = 0;
	mus->lpos = 0.0;
	mus->volume = (uint8_t) vol;
	mus->head = nullptr;
	mus->next = nullptr;
}

inline MusData* MusData_LinkMUS(MusData* mus, MusData* link) {
	if (mus && link) {
	  if (mus->head == nullptr && link->head == nullptr) {
	  	MusData* last = mus;
	  	MusData* current = mus->next;
	  	
	  	while (current) {
	  	  last = current;
	  	  current = current->next;
	  	}
	  	
	  	last->next = link;
	  	link->head = mus;
	  }
	}
	
return mus;
}

INLINE void MusData_SetVolume(MusData* mus, int vol) {
	mus->volume = (uint8_t) vol;
}

#define MusCount 6

#endif /* MUSDATA_DEF_H_INCLUDED */
