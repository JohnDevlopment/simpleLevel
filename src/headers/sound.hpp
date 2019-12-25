#ifndef SOUND_HPP
#define SOUND_HPP

#include <iostream>
#include <initializer_list>
#include <climits>
#include "levelLoad.hpp"
#include "bad_option"
#include "global_attributes.hpp"

// relevent types
#include "sfxdata_def.h"
#include "musdata_def.h"

#if defined(__GNUG__)
 #define Sound_BadParameterAndMsg(f, p, msg) std::cerr << "Invalid parameter to " << #f << " '" \
                                             << #p << "' : " << msg << '\n'
 #define Sound_BadParameter(f, p)            std::cerr << "Invalid parameter to " << #f << " '" \
                                             << #p << "'\n"
#endif

#define Sound_SetError(f,msg)	std::cerr << "Error from " << #f << ": " << msg << '\n'
#define MAX_VOLUME		128

/* Initialize the sound system with the given frequency/sample rate (samples per second [in Hz]),
   with a format defining the type of data being used by the device, and the number of samples
   per sound chunk in the audio callback. Returns true or false depending on the success. */
bool Sound_Init(int freq, uint16_t format, int ch, int chunksize) COLD;

// close the sound system
void Sound_Quit() COLD;

// returns true if the sound system is active
bool Sound_Active();

//////////////////////// these functions manipulate MusData /////////////////////////

/*
Sound_LoadMUSType_RW:
Loads a RWops to memory as either a WAV or an OGG audio stream. If 'freesrc'
is not equal to 0, 'rw' will be closed upon invocation of Sound_FreeMUS. It returns
a pointer to MusData or NULL on failure. And if the pointer returned is NULL, then
Sound_FreeMUS is called. 'type' is used to tell the function whether the data being
read is WAV or OGG. Check musdata_def.h for possible values.

Sound_LoadMUSType:
Loads a RWops to memory from a given file. It returns a valid pointer to MusData or NULL
on failure. 'type' works the same way as with Sound_LoadMUSType_RW. Sound_LoadMUSType
creates a RWops to the given file and then forwards it to Sound_LoadMUSType_RW and
passes 'freesrc' as 1. Check the above paragraph for more info.
*/
MusData* Sound_LoadMUS(const char* src);
MusData* Sound_LoadMUSType_RW(SDL_RWops* rw, MUS_Type type, int freesrc);
MusData* Sound_LoadMUSType(const char* src, MUS_Type type);

/*
Sound_PlayMusic:
A macro to Sound_FadeInMusicPos; it accepts a pointer to MusData and the number
of times to play the song -- 1 to play it once, 2 for twice, etc. The last couple options
for Sound_FadeInMusicPos are zeroed out. Check the below paragraph to know what this does.

Sound_FadeInMusicPos:
Plays the given MusData pointer on the music channel and returns 0 if it succeeds or -1
on failure. 'loops' is how many times to play the song -- 0 or below for not at all, 1 for once,
2 for twice, and so on. 'ms' is how many milliseconds to fade in the track over or 0 to not
initiate a fade-in. 'position' is where to start playback or 0 to play from the beginning. Like
Sound_PlayMusic it returns 0 on success and -1 on failure.

Sound_FadeInMusic:
A macro to Sound_FadeInMusicPos; it accepts a pointer to MusData, loops, and ms as arguments.
This has the same effect as Sound_FadeInMusicPos(mus, loops, ms, 0.0).
*/
int Sound_FadeInMusicPos(MusData* mus, int loops, int ms, double position);
#define Sound_PlayMusic(mus, loops) Sound_FadeInMusicPos(mus, loops, 0, 0.0)
#define Sound_FadeInMusic(mus, loops, ms) Sound_FadeInMusicPos(mus, loops, ms, 0.0)

// returns true if music is playing, false otherwise
bool Sound_PlayingMusic();

// sets the volume for the current music track
int Sound_VolumeMusic(int vol);

// seeks to a given position in the current music track
int Sound_SetMusicPosition(double position);

// pauses music playback
void Sound_PauseMusic();

// resumes music playback
void Sound_ResumeMusic();

// returns true if music is paused, false otherwise
bool Sound_PausedMusic();

// returns the fade status of current playing music
MUS_FadingStatus Sound_FadingMusic();

/* Fades the music out to a halt over the span of however many milliseconds
   are specified. If 'ms' is <= 0, then it immediately halts playback via
   Sound_HaltMusic.
 */
int Sound_FadeOutMusic(int ms);

// halts music playback
void Sound_HaltMusic();

// frees a MusData pointer
void Sound_FreeMUS(MusData* mus);

/////////////////// these functions manipulate SFXData /////////////////////////////

/*
Sound_LoadWAV:
Loads a WAV file into memory and returns a pointer to SFXData. If it returns NULL, then there
was an error. Sound_LoadWAV actually forwards to Sound_LoadWAV_RW.

Sound_LoadWAV_RW:
Loads a WAV file into memory and returns a pointer to SFXData. If it returns NULL, then there was
an error. The arguments are a pointer to SDL_RWops, which can either point to a file on the
harddisk or a segment of memory that's in WAV format. If the other argument is non-zero, then the
RWops is automatically closed when the SFXData is freed using Sound_FreeSFX; otherwise it must be
closed manually.
*/
SFXData* Sound_LoadWAV(const char* file) UNUSED;
SFXData* Sound_LoadWAV_RW(SDL_RWops* rw, int freesrc) UNUSED;

/*
Sound_PlayChannelTimed:
Plays a sound chunk on a particular channel for 'loops' amount of times. If 'ch' is
-1 then play on the first available channel. 'ms' is how many milliseconds to allow
the channel to play before stopping it; this will interrupt the sound playing if it's
longer than 'ms'. If it's -1, then allow the channel to play from beginning to end.

Sound_PlayChannel:
Plays a sound chunk on a particular channel for 'loops' amount of times. If 'ch' is
-1 then it plays on the first available channel. Sound_PlayChannel is just a macro to
Sound_PlayChannelTimed that specifies 'ms' as -1.
*/
#define Sound_PlayChannel(ch, sfx, loops) Sound_PlayChannelTimed(ch, sfx, loops, -1)
int Sound_PlayChannelTimed(int ch, SFXData* sfx, int loops, int ms);
int Sound_PlaySFX(int track, int loops);

// returns greater than 0 if the channel is playing; pass -1 to check all channels
int Sound_Playing(int ch);

// set volume of a channel
int Sound_VolumeChannel(int ch, int vol);

// halts the channel; pass -1 to halt all of them
void Sound_HaltChannel(int ch);

// free sfx
void Sound_FreeSFX(SFXData* sfx);

// get SFXData from global list
SFXData* Sound_GetSFX(int track);

#endif /* SOUND_HPP */
