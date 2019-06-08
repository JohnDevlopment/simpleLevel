#include "memory.hpp"
#include "sound.hpp"
#include "sound/sound_ogg.h"
#include "res.hpp"
#include "stdinc.h"
#include "endian.hpp"

#include <vector>

void WAVStream_FreeSong(WAVStream* wav);

#ifndef NDEBUG
 #include "log.hpp"
 #define DLOG(args...) Log_Cout(args)
 #define DPRINT(args...) std::printf(args)
#else
 #define DLOG(args...)
 #define DPRINT(args...)
#endif

/* WAVE files are little-endian */

/*******************************************/
/* Define values for Microsoft WAVE format */
/*******************************************/
#define RIFF        0x46464952      /* "RIFF" */
#define WAVE        0x45564157      /* "WAVE" */
#define FMT         0x20746D66      /* "fmt " */
#define DATA        0x61746164      /* "data" */
#define SMPL        0x6c706d73      /* "smpl" */
#define PCM_CODE    1
#define ADPCM_CODE  2
#define WAVE_MONO   1
#define WAVE_STEREO 2

/*********************************************/
/* Define values for AIFF (IFF audio) format */
/*********************************************/
#define FORM        0x4d524f46      /* "FORM" */
#define AIFF        0x46464941      /* "AIFF" */
#define SSND        0x444e5353      /* "SSND" */
#define COMM        0x4d4d4f43      /* "COMM" */

// globals
extern std::unique_ptr<SDL_AudioSpec> WaveMixer;
static WAVStream* Music = nullptr;

//////////////////////// list of functions that support LoadWAVStream ////////////////////////
static bool ParseFMT(WAVStream* wav, uint32_t len) {
	SDL_RWops* src = wav->src;
	SDL_AudioSpec* spec = &wav->spec;
	WaveFMT* format;
	std::unique_ptr<uint8_t[]> data(new uint8_t[len]);
	
	if (len < sizeof(WaveFMT)) {
	  Sound_SetError(ParseFMT, "size of FMT block too small");
	  return false;
	}
	
	// read FMT chunk into memory and interpret it as WaveFMT
	if ( ! SDL_RWread(src, data.get(), len, 1) ) {
	  Sound_SetError(ParseFMT, "failed to read bytes") \
	  << "Error: " << SDL_GetError() << '\n';
	  return false;
	}
	format = (WaveFMT*) data.get();
	
	// wrong encoding
	uint16_t uiEncoding = SwapLE16(format->encoding);
	
	if (uiEncoding != PCM_CODE) {
	  Sound_SetError(ParseFMT, "unknown WAVE format");
	  return false;
	}
	
	// frequency/sample rate
	spec->freq = SwapLE32(format->frequency);
	
	// supports 8-bit unsigned data and signed 16-bit data
	switch ( SwapLE16(format->bitspersample) ) {
	  case 8:
	  	spec->format = AUDIO_U8;
	  	break;
	  
	  case 16:
	  	spec->format = AUDIO_S16;
	  	break;
	  
	  default:
	  	Sound_SetError(ParseFMT, "unknown PCM format");
	  	return false;
	  	break;
	}
	spec->channels = (uint8_t) SwapLE16(format->channels);
	spec->samples = 4096;
	
return true;
}

static bool ParseDATA(WAVStream* wav, uint32_t len) {
	wav->start = SDL_RWtell(wav->src);
	wav->stop = wav->start + len;
	SDL_RWseek(wav->src, len, RW_SEEK_CUR);
	
return true;
}

static bool ParseSMPL(WAVStream* wav, uint32_t len) {
	std::unique_ptr<uint8_t[]> data(new uint8_t[len]);
	SamplerChunk* smpl;
	
	if ( ! SDL_RWread(wav->src, data.get(), len, 1) ) {
	  Sound_SetError(ParseSMPL, "not enough bytes from file");
	  return false;
	}
	smpl = (SamplerChunk*) data.get();
	
	for (uint32_t i = 0; i < SDL_SwapLE32(smpl->sampleloops); ++i) {
	  const uint32_t _loop_forward = 0;
	  uint32_t _type = SDL_SwapLE32(smpl->loops[i].type);
	  if (_type == _loop_forward) {
	  	// TODO add a loop point
	  }
	}
	
return true;
}

