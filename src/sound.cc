// global game data
#include "game.hpp"

// dynamic runtime linking; general sound API
#include "res.hpp"
#include "sound.hpp"

// info about a level; level-related functions and data
#include "levelinfo.hpp"
#include "levelcode.hpp"

// Tcl scripts
#include "tcl.hpp"

// multi-threaded functionality; thread locks and mutexes
#include "threads.hpp"
#include "locks.hpp"

// logging output
#include "log.hpp"

// compile-time strings
#include "str_const.hpp"

// memory functions
#include "memory.hpp"

#include "sound/sound_thread.h"
#include "sound/sound_channel_def.h"

// std::list; std::vector; std::new and std::nothrow
#include <list>
#include <vector>
#include <new>

#include "debug.hpp"

/* Magic numbers for various audio file formats */
#define RIFF        0x46464952      /* "RIFF" */
#define WAVE        0x45564157      /* "WAVE" */
#define FORM        0x4d524f46      /* "FORM" */
#define OGGS        0x5367674f      /* "OggS" */
#define CREA        0x61657243      /* "Crea" */
#define FLAC        0x43614C66      /* "fLaC" */

using std::cerr;
using game::Flags;

// WAVStream functions to make visible
void WAVStream_FreeSong(WAVStream* wav);
WAVStream* WAVStream_LoadSong_RW(SDL_RWops* src, int freesrc);
void WAVStream_Start(WAVStream* wav);
void WAVStream_Stop();
bool WAVStream_Playing();
int WAVStream_PlaySome(uint8_t* stream, int len);
int WAVStream_SetPosition(double position);

// functions from sound_music_fmt.cc
int MusicFmt_Init(SDL_AudioSpec&);
void MusicFmt_Quit();

// private function prototypes
static void closeGlobalSFX();
static bool loadGlobalSFX();
//static void freeAllMusic();
static int checkSFXInternal(SFXData* sfx);
static void audioCallback(void*, uint8_t*, int);
static void closeAudioDevice();
static int openAudioDevice(SDL_AudioSpec& desiredspecs);
static int internalPlayMusic(MusData* mus, double position);
static int internalSetPosition(double position);
static void internalHaltMusic();
static void internalVolumeMusic(int vol);
static bool internalPlayingMusic();
static int internalHaltOrLoopMusic();

// private variables
constexpr int _num_channels = 4;

static SDL_AudioSpec* CurrentAudioSpecs = nullptr;
static bool AudioOpened = false;
static SDL_AudioDeviceID AudioID;
//static uint8_t MusicVolume = MAX_VOLUME;
//static bool MusicStopped = false;
static bool MusicActive = false;
static int MsPerStep;
static _Sound_Channel* Channel = nullptr;
static std::unique_ptr<SFXData*[]> Sfxs(new SFXData*[NUMBER_OF_GLOBAL_SFX]);
static void* Handle = nullptr;

MusData* CurrentMusic = nullptr;

#define _Sound_ClearChannel(ch) { \
	Channel[ch].playing = 0; \
	Channel[ch].looping = 0; \
	Channel[ch].paused = false; \
	Channel[ch].samples = nullptr; \
	Channel[ch].chunk = nullptr; \
	Channel[ch].expire = 0; \
	Channel[ch].start = 0; \
}

