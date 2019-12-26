#ifndef SOUND_CHANNEL_DEF_H_INCLUDED
#define SOUND_CHANNEL_DEF_H_INCLUDED

#include <iostream>
#include "musdata_def.h"
#include "sfxdata_def.h"

struct _Sound_Channel {
	int playing;
	int looping;
	bool paused;
	uint8_t* samples;
	SFXData* chunk;
	int expire;
	int start;
	int volume;
};

#endif /* SOUND_CHANNEL_DEF_H_INCLUDED */
