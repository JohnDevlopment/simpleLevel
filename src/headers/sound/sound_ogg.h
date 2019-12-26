#ifndef SOUND_OGG_H_INCLUDED
#define SOUND_OGG_H_INCLUDED

#include OGG_HEADER
#include "stdinc.h"
#include "sdl_incs.h"

struct OGGMusic {
	SDL_RWops* src;
	bool freesrc;
	int playing;
	int volume;
	OggVorbis_File vf;
	int section;
	SDL_AudioCVT cvt;
	int lenavailable;
	uint8_t* sndavailable;
};

struct vorbis_loader {
	bool loaded;
	void* handle;
	int (*ov_clear)(OggVorbis_File* vf);
	vorbis_info* (*ov_info)(OggVorbis_File* vf, int link);
	int (*ov_open_callbacks)(void* datasrc, OggVorbis_File* vf, const char* initial, long bytes, ov_callbacks callbacks);
	ogg_int64_t (*ov_pcm_total)(OggVorbis_File* vf, int i);
	long (*ov_read)(OggVorbis_File* vf, char* buffer, int length, int bigendianp, int word, int sgnd, int* bitstream);
	int (*ov_time_seek)(OggVorbis_File* vf, double pos);
};

/*
Notes: this "loader" is, I think, meant to be used for loading the Ogg Vorbis library and getting a pointer to several functions within
that library.

loaded: whether or not the library is open

handle: if library is open, this points to it in memory

Function pointers:
	* ov_clear: UNKNOWN
	* ov_info: presumably returns a pointer to a struct with information on the file that's open
	* ov_open_callbacks: UNKNOWN
	* ov_pcm_total: UNKNOWN
	* ov_read: reads a certain number of bytes from the ogg file
	* ov_time_seek: I guess it seeks the OGG file
*/

#endif /* SOUND_OGG_H_INCLUDED */