// public functions
bool Sound_Init(int freq, uint16_t format, int ch, int chunksize) {
	SDL_AudioSpec desiredspecs;
	
	Log_Cout("### Attempting to initialize SOUND system\n");
	
	// allocate pointer
	if (CurrentAudioSpecs == nullptr)
	  CurrentAudioSpecs = new SDL_AudioSpec;
	
	// if device already opened, return
	if ( Flags.mask(SOUND_API_INIT) ) return true;
	
	// open audio device
	SDL_zero(desiredspecs);
	SDL_zerop(CurrentAudioSpecs);
	
	desiredspecs.freq = freq;
	desiredspecs.format = format;
	desiredspecs.channels = ch;
	desiredspecs.samples = chunksize;
	desiredspecs.callback = audioCallback;
	desiredspecs.userdata = nullptr;
	
	if ( openAudioDevice(desiredspecs) < 0 ) return false;
	
	// init music players
	if ( MusicFmt_Init(*CurrentAudioSpecs) < 0 ) return false;
	Log_Cout("Initialized Ogg decoder\n");
	
//	MusicStopped = false;
//	CurrentMusic = nullptr;
	Sound_VolumeMusic(MAX_VOLUME);
	
	// allocate sound byte channels
	Channel = new _Sound_Channel[_num_channels];
	for (int x = 0; x < _num_channels; ++x) {
	  Channel[x].playing = 0;
	  Channel[x].looping = 0;
	  Channel[x].paused  = false;
	  Channel[x].samples = nullptr;
	  Channel[x].chunk   = nullptr;
	  Channel[x].expire  = 0;
	  Channel[x].start   = 0;
	  Channel[x].volume  = MAX_VOLUME;
	}
	Log_Cout("Allocated %d channels for mixing\n", _num_channels);
	
	// set milliseconds per callback (step)
	MsPerStep = static_cast<int>( (float) CurrentAudioSpecs->samples * 1000.0f / CurrentAudioSpecs->freq );
	
	#ifndef NDEBUG
	Log_Cout("Milliseconds per callback: %d\n", MsPerStep);
	#endif
	
	// open library file
	Handle = DLOpen("audio/sound_effects/sfx.so");
	if (Handle == nullptr) {
	  Sound_SetError(Sound_Init, "failed to open sfx.so");
	  return false;
	}
	Log_Cout("Opened audio/sound_effects/sfx.so for reading\n");
	
	// load sound effects from library
	if ( loadGlobalSFX() < 0 ) return false;
	Log_Cout("Loaded %d sound effects to the global array\n", NUMBER_OF_GLOBAL_SFX);
	
	// set global flag
	Flags.set(SOUND_API_INIT);
	SDL_PauseAudioDevice(AudioID, 0);
	Log_Cout("Initialized SOUND system and set the appropriate flag\n");
	
return true;
}

void Sound_Quit() {
	if ( Flags.mask(SOUND_API_INIT) ) {
	  closeGlobalSFX();
	  DLClose(Handle);
	  Handle = nullptr;
	  
	  // disable the callback
	  SDL_LockAudioDevice(AudioID);
	  CurrentAudioSpecs->callback = nullptr;
	  SDL_UnlockAudioDevice(AudioID);
	  
	  delete[] Channel;
	  Channel = nullptr;
	  MusicFmt_Quit();
	  closeAudioDevice(); // FIXME callback still runs even after this, causing a SEGFAULT
	  Flags.unset(SOUND_API_INIT);
	}
}

int Sound_VolumeMusic(int vol) {
	int iPrevVol = -1;
	
	if (CurrentMusic) {
	  iPrevVol = CurrentMusic->volume;
	  
	  if (vol >= 0 && vol <= MAX_VOLUME)
	  	CurrentMusic->volume = MAX_VOLUME;
	  
	  internalVolumeMusic(vol);
	}
	
return iPrevVol;
}

int Sound_VolumeChannel(int ch, int vol) {
	int iPrevVol = 0;
	
	if (ch < 0) {
	  for (ch = 0; ch < _num_channels; ++ch) {
	  	iPrevVol += Sound_VolumeChannel(ch, vol);
	  }
	  iPrevVol /= _num_channels;
	}
	else {
	  iPrevVol = Channel[ch].volume;
	  if (vol >= 0) {
	  	if (vol > MAX_VOLUME) vol = MAX_VOLUME;
	  	Channel[ch].volume = vol;
	  }
	}
	
return iPrevVol;
}

