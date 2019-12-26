#ifndef SOUND_WAV_H_INCLUDED
#define SOUND_WAV_H_INCLUDED

#include <iostream>
#include "sdl_incs.h"

// chunk that defines the wave file's format, like a header almost
struct WaveFMT {
	//uint32_t chunkID;
	//uint32_t chunkLen;
	uint16_t encoding;
	uint16_t channels; // 1 = mono, 2 = stereo
	uint32_t frequency; // one of 11025, 22050, or 44100 Hz
	uint32_t byterate; // average bytes per second
	uint16_t blockalign; // bytes per sample block
	uint16_t bitspersample; // one of 8, 12, 16, or 4 for ADPCM
};

struct SampleLoop {
	uint32_t id;
	uint32_t type;
	uint32_t start;
	uint32_t end;
	uint32_t fraction;
	uint32_t play_count;
};

struct SamplerChunk {
	uint32_t manufacturer;
	uint32_t product;
	uint32_t sampleperiod;
	uint32_t MIDIunitynote;
	uint32_t MIDIpitchfraction;
	uint32_t SMTPEformat;
	uint32_t SMTPEoffset;
	uint32_t sampleloops;
	uint32_t samplerdata;
	SampleLoop loops[];
};

struct WAVLoopPoint {
	bool active;
	uint32_t start;
	uint32_t stop;
	uint32_t initial_play_count;
	uint32_t current_play_count;
};

struct WAVStream {
	SDL_RWops*    src;
	bool          freesrc;
	SDL_AudioSpec spec;
	int64_t       start;
	int64_t       stop;
	SDL_AudioCVT  cvt;
	int           numloops;
	WAVLoopPoint* loops;
	uint8_t       volume;
};

#endif /* SOUND_WAV_H_INCLUDED */
