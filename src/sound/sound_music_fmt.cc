#include <memory>
#include "sound.hpp"
#include "res.hpp"
#include "stdinc.h"

// general functions to make visible
int MusicFmt_Init(SDL_AudioSpec& mixer);
void MusicFmt_Quit();

#define cast_to_func(type, args...) (type (*)(args))

vorbis_loader vorbis;

// mixers for each codec
std::unique_ptr<SDL_AudioSpec> WaveMixer(new SDL_AudioSpec);
std::unique_ptr<SDL_AudioSpec> OggMixer(new SDL_AudioSpec);

/////////////////////// list of functions that support MusicFmt_Init //////////////////////////
int MusicFmt_InitOgg() {
	if (! vorbis.loaded) {
	  // open shared library libVorbisFile
	  vorbis.handle = DLOpenLib(OGG_FILE_LIB);
	  if (vorbis.handle == nullptr) return -1;
	  
	  vorbis.ov_clear = cast_to_func(int, OggVorbis_File*) DLFunction(vorbis.handle, "ov_clear");
	  if (vorbis.ov_clear == nullptr) {
	  	DLClose(vorbis.handle);
	  	return -1;
	  }
	  
	  vorbis.ov_info = cast_to_func(vorbis_info*, OggVorbis_File*, int) DLFunction(vorbis.handle, "ov_info");
	  if (vorbis.ov_info == nullptr) {
	  	DLClose(vorbis.handle);
	  	return -1;
	  }
	  
	  vorbis.ov_open_callbacks = \
	  	cast_to_func(int, void*, OggVorbis_File*, const char*, long, ov_callbacks) DLFunction(vorbis.handle, "ov_open_callbacks");
	  if ( vorbis.ov_open_callbacks == nullptr ) {
	  	DLClose(vorbis.handle);
	  	return -1;
	  }
	  
	  // ov_pcm_total
	  vorbis.ov_pcm_total =
	  	cast_to_func(ogg_int64_t, OggVorbis_File*, int) DLFunction(vorbis.handle, "ov_pcm_total");
	  if ( vorbis.ov_pcm_total == nullptr ) {
	  	DLClose(vorbis.handle);
	  	return -1;
	  }
	  
	  // ov_read
	  vorbis.ov_read =
	  	cast_to_func(long, OggVorbis_File*, char*, int, int, int, int, int*)
	  	  DLFunction(vorbis.handle, "ov_read");
	  if ( vorbis.ov_read == nullptr ) {
	  	DLClose(vorbis.handle);
	  	return -1;
	  }
	  
	  // ov_time_seek
	  vorbis.ov_time_seek =
	  	cast_to_func(int, OggVorbis_File* vf, double pos)
	  	  DLFunction(vorbis.handle, "ov_time_seek");
	  if ( vorbis.ov_time_seek == nullptr ) {
	  	DLClose(vorbis.handle);
	  	return -1;
	  }
	  
	  vorbis.loaded = true;
	}
	
return 0;
}

void MusicFmt_QuitOgg() {
	if (vorbis.loaded) {
	  DLClose(vorbis.handle);
	  SDL_memset( &vorbis, 0, sizeof(vorbis_loader) );
	}
}

/* Returns 0 if successfull, -1 if Ogg fails to load */
int MusicFmt_Init(SDL_AudioSpec& mixer) {
	*WaveMixer = mixer;
	*OggMixer = mixer;
	
return MusicFmt_InitOgg();
}

/* Does not return; deinits Ogg and other formats */
void MusicFmt_Quit() {
	MusicFmt_QuitOgg();
}