void Sound_HaltChannel(int ch) {
	if (ch == -1) {
	  for (int i = 0; i < _num_channels; ++i)
	  	Sound_HaltChannel(i);
	}
	else {
	  SDL_LockAudioDevice(AudioID);
	  _Sound_ClearChannel(ch);
	  SDL_UnlockAudioDevice(AudioID);
	}
}

int Sound_PlayChannelTimed(int ch, SFXData* sfx, int loops, int ms) {
	// don't play null pointers
	if (sfx == nullptr) {
	  ERROR(Sound_PlayChannelTimed, sfx);
	  return -1;
	}
	
	if ( ! checkSFXInternal(sfx) ) {
	  Sound_SetError(Sound_PlayChannelTimed, "SFX had a bad frame");
	  return -1;
	}
	
	SDL_LockAudioDevice(AudioID);
	{
	  // if 'ch' is -1, find the first available channel
	  if (ch == -1) {
	  	int i = 0;
	  	for (; i < _num_channels; ++i) {
	  	  if (Channel[i].playing <= 0) break;
	  	}
	  	
	  	// channels all unavailable
	  	if (i == _num_channels) {
	  	  Sound_SetError(Sound_PlayChannelTimed, "no free channel available");
	  	  ch = -1;
	  	}
	  	else {
	  	  ch = i;
	  	}
	  }
	  
	  // activate the channel
	  if ( ch >= 0 && ch < _num_channels ) {
	  	uint32_t _ticks = SDL_GetTicks();
	  	Channel[ch].samples = sfx->abuf;
	  	Channel[ch].playing = sfx->alen;
	  	Channel[ch].chunk   = sfx;
	  	Channel[ch].paused  = false;
	  	Channel[ch].start   = _ticks;
	  	Channel[ch].expire  = (ms > 0) ? (_ticks + ms) : 0;
	  }
	} // end block
	SDL_UnlockAudioDevice(AudioID);
	
return ch;
}

int Sound_Playing(int ch) {
	int status = 0;
	
	if (ch == -1) {
	  for (ch = 0; ch < _num_channels; ++ch) {
	  	if (Channel[ch].playing > 0) ++status;
	  }
	}
	else {
	  if (Channel[ch].playing > 0) ++status;
	}
	
return status;
}

inline SFXData* Sound_GetSFX(int track) {
	return (track < NUMBER_OF_GLOBAL_SFX) ? Sfxs[track] : nullptr;
}

int Sound_PlaySFX(int track, int loops) {
	SFXData* sfx = Sound_GetSFX(track);
	int code = -1;
	
	if (sfx) {
	  code = Sound_PlayChannelTimed(sfx->channel, sfx, loops, 0);
	}
	else {
	  Sound_SetError(Sound_PlaySFX, "parameter 'track' is out of range");
	  PRINT_EXPR(cerr, track);
	  PRINT_LINE(cerr);
	}
	
return code;
}



SFXData* Sound_LoadWAV(const char* file) {
	SFXData* sfx = nullptr;
	SDL_RWops* rw = nullptr;
	
	if (file == nullptr) {
	  ERROR(Sound_LoadWAV, file);
	  return nullptr;
	}
	
	// open file
	rw = SDL_RWFromFile(file, "rb");
	
	if (rw == nullptr) {
	  Sound_SetError(Sound_LoadWAV, SDL_GetError());
	  PRINT_LINE(cerr);
	  return nullptr;
	}
	
	sfx = Sound_LoadWAV_RW(rw, 1);
	
return sfx;
}