// read a WAVE or RIFF stream and return whether or not it succeeded
static bool LoadWAVStream(WAVStream* wav) {
	SDL_RWops* src = wav->src;
	uint32_t uiChType, uiChLen;
	bool found_DATA = false, found_FMT = false;
	
	// wave header
	{
	  SDL_ReadLE32(src); // wave length
	  SDL_ReadLE32(src); // wave "magic" (ie, "WAVE" characters)
	}
	
	// FMT, DATA, and SMPL chunks
	while (1) {
	  uiChType = SDL_ReadLE32(src);
	  uiChLen = SDL_ReadLE32(src);
	  
	  if (uiChLen == 0) break;
	  
	  switch (uiChType) {
	  	case FMT:
	  	  found_FMT = true;
	  	  if ( ! ParseFMT(wav, uiChLen) ) return false;
	  	  break;
	  	
	  	case DATA:
	  	  found_DATA = true;
	  	  if ( ! ParseDATA(wav, uiChLen) ) return false;
	  	  break;
	  	
	  	case SMPL:
	  	  // if ( ! ParseSMPL(wav, uiChLen) ) return false;
	  	  Sound_SetError(LoadWAVStream, "SMPL not currently supported");
	  	  return false; // FIXME arbitrarily returns false
	  	  break;
	  	
	  	default:
	  	  SDL_RWseek(src, uiChLen, RW_SEEK_CUR);
	  	  break;
	  }
	}
	
	if (! found_FMT) {
	  Sound_SetError(LoadWAVStream, "FMT block not found; bad WAV file");
	  return false;
	}
	
	if (! found_DATA) {
	  Sound_SetError(LoadWAVStream, "DATA block not found; bad WAV file");
	  return false;
	}
	
return true;
}

static int PlaySome(uint8_t* stream, int len) {
	int64_t iPos, iStop; // , iLoopStart, iLoopStop
//	WAVLoopPoint* loop = nullptr;
	int iConsumed;
	
	iPos = SDL_RWtell(Music->src);
	iStop = Music->stop;
	
//	// loop points
//	for (int x = 0; x < Music->numloops; ++x) {
//	  const int iBytesPerSample = SDL_AUDIO_BITSIZE(Music->spec.format) / 8;
//	  
//	  loop = &Music->loops[x];
//	  iLoopStart = Music->start + loop->start * iBytesPerSample;
//	  iLoopStop = Music->start + (loop->stop + 1) * iBytesPerSample;
//	  
//	  if (iPos >= iLoopStart && iPos < iLoopStop) {
//	  	iStop = iLoopStop;
//	  	break;
//	  }
//	  loop = nullptr;
//	}
	
	// if a conversion is needed
	if (Music->cvt.needed) {
	  int iOriginalLength = static_cast<int>( (double) len / Music->cvt.len_ratio );
	  
	  // conversion length isn't the same
	  if (Music->cvt.len != iOriginalLength) {
	  	const int iWorksize = iOriginalLength * Music->cvt.len_mult;
	  	
	  	// replace old buffer (if it exists) with new bufer of iOriginalLength bytes
	  	if (Music->cvt.buf)
	  	  delete[] Music->cvt.buf;
	  	
	  	Music->cvt.buf = new uint8_t[iWorksize];
	  	Music->cvt.len = iOriginalLength;
	  }
	  
	  // if less than iOriginalLength left to this music buffer
	  if ( (iStop - iPos) < iOriginalLength ) {
	  	iOriginalLength = static_cast<int>(iStop - iPos);
	  }
	  
	  // read from the wave file into the buffer
	  iOriginalLength = SDL_RWread(Music->src, Music->cvt.buf, 1, iOriginalLength);
	  
	  // decrease the buffer length if it is odd and we're dealing with 16-bit audio, due to how SDL_ConvertAudio works
	  if ( (Music->cvt.src_format & 0x0010) && (iOriginalLength & 1) ) {
	  	--iOriginalLength;
	  }
	  
	  // size of the original audio
	  Music->cvt.len = iOriginalLength;
	  
	  // convert and then mix the audio
	  SDL_ConvertAudio(&Music->cvt);
	  SDL_MixAudioFormat(stream, Music->cvt.buf, WaveMixer->format, Music->cvt.len_cvt, Music->volume);
	  iConsumed = Music->cvt.len_cvt;
	}
	
	// no conversion
	else {
	  uint8_t* data;
	  
	  // if less than len left to this music buffer
	  if ((iStop - iPos) < len) {
	  	len = static_cast<int>(iStop - iPos);
	  }
	  
	  // copy the audio data to the buffer and then mix it
	  data = new uint8_t[len];
	  len = SDL_RWread(Music->src, data, 1, len);
	  SDL_MixAudioFormat(stream, data, WaveMixer->format, len, Music->volume);
	  delete[] data;
	  iConsumed = len;
	}
	
	/* If a loop point is active, seek to that point in the file
	   and decrement the play count. */
//	if (loop && SDL_RWtell(Music->src) >= iStop) {
//	  if (loop->current_play_count == 1) {
//	  	loop->active = false;
//	  }
//	  else if (loop->current_play_count > 0) {
//	  	loop->current_play_count--;
//	  }
//	  SDL_RWseek(Music->src, iLoopStart, RW_SEEK_SET);
//	}
	
return iConsumed;
}

