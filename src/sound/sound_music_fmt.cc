#include <memory>
#include "sound.hpp"
#include "res.hpp"
#include "stdinc.h"

// general functions to make visible
int MusicFmt_Init(SDL_AudioSpec& mixer);
void MusicFmt_Quit();
int MusicFmt_InitOgg();
void MusicFmt_QuitOgg();

vorbis_loader Vorbis;

// mixers for each codec
std::unique_ptr<SDL_AudioSpec> WaveMixer(new SDL_AudioSpec);
std::unique_ptr<SDL_AudioSpec> OggMixer(new SDL_AudioSpec);

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