SFXData* Sound_LoadWAV_RW(SDL_RWops* rw, int freesrc) {
	SFXData* sfx;
	uint32_t uiMagic;
	SDL_AudioSpec* loaded;
	
	// invalid parameters or not opened device
	if (! AudioOpened) {
	  Sound_SetError(Sound_LoadWAV_RW, "audio device is not open");
	  PRINT_LINE(cerr);
	  if (freesrc) {
	  	SDL_RWclose(rw);
	  }
	  return nullptr;
	}
	
	if (rw == nullptr) {
	  ERROR(Sound_LoadWAV_RW, rw);
	  return nullptr;
	}
	
	// suss out the format type
	sfx = new SFXData;
	
	uiMagic = SDL_ReadLE32(rw);
	SDL_RWseek(rw, -(int)sizeof(uint32_t), RW_SEEK_CUR);
	
	switch (uiMagic) {
	  case WAVE:
	  case RIFF:
	  	loaded = SDL_LoadWAV_RW( rw, 1, &sfx->specs, (uint8_t**) &sfx->abuf, &sfx->alen );
	  	break;
	  
	  case OGGS:
	  	SDL_SetError("OGG not supported yet");
	  	break;
	  
	  default:
	  	Sound_SetError(Sound_LoadWAV_RW, "unsupported format");
	  	loaded = nullptr;
	  	break;
	}
	
	// if it's failed to load
	if (loaded == nullptr) {
	  Sound_SetError(Sound_LoadWAV_RW, SDL_GetError());
	  delete sfx;
	  return nullptr;
	}
	
	// build audio conversion information
	SDL_AudioSpec& specshad = sfx->specs;
	if ( specshad.format != CurrentAudioSpecs->format
		|| specshad.channels != CurrentAudioSpecs->channels
		|| specshad.freq != CurrentAudioSpecs->freq ) {

	  // build conversion structure
	  SDL_AudioCVT cvt;
	  int iSampleSize;
	  if ( SDL_BuildAudioCVT( &cvt, specshad.format, specshad.channels, specshad.freq,
	  	CurrentAudioSpecs->format, CurrentAudioSpecs->channels, CurrentAudioSpecs->freq ) < 0 ) {

	  	// set error and free memory
	  	Sound_SetError(Sound_LoadWAV_RW, SDL_GetError());
	  	SDL_free(sfx->abuf);
	  	delete sfx;
	  	return nullptr;
	  }
	  
	  iSampleSize = ((specshad.format & 0xff) / 8) * specshad.channels;
	  cvt.len = sfx->alen & ~(iSampleSize - 1);
	  cvt.buf = (uint8_t*) SDL_calloc(1, cvt.len * cvt.len_mult);
	  if (cvt.buf == nullptr) {
	  	Sound_SetError(Sound_LoadWAV_RW, "ran out of memory");
	  	SDL_free(sfx->abuf);
	  	delete sfx;
	  	return nullptr;
	  }
	  
	  SDL_memcpy(cvt.buf, sfx->abuf, sfx->alen);
	  SDL_free(sfx->abuf);
	  
	  // convert the audio buffer
	  if ( SDL_ConvertAudio(&cvt) < 0 ) {
	  	Sound_SetError(Sound_LoadWAV_RW, SDL_GetError());
	  	SDL_free(cvt.buf);
	  	delete sfx;
	  	return nullptr;
	  }
	  
	  // place the converted audio into the SFXData
	  sfx->abuf = cvt.buf;
	  sfx->alen = cvt.len_cvt;
	}
	
	sfx->allocated = true;
	sfx->volume = MAX_VOLUME;
	
return sfx;
}

void Sound_FreeSFX(SFXData* sfx) {
	if (sfx) {
	  // make sure the audio callback isn't working
	  SDL_LockAudioDevice(AudioID);
	  if (Channel) {
	  	// clear the channel if it's playing the SFX
	  	for (int x = 0; x < _num_channels; ++x) {
	  	  if (sfx == Channel[x].chunk) {
	  	  	_Sound_ClearChannel(x);
	  	  }
	  	}
	  }
	  SDL_UnlockAudioDevice(AudioID);
	  
	  if (sfx->allocated) SDL_FreeWAV(sfx->abuf);
	  delete sfx;
	}
}

