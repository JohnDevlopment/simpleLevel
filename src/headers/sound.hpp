#ifndef SOUND_HPP
#define SOUND_HPP

#include <SDL.h>
#include <SDL_mixer.h>
#include <iostream>
#include <initializer_list>
#include <custom/levelLoad.hpp>
#include <custom/bad_option>
#include <climits>

#include "global_attributes.hpp"

#define END_OF_GLOBAL_TRACKS	0
#define MUSIC_DIR_PREFIX	"audio/music/"
#define TOTAL_TRACKS		3
#define SBFLOOP			1
#define SBFSTOPPED		2

enum SoundBitFlags {
	SBFLoop	= 1,
	SBFRew	= 2
};

// flags that determine how the music is treated in Sound_LoadMusicLooped()
enum MusicMode {
	MMSingle,
	MMPair,
	MMTrio
};

typedef enum {
	MTDeath,
	MTLevel,
	MTLevel_2,
	MTLevel_3,
	MTEnd
} MusicTrackNum;

typedef enum {
	SFXChargedJump,
	SFXJump,
	SFXHurt,
	SFXStartGame,
	SFXDiaClose,
	SFXNo,
	SFXYes,
	
	SFXCount
} SFXNum;

//struct MusicBlock {
//	Mix_Music*  music;
//	MusicBlock* parts[2];
//	uint8_t     id;
//	uint8_t     currentPart;
//	uint8_t     flags;
//	double      loop;
//	
//	MusicBlock() : music(nullptr), id(0), currentPart(0), flags(0), loop(0) {
//		parts[0] = nullptr;
//		parts[1] = nullptr;
//	}
//	
//	MusicBlock(MusicBlock&& src) {
//		// transfer ownership of 'music'
//		music = src.music;
//		src.music = nullptr;
//		
//		// copy flags
//		flags = src.flags;
//		
//		// transfer ownership of parts[0] & parts[1]
//		parts[0] = src.parts[0];
//		parts[1] = src.parts[1];
//		src.parts[0] = nullptr;
//		src.parts[1] = nullptr;
//		
//		// copy current index to alternate phase of song
//		currentPart = src.currentPart;
//		loop = src.loop;
//	}
//	
//	~MusicBlock() {clear();}
//	
//	void unset_flags(int _flags) {
//		flags &= _flags ^ 0xff;
//	}
//	
//	void clear() {
//		if (parts[0] != nullptr) {
//		  delete parts[0];
//		  parts[0] = nullptr;
//		}
//		
//		if (parts[1] != nullptr) {
//		  delete parts[1];
//		  parts[1] = nullptr;
//		}
//		
//		Mix_FreeMusic(music);
//		music = nullptr;
//		
//		currentPart = 0;
//		loop = 0.0;
//		flags = 0;
//	}
//};

//struct SFXBlock {
//	Mix_Chunk* sound;
//	
//	SFXBlock() : sound(nullptr) {}
//	
//	~SFXBlock() {clear();}
//	
//	void clear() {
//		Mix_FreeChunk(sound);
//		sound = nullptr;
//	}
//};



#endif /* SOUND_HPP */
