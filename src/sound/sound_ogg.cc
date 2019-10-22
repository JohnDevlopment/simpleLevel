#include "sound.hpp"
#include "res.hpp"
#include "stdinc.h"
#include "endian.hpp"
#include "memory.hpp"

#define cast_to_func(type, args...) (type (*)(args))

int MusicFmt_InitOgg();

extern vorbis_loader Vorbis;
extern std::unique_ptr<SDL_AudioSpec> OggMixer;

static std::unique_ptr<char[]> Buffer(new char[4096]);

// stop the OGG stream
void OGGMusic_Stop(OGGMusic* mus) {
	mus->playing = 0;
}

// start playback
void OGGMusic_Play(OGGMusic* mus) {
	mus->playing = 1;
}

// is it playing?
bool OGGMusic_Playing(OGGMusic* mus) {
	return (bool) mus->playing;
}

// set internal volume
void OGGMusic_SetVolume(OGGMusic* mus, int vol) {
	mus->volume = vol;
}

// seeks to a different place in the file
void OGGMusic_SetPosition(OGGMusic* mus, double position) {
	Vorbis.ov_time_seek(&mus->vf, position);
}

// frees the OGG music
void OGGMusic_Free(OGGMusic* mus) {
	if (mus) {
	  delete[] mus->cvt.buf;
	  if (mus->freesrc) {
	  	SDL_RWclose(mus->src);
	  }
	  Vorbis.ov_clear(&mus->vf);
	  delete mus;
	}
}

///////////////// helper functions to OGGMusic_PlayAudioData ////////////////////
static void oggGetSome(OGGMusic* mus) {
	int iLen, iSection;
	SDL_AudioCVT& cvt = mus->cvt;
	
	// read data into a buffer
	iLen = Vorbis.ov_read(&mus->vf, Buffer.get(), 4096, 0, 2, 1, &iSection);
	if (iLen <= 0) {
	  if (! iLen) OGGMusic_Stop(mus);
	  return;
	}
	
	// build conversion information
	if (iSection != mus->section) {
	  vorbis_info* vi = Vorbis.ov_info(&mus->vf, -1);
	  SDL_BuildAudioCVT(&cvt, AUDIO_S16, vi->channels, vi->rate, OggMixer->format, OggMixer->channels, OggMixer->freq);
	  
	  // build a new audio buffer that's to the size of the buffer being read
	  delete[] cvt.buf;
	  cvt.buf = new uint8_t[4096 * cvt.len_mult];
	  mus->section = iSection;
	}
	
	// copy to the conversion buffer and convert it if neccessary
	SDL_memcpy(cvt.buf, Buffer.get(), iLen);
	if (cvt.needed) {
	  cvt.len = iLen;
	  SDL_ConvertAudio(&cvt);
	}
	else {
	  cvt.len_cvt = iLen;
	}
	
	mus->lenavailable = cvt.len_cvt;
	mus->sndavailable = cvt.buf;
}

int OGGMusic_PlayAudioData(OGGMusic* mus, uint8_t* data, int len) {
	int iMixable;
	
	while (len > 0 && mus->playing) {
	  if (! mus->lenavailable) {
	  	oggGetSome(mus);
	  }
	  iMixable = len;
	  if (iMixable > mus->lenavailable)
	  	iMixable = mus->lenavailable;
	  
	  // copy the audio data
	  if (mus->volume == MAX_VOLUME) {
	  	SDL_memcpy(data, mus->sndavailable, iMixable);
	  }
	  else {
	  	SDL_MixAudioFormat(data, mus->sndavailable, OggMixer->format, iMixable, mus->volume);
	  }
	  
	  mus->lenavailable -= iMixable;
	  mus->sndavailable += iMixable;
	  len -= iMixable;
	  data += iMixable;
	}
	
return len;
}

//////////////////// helper functions to OGGMusic_LoadSong_RW ////////////////////
static size_t readFunc(void* ptr, size_t size, size_t nmembers, void* datasource) {
	return SDL_RWread((SDL_RWops*) datasource, ptr, size, nmembers);
}

static int seekFunc(void* datasource, ogg_int64_t offset, int whence) {
	return (int) SDL_RWseek((SDL_RWops*) datasource, offset, whence);
}