void Sound_FreeMUS(MusData* mus) {
	if (! mus) return;
	
	SDL_LockAudioDevice(AudioID);
	if (mus == CurrentMusic) {
	  // wait for the fade-out to finish
	  while (mus->fade_status == MUS_FADE_OUT) {
	  	SDL_UnlockAudioDevice(AudioID);
	  	SDL_Delay(100);
	  	SDL_LockAudioDevice(AudioID);
	  }

	  // if it's still playing
	  if (mus == CurrentMusic) {
	  	internalHaltMusic();
	  }
	}
	SDL_UnlockAudioDevice(AudioID);
	
	// free data according to type
	switch (mus->type) {
	  case MUS_TYPE_WAV:
	  	WAVStream_FreeSong(mus->wav);
	  	break;
	  
	  case MUS_TYPE_OGG:
	  	Sound_SetError(Sound_FreeMUS, "Ogg not currently supported");
	  	break;
	  
	  default:
	  	Sound_SetError(Sound_FreeMUS, "invalid format");
	  	break;
	}
	
	delete mus;
}

void Sound_HaltMusic() {
	SDL_LockAudioDevice(AudioID);
	if (CurrentMusic) {
	  internalHaltMusic();
	}
	SDL_UnlockAudioDevice(AudioID);
}

int Sound_FadeInMusicPos(MusData* mus, int loops, int ms, double position) {
	// invalid parameters or audio device not set
	if (! MsPerStep) {
	  Sound_SetError(Sound_FadeInMusicPos, "audio device has not been activated");
	  return -1;
	}
	
	if (mus == nullptr) {
	  Sound_BadParameter(Sound_FadeInMusicPos, mus);
	  return -1;
	}
	
	// fade status
	if (ms) {
	  mus->fade_status = MUS_FADE_IN;
	}
	else {
	  mus->fade_status = MUS_FADE_NONE;
	}
	mus->fade_step = 0;
	mus->fade_steps = ms / MsPerStep;
	
	// if fading out, wait for the fade to finish
	SDL_LockAudioDevice(AudioID);
	while (CurrentMusic && CurrentMusic->fade_status == MUS_FADE_OUT) {
	  SDL_UnlockAudioDevice(AudioID);
	  SDL_Delay(100);
	  SDL_LockAudioDevice(AudioID);
	}
	MusicActive = true;
	
	// loops
	if (loops == 1) loops = 0;
	mus->loops = loops;
	
	int retval = internalPlayMusic(mus, position);
	SDL_UnlockAudioDevice(AudioID);
	
return retval;
}

MusData* Sound_LoadMUSType(const char* src, MUS_Type type) {
	SDL_RWops* rw = SDL_RWFromFile(src, "rb");
	
	if (rw == nullptr) {
	  Sound_SetError(Sound_LoadMUSType, SDL_GetError());
	  return nullptr;
	}
	
return Sound_LoadMUSType_RW(rw, type, 1);
}

MusData* Sound_LoadMUSType_RW(SDL_RWops* rw, MUS_Type type, int freesrc) {
	MusData* mus;
	
	if (rw == nullptr) {
	  Sound_BadParameter(Sound_LoadMUSType_RW, rw);
	  return nullptr;
	}
	
	// for now, reject the type argument being MUS_TYPE_NONE; TODO change this
	if (type == MUS_TYPE_NONE) {
	  Sound_BadParameterAndMsg(Sound_LoadMUSType_RW, type, "MUS_TYPE_NONE is not accepted right now");
	  return nullptr;
	}
	
	mus = new MusData;
	MusData_Clear(mus, MAX_VOLUME);
	
	// load according to type
	switch (type) {
	  case MUS_TYPE_WAV:
	  	mus->type = MUS_TYPE_WAV;
	  	mus->wav = WAVStream_LoadSong_RW(rw, freesrc);
	  	if (mus->wav == nullptr) mus->error++;
	  	break;
	  
	  case MUS_TYPE_OGG:
	  	mus->type = MUS_TYPE_OGG;
	  	mus->error++;
	  	break;
	  
	  default:
	  	Sound_SetError(Sound_LoadMUSType_RW, "unrecognized format type");
	  	mus->error++;
	  	break;
	}
	
	// return and free memory if there is an error
	if (mus->error) {
	  delete mus;
	  mus = nullptr;
	  if (! freesrc) {
	  	SDL_RWseek(rw, 0, RW_SEEK_SET);
	  }
	}
	
return mus;
}