///////////////////// public functions //////////////////////////
int WAVStream_SetPosition(double position) {
	const short int iSampleSize = SDL_AUDIO_BITSIZE(Music->spec.format) / 8 * Music->spec.channels;
	int64_t iOffset = static_cast<int64_t>(position * (double) Music->spec.freq * (double) iSampleSize);
	
	auto retval = SDL_RWseek(Music->src, Music->start, RW_SEEK_SET);
	if (retval >= 0) {
	  retval = SDL_RWseek(Music->src, iOffset, RW_SEEK_CUR);
	  if (retval < 0) Sound_SetError(WAVStream_SetPosition, SDL_GetError());
	}
	
return retval;
}

int WAVStream_PlaySome(uint8_t* stream, int len) {
	if (! Music) return 0;
	
	while ( SDL_RWtell(Music->src) < Music->stop && len > 0 ) {
	  int iConsumed = PlaySome(stream, len);
	  if (! iConsumed)
	  	break;
	  
	  stream += iConsumed;
	  len -= iConsumed;
	}
	
return len;
}

// returns true if the wave is in the middle of playback
bool WAVStream_Playing() {
	bool retval = false;
	
	if (Music && SDL_RWtell(Music->src) < Music->stop)
	  retval = true;
	
return retval;
}

// prep the wave stream for playback
void WAVStream_Start(WAVStream* wav) {
	for (int x = 0; x < wav->numloops; ++x) {
	  WAVLoopPoint* loop = &wav->loops[x];
	  loop->active = true;
	  loop->current_play_count = loop->initial_play_count;
	}
	SDL_RWseek(wav->src, wav->start, RW_SEEK_SET);
	Music = wav;
}

// load wave data from a given SDL_RWops object
WAVStream* WAVStream_LoadSong_RW(SDL_RWops* src, int freesrc) {
	WAVStream* wave;
	bool loaded = false;
	uint32_t uiMagic;
	
	// allocate pointer; set initial values; give data buffer to WAVStream
	wave = new WAVStream;
	SDL_zerop(wave);
	wave->src = src;
	wave->freesrc = (bool) freesrc;
	wave->volume = MAX_VOLUME;
	
	// make sure it's a WAV file before going on
	uiMagic = SDL_ReadLE32(src);
	if (uiMagic == RIFF || uiMagic == WAVE) {
	  loaded = LoadWAVStream(wave);
	}
	else {
	  Sound_SetError(WAVStream_LoadSong_RW, "do not recognize wave format");
	}
	
	if (! loaded) {
	  WAVStream_FreeSong(wave);
	  Sound_SetError(WAVStream_LoadSong_RW, "failed to load WAVE file");
	  return nullptr;
	}
	
	// build information on how to convert audio to device format
	SDL_BuildAudioCVT(&wave->cvt, wave->spec.format, wave->spec.channels, wave->spec.freq,
		WaveMixer->format, WaveMixer->channels, WaveMixer->freq);
	
return wave;
}

void WAVStream_FreeSong(WAVStream* wav) {
	if (wav) {
	  if (wav->loops) {
	  	delete wav->loops;
	  }
	  if (wav->cvt.buf) {
	  	SDL_free(wav->cvt.buf);
	  }
	  if (wav->freesrc) {
	  	SDL_RWclose(wav->src);
	  }
	  
	  delete wav;
	}
}

void WAVStream_Stop() {
	Music = nullptr;
}
