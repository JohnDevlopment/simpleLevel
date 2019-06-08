#ifndef MUSDATA_DEF_H_INCLUDED
#define MUSDATA_DEF_H_INCLUDED

#include <iostream>
#include <bad_option>
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

struct MusData;
struct MusData {
	MUS_Type type;
	int error;
	int loops;
	int playing;
	int fade_status;
	int fade_step;
	int fade_steps;
	union {
	  void* ogg;
	  WAVStream* wav;
	  void* cmd;
	};
	double lpos;
	uint8_t volume;
	MusData* head;
	MusData* next;
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

inline void MusData_SetVolume(MusData* mus, int vol) {
	if (mus) {
	  mus->volume = (uint8_t) vol;
	}
}

#define MusCount 6

#endif /* MUSDATA_DEF_H_INCLUDED */