int Sound_SetMusicPosition(double position) {
	int retval = 0;
	
	SDL_LockAudioDevice(AudioID);
	if (CurrentMusic) {
	  retval = internalSetPosition(position);
	  if (retval < 0) {
	  	Log_Cerr("Error from Sound_SetMusicPosition: audio format does not support seeking\n");
	  }
	}
	else {
	  Log_Cerr("Error from Sound_SetMusicPosition: no music is playing\n");
	}
	SDL_UnlockAudioDevice(AudioID);
	
return retval;
}

/////////////////////////////////////////////////
constexpr str_const _sfx_syms[NUMBER_OF_GLOBAL_SFX] = {
	"_binary_chgjump_wav_start",
	"_binary_yes_wav_start",
	"_binary_no_wav_start",
	"_binary_diaclose_wav_start",
	"_binary_startgame_wav_start"
};

constexpr int _sfx_chs[NUMBER_OF_GLOBAL_SFX] = {0, 1, 1, 1, 0};

bool loadGlobalSFX() {
	SFXData* sfx;
	uint32_t uiSize;
	void* vpSym;
	
	for (int x = 0; x < NUMBER_OF_GLOBAL_SFX; ++x) {
	  uiSize = 0;
	  vpSym = DLPtr(Handle, _sfx_syms[x].get(), &uiSize);
	  if (vpSym == nullptr || ! uiSize) {
	  	return -1;
	  }
	  
	  sfx = Sound_LoadWAV_RW( SDL_RWFromConstMem(vpSym, uiSize), 0 );
	  if (sfx == nullptr) {
	  	return -1;
	  }
	  Sfxs[x] = sfx;
	}
	
return 0;
}

void closeGlobalSFX() {
	for (int x = 0; x < NUMBER_OF_GLOBAL_SFX; ++x) {
	  Sound_FreeSFX(Sfxs[x]);
	  Sfxs[x] = nullptr;
	}
}

int openAudioDevice(SDL_AudioSpec& desiredspecs) {
	// open audio device
	AudioID = SDL_OpenAudioDevice(nullptr, 0, &desiredspecs, CurrentAudioSpecs, SDL_AUDIO_ALLOW_FORMAT_CHANGE);
	if ( AudioID < 0 ) {
	  Sound_SetError(Sound_Init, SDL_GetError());
	  return -1;
	}
	
	Log_Cout("Opened audio device: frequency = %d, %s audio, %d samples per chunk\n",
		CurrentAudioSpecs->freq, (CurrentAudioSpecs->channels == 2) ? "stereo" : "mono",
		CurrentAudioSpecs->samples);
	
	#ifndef NDEBUG
	{
	  std::unique_ptr<std::string[]> _strs(new std::string[4]);
	  _strs[0] = "little";
	  _strs[1] = "big";
	  _strs[2] = "unsigned";
	  _strs[3] = "signed";
	  
	  int _format = CurrentAudioSpecs->format;
	  
	  if ( SDL_AUDIO_ISFLOAT(_format) ) {
	  	int _endian = SDL_AUDIO_ISBIGENDIAN(_format) ? 1 : 0;
	  	Log_Cout("Audio device is based on 32-bit float data in %s-endian byte order\n", _strs[_endian]);
	  }
	  else {
	  	int _endian = SDL_AUDIO_ISBIGENDIAN(_format) ? 1 : 0;
	  	int _signed = SDL_AUDIO_ISSIGNED(_format) ? 1 : 0;
	  	Log_Cout("Audio device is based on %d-bit %s integer data in %s-endian byte order\n",
	  		SDL_AUDIO_BITSIZE(_format), _strs[_signed+2].c_str(), _strs[_endian].c_str());
	  }
	}
	#endif
	
	AudioOpened = true;
	
return 0;
}

