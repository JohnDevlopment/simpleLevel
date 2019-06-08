#ifndef SOUND_HPP
#define SOUND_HPP

#include <iostream>
#include <initializer_list>
#include <levelLoad.hpp>
#include <bad_option>
#include <climits>

#include "global_attributes.hpp"

// relevent types
#include "sfxdata_def.h"
#include "musdata_def.h"

#if defined(__GNUG__)
 #define Sound_BadParameterAndMsg(f, p, msg) std::cerr << "Invalid parameter to " << #f << " '" \
                                             << #p << "' : " << msg << '\n'
 #define Sound_BadParameter(f, p)            std::cerr << "Invalid parameter to " << #f << " '" \
                                             << #p << '\n'
#endif

#define Sound_SetError(f,msg)	std::cerr << "Error from " << #f << ": " << msg << '\n'
#define MAX_VOLUME		128

// initialize the sound system
bool Sound_Init(int freq, uint16_t format, int ch, int chunksize) COLD;

// close the sound system
void Sound_Quit() COLD;

// these functions manipulate MusData

// TODO add description
MusData* Sound_LoadMUSType_RW(SDL_RWops* rw, MUS_Type type, int freesrc);
MusData* Sound_LoadMUSType(const char* src, MUS_Type type);

// TODO add description
#define Sound_PlayMusic(mus, loops) Sound_FadeInMusicPos(mus, loops, 0, 0.0)
int Sound_FadeInMusicPos(MusData* mus, int loops, int ms, double position);

// set music volume
int Sound_VolumeMusic(int vol);

// set the position of the music
int Sound_SetMusicPosition(double position);

// halt the music track
void Sound_HaltMusic();

// free music chunk
void Sound_FreeMUS(MusData* mus);

/////////////////// these functions manipulate SFXData /////////////////////////////

/*
Load WAV files from either a file or SDL_RWops. Both return either a pointer to the sound effect
or NULL on failure. As for Sound_LoadWAV_RW, if freesrc is non-zero, then Sound_LoadWAV_RW will
automatically free rw.
*/
SFXData* Sound_LoadWAV(const char* file) UNUSED;
SFXData* Sound_LoadWAV_RW(SDL_RWops* rw, int freesrc) UNUSED;

// free sfx
void Sound_FreeSFX(SFXData* sfx);

/*
Plays a sound chunk on a particular channel. If 'ch' is -1, play on the first
available channel.
'loops' is how many times to play the sound again after the first time; 0 means
"play only once".
'ms' is how long to allow it to play; -1 lets the channel play the entire sound.
*/
#define Sound_PlayChannel(ch, sfx, loops) Sound_PlayChannelTimed(ch, sfx, loops, -1)
int Sound_PlayChannelTimed(int ch, SFXData* sfx, int loops, int ms);
int Sound_PlaySFX(int track, int loops);

// returns greater than 0 if the channel is playing; pass -1 to check all channels
int Sound_Playing(int ch);

// halts the channel; pass -1 to halt all of them
void Sound_HaltChannel(int ch);

// set volume of a channel
int Sound_VolumeChannel(int ch, int vol);

// get SFXData from global list
SFXData* Sound_GetSFX(int track);

#endif /* SOUND_HPP */