static long tellFunc(void* datasource) {
	return (long) SDL_RWtell((SDL_RWops*) datasource);
}
//int (*ov_open_callbacks)(void* datasrc, OggVorbis_File* vf, const char* initial, long bytes, ov_callbacks callbacks);
OGGMusic* OGGMusic_LoadSong_RW(SDL_RWops* src, int freesrc) {
	OGGMusic* mus;
	ov_callbacks callbacks;
	
	// if Ogg isn't initialized, return
	if ( MusicFmt_InitOgg() < 0 ) return nullptr;
	
	// initialize callbacks
	SDL_memset(&callbacks, 0, sizeof(ov_callbacks));
	callbacks.read_func = readFunc;
	callbacks.seek_func = seekFunc;
	callbacks.tell_func = tellFunc;
	
	// set return value
	mus = new OGGMusic;
	SDL_zerop(mus);
	mus->src = src;
	mus->freesrc = (bool) freesrc;
	OGGMusic_Stop(mus);
	OGGMusic_SetVolume(mus, MAX_VOLUME);
	mus->section = -1;
	
	// open the OGG file
	if ( Vorbis.ov_open_callbacks(src, &mus->vf, nullptr, 0, callbacks) < 0 ) {
	  Sound_SetError(OGGMusic_LoadSong_RW, "not an OGG file");
	  delete mus;
	  return nullptr;
	}
	
return mus;
}

/////////////////////// list of functions that support MusicFmt_Init //////////////////////////
int MusicFmt_InitOgg() {
	if (! Vorbis.loaded) {
	  // open shared library libVorbisFile
	  Vorbis.handle = DLOpenLib(OGG_FILE_LIB);
	  if (Vorbis.handle == nullptr) return -1;
	  
	  Vorbis.ov_clear = cast_to_func(int, OggVorbis_File*) DLFunction(Vorbis.handle, "ov_clear");
	  if (Vorbis.ov_clear == nullptr) {
	  	DLClose(Vorbis.handle);
	  	return -1;
	  }
	  
	  Vorbis.ov_info = cast_to_func(vorbis_info*, OggVorbis_File*, int) DLFunction(Vorbis.handle, "ov_info");
	  if (Vorbis.ov_info == nullptr) {
	  	DLClose(Vorbis.handle);
	  	return -1;
	  }
	  
	  Vorbis.ov_open_callbacks = \
	  	cast_to_func(int, void*, OggVorbis_File*, const char*, long, ov_callbacks) DLFunction(Vorbis.handle, "ov_open_callbacks");
	  if ( Vorbis.ov_open_callbacks == nullptr ) {
	  	DLClose(Vorbis.handle);
	  	return -1;
	  }
	  
	  // ov_pcm_total
	  Vorbis.ov_pcm_total =
	  	cast_to_func(ogg_int64_t, OggVorbis_File*, int) DLFunction(Vorbis.handle, "ov_pcm_total");
	  if ( Vorbis.ov_pcm_total == nullptr ) {
	  	DLClose(Vorbis.handle);
	  	return -1;
	  }
	  
	  // ov_read
	  Vorbis.ov_read =
	  	cast_to_func(long, OggVorbis_File*, char*, int, int, int, int, int*)
	  	  DLFunction(Vorbis.handle, "ov_read");
	  if ( Vorbis.ov_read == nullptr ) {
	  	DLClose(Vorbis.handle);
	  	return -1;
	  }
	  
	  // ov_time_seek
	  Vorbis.ov_time_seek =
	  	cast_to_func(int, OggVorbis_File* vf, double pos)
	  	  DLFunction(Vorbis.handle, "ov_time_seek");
	  if ( Vorbis.ov_time_seek == nullptr ) {
	  	DLClose(Vorbis.handle);
	  	return -1;
	  }
	  
	  Vorbis.loaded = true;
	}
	
return 0;
}

void MusicFmt_QuitOgg() {
	if (Vorbis.loaded) {
	  DLClose(Vorbis.handle);
	  SDL_memset( &Vorbis, 0, sizeof(vorbis_loader) );
	}
}