void closeAudioDevice() {
	if (AudioOpened) {
	  SDL_CloseAudioDevice(AudioID);
	  MsPerStep = 0;
	  AudioOpened = false;
	  delete[] Channel;
	}
}

int checkSFXInternal(SFXData* sfx) {
	int iFrameWidth = 1;
	if ( SDL_AUDIO_BITSIZE(CurrentAudioSpecs->format) == 16 )
	  iFrameWidth = 2;
	
	iFrameWidth *= CurrentAudioSpecs->channels;
	
	while (sfx->alen % iFrameWidth)
	  sfx->alen--;
	
return sfx->alen;
}

int internalSetPosition(double position) {
	int retval = 0;
	
	if (position == 0.0) return 0;
	
	switch (CurrentMusic->type) {
	  case MUS_TYPE_WAV:
	  	retval = WAVStream_SetPosition(position);
	  	if (retval < 0) Sound_SetError(internalSetPosition, SDL_GetError());
	  	break;
	  
	  case MUS_TYPE_OGG:
	  	Sound_SetError(internalSetPosition, "Ogg not implemented");
	  	retval = -1;
	  	break;
	  
	  default:
	  	retval = -1;
	  	break;
	}
	
return retval;
}

int internalPlayMusic(MusData* mus, double position) {
	int retval = 0;
	
	if (CurrentMusic) {
	  internalHaltMusic();
	}
	CurrentMusic = mus;
	
	switch (mus->type) {
	  case MUS_TYPE_WAV:
	  	WAVStream_Start(CurrentMusic->wav);
	  	break;
	  
	  case MUS_TYPE_OGG:
	  	// TODO play ogg
	  	break;
	  
	  default:
	  	Log_Cerr("Error from internalPlayMusic: unknown format of file\n");
	  	break;
	}
	
	// trying to set position
	if (retval == 0) {
	  if (position > 0.0) {
	  	if ( internalSetPosition(position) < 0 ) {
	  	  Sound_SetError(internalPlayMusic, "this format does not set positions");
	  	  retval = -1;
	  	}
	  }
	  else {
	  	internalSetPosition(0.0);
	  }
	}
	
	if (retval < 0) CurrentMusic = nullptr;
	
return retval;
}

void internalHaltMusic() {
	switch (CurrentMusic->type) {
	  case MUS_TYPE_WAV:
	  	void WAVStream_Stop();
	  	break;
	  
	  case MUS_TYPE_OGG:
	  	//
	  	break;
	  
	  default:
	  	break;
	}
	
	CurrentMusic->fade_status = MUS_FADE_NONE;
	CurrentMusic = nullptr;
}

void internalVolumeMusic(int vol) {
	switch (CurrentMusic->type) {
	  case MUS_TYPE_WAV:
	  	CurrentMusic->wav->volume = (uint8_t) vol;
	  	break;
	  
	  case MUS_TYPE_OGG:
	  	//
	  	break;
	  
	  default: break;
	}
}

bool internalPlayingMusic() {
	bool retval;
	
	if (CurrentMusic == nullptr)
	  return false;
	
	switch (CurrentMusic->type) {
	  case MUS_TYPE_WAV:
	  	retval = WAVStream_Playing();
	  	break;
	  
	  case MUS_TYPE_OGG: break;
	  
	  default: break;
	}
	
	
return retval;
}

int internalHaltOrLoopMusic() {
	if (! internalPlayingMusic()) {
	  // restart the music if it has to loop
	  if (CurrentMusic->loops) {
	  	int iFade = CurrentMusic->fade_status;
	  	
	  	if (CurrentMusic->loops > 0) CurrentMusic->loops--;
	  	internalPlayMusic(CurrentMusic, CurrentMusic->lpos);
	  	CurrentMusic->fade_status = iFade;
	  }
	  else {
	  	internalHaltMusic();
	  	return 0;
	  }
	}
	
return 1;
}

////////////////////// music mixer: helper to audioCallback ////////////////////////////
static void musicMixer(void* udata, uint8_t* stream, int len) {
	int iLeft = 0;
	
	if (CurrentMusic && MusicActive) {
	  // handle fading status
	  if (CurrentMusic->fade_status != MUS_FADE_NONE) {
	  	if (CurrentMusic->fade_step++ < CurrentMusic->fade_steps) {
	  	  int iVolume = CurrentMusic->volume;
	  	  int iFadeStep = CurrentMusic->fade_step;
	  	  int iFadeSteps = CurrentMusic->fade_steps;
	  	  
	  	  if (CurrentMusic->fade_status == MUS_FADE_OUT) {
	  	  	iVolume = (iVolume * (iFadeSteps - iFadeStep)) / iFadeSteps;
	  	  } // fading out
	  	  else {
	  	  	iVolume = (iVolume * iFadeStep) / iFadeSteps;
	  	  } // fading in
	  	  
	  	  internalVolumeMusic(iVolume);
	  	}
	  	else {
	  	  if (CurrentMusic->fade_status == MUS_FADE_OUT) {
	  	  	internalHaltMusic();
	  	  	return;
	  	  }
	  	  CurrentMusic->fade_status = MUS_FADE_NONE;
	  	}
	  } // end if fading status
	  
	  // return if the music has ended
	  internalHaltOrLoopMusic();
	  if (! internalPlayingMusic())
	  	return;
	  
	  // play according to the format
	  switch (CurrentMusic->type) {
	  	case MUS_TYPE_WAV:
	  	  iLeft = WAVStream_PlaySome(stream, len);
	  	  break;
	  	
	  	case MUS_TYPE_OGG: break;
	  	
	  	default: break;
	  }
	} // end if music active
	
	// handle seamless looping
	if (iLeft > 0 && iLeft < len) {
	  internalHaltOrLoopMusic();
	  if (internalPlayingMusic()) {
	  	musicMixer(udata, stream + (len - iLeft), iLeft);
	  }
	}
}

void audioCallback(void* udata, uint8_t* stream, int len) {
	int iMixable, iVolume = MAX_VOLUME;
	
	#if SDL_VERSION_ATLEAST(1, 3, 0)
	SDL_memset(stream, CurrentAudioSpecs->silence, len);
	#endif
	
	// mix the music track
	if (CurrentMusic)
	  musicMixer(udata, stream, len);
	
	// check on all the audio channels
	uint32_t uiTicks = SDL_GetTicks();
	for (int i = 0; i < _num_channels; ++i) {
	  int iIndex = 0;
	  int iRemaining = len;
	  
	  // expire the channel if it's time limit is up
	  if ( Channel[i].expire > 0 && (uint32_t) Channel[i].expire < uiTicks ) {
	  	//Sound_HaltChannel(i);
	  	_Sound_ClearChannel(i);
	  }
	  
	  // mix channel if it's playing
	  while ( Channel[i].playing > 0 && iIndex < len ) {
	  	iRemaining = len - iIndex;
	  	iMixable = Channel[i].playing;
	  	iVolume = (Channel[i].volume * Channel[i].chunk->volume) / MAX_VOLUME;
	  	
	  	if (iMixable > iRemaining) {
	  	  iMixable = iRemaining;
	  	}
	  	
	  	SDL_MixAudioFormat(stream+iIndex, Channel[i].samples, CurrentAudioSpecs->format, iMixable, iVolume);
	  	Channel[i].samples += iMixable;
	  	Channel[i].playing -= iMixable;
	  	iIndex += iMixable;
	  }
	} // end for
}
