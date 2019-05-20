// global game data
#include "headers/game.hpp"

// open and read binary resource files
#include "headers/res.hpp"

// sound system
#include "headers/sound.hpp"

// info about a level
#include "headers/levelinfo.hpp"

// Tcl scripts
#include "headers/tcl.hpp"

// level data and functions
#include "headers/levelcode.hpp"

// multi-threaded functionality
#include "headers/threads.hpp"

// thread locks and mutexes
#include "headers/locks.hpp"

// logging functionality
#include "headers/log.hpp"

// std::list
#include <list>

// stdLLvector
#include <vector>

// memory
#include <new>

using namespace std;
using game::Flags;

// sound effects
//static SFXBlock* SFXList = nullptr;

//// music system
//static MusicBlock* CurrentTrack = nullptr; // currently playing track
//static MusicBlock* MusicList    = nullptr; // list of music tracks

//// threads
//static Thread*    SoundThread = nullptr;
//static MutexCond* SoundLock = nullptr;

// private helper functions //
//constexpr double _data_loops[3] = {
//	0.0, 6.7, 0.0
//};

//static int soundThreadFunction(void* data) {
//	// lock the mutex
//	SoundLock->lock.aquire();
//	
//	// check on the status of the song
//	while (! SoundLock->flag) {
//	  // wait on a signal
//	  SoundLock->cond.wait( SoundLock->lock.get() );
//	  
//	  // if looping, play back from loop position
//	  if (CurrentTrack->flags & SBFLOOP)
//	  	Mix_FadeInMusicPos(CurrentTrack->music, 1, 100, CurrentTrack->loop);
//	  else
//	  	Sound_Stop();
//	  
//	  // toggle flag that says music has stopped
//	  CurrentTrack->unset_flags(SBFSTOPPED);
//	}
//	
//	// unlock the mutex
//	SoundLock->lock.release();
//	
//return 0;
//}

//static void musicfinished() {
//	if (CurrentTrack != nullptr) {
//	  CurrentTrack->flags |= SBFSTOPPED;
//	  SoundLock->cond.signal();
//	}
//}

//static void loadGlobalMusic() {
//	string file;
//	
//	for (int x = 0; x < END_OF_GLOBAL_TRACKS; ++x) {
//	  // file name of the music track
//	  file = MUSIC_DIR_PREFIX;
//	  file += LevelInfo_Music(x);
//	  
//	  // load music file
//	  CurrentTrack = MusicList + x;
//	  CurrentTrack->music = Mix_LoadMUS( file.c_str() );
//	  
//	  if (CurrentTrack->music == nullptr)
//	  	cerr << "Failed to load " << file << ": " << Mix_GetError() << '\n';
//	  
//	  // configure loop
//	  CurrentTrack->loop = _data_loops[x];
//	  
//	  // ...
//	  CurrentTrack = nullptr;
//	}
//}

//static SDL_RWops* rwopsFromSymbol(const char* sym) {
//	SDL_RWops* rw = nullptr;
//	size_t sz = 0;
//	void* ptr = DLPtr(sym, &sz);
//	
//	if (ptr != nullptr) {
//	  rw = SDL_RWFromConstMem(ptr, static_cast<int>(sz));
//	  if (rw == nullptr)
//	  	Log_Cerr("Unable to load sound effect, %s, due to error: %s\n", sym, SDL_GetError());
//	}
//	
//return rw;
//}

//void loadSFX() {
//	typedef unsigned int UINT;
//	
//	for (UINT x = 0; x < (UINT) SFXCount; ++x) {
//	  SDL_RWops* rw = rwopsFromSymbol( LevelInfo_Sfx(x) );
//	  if (rw == nullptr) continue;
//	  
//	  // load sfx
//	  SFXList[x].sound = Mix_LoadWAV_RW(rw, 1);
//	  if (SFXList[x].sound == nullptr) {
//	  	Log_Cerr("Failed to load sfx index %d : %s\n", x, Mix_GetError());
//	  	SDL_RWclose(rw);
//	  }
//	}
//}

//bool Sound_Playing() {
//	return (bool) Mix_PlayingMusic();
//}

//bool Sound_Init() {
//	// already initted
//	if (Flags.mask(SOUND_API_INIT)) {
//	  cerr << "Warning: Sound_Init called when API was already loaded.\n";
//	  return true;
//	}
//	
//	// allocate resources
//	SFXList     = new SFXBlock[SFXCount];
//	MusicList   = new MusicBlock[MTEnd];
//	SoundThread = new Thread (soundThreadFunction, "SoundMain");
//	SoundLock   = new MutexCond;
//	
//	// allocate a list of "global" music tracks
//	loadGlobalMusic(); // list: 'game over' music
//	
//	// allocate a list of sound effects
//	loadSFX();
//	
//	// register hook function
//	Mix_HookMusicFinished(musicfinished);
//	
//	// flag indicates API is loaded
//	Flags.set(SOUND_API_INIT);
//	
//return true;
//}

//void Sound_Quit() {
//	Sound_Stop(); // stop all music
//	Mix_HookMusicFinished(nullptr); // disable hook
//	
//	// delete music and sound arrays
//	delete[] SFXList;
//	delete[] MusicList;
//	
//	// delete thread class
//	delete SoundThread;
//	SoundThread = nullptr;
//	
//	// delete lock structure
//	delete SoundLock;
//	SoundLock = nullptr;
//	
//	// toggle a flag that says whether sound is working or not
//	Flags.unset(SOUND_API_INIT);
//}

//bool Sound_LoadMusicFromIndex(const unsigned int track, const MusicTrackNum index) {
//	// get a file name based on the first argument
//	const char* sFileName = LevelInfo_Music(track + (END_OF_GLOBAL_TRACKS + 1) );
//	
//	cout << "Music index " << track + (END_OF_GLOBAL_TRACKS + 1) << endl;
//	
//return Sound_LoadMusic(sFileName, index);
//}

//bool Sound_LoadMusic(string file, const MusicTrackNum index) {
//	if ( ! Flags.mask(SOUND_API_INIT) ) {
//	  cerr << "Sound_Init needs to be called before Sound_LoadMusic can be called.\n";
//	  return false;
//	}
//	
//	// pointer to music slot
//	MusicBlock* pMus = MusicList + index;
//	pMus->clear();
//	
//	// unselect the track we're modifying
//	if (CurrentTrack == pMus) {
//	  Sound_Stop();
//	  CurrentTrack = nullptr;
//	}
//	
//	// get track ID number
//	pMus->id = (int8_t) LevelInfo_MusicID(file);
//	
//	cout << "Music id: " << (int) pMus->id << endl;
//	
//	// file path
//{
//	string temp = std::move(file);
//	file = MUSIC_DIR_PREFIX + std::move(temp);
//}
//	
//	// load music track
//	pMus->music = Mix_LoadMUS( file.c_str() );
//	
//	if (pMus->music == nullptr) {
//	  cerr << "Sound_LoadMusic -- failed to load " << file << " : " << Mix_GetError() << endl;
//	  return false;
//	}
//	
//return true;
//}

//void Sound_PlayMusicOnce(MusicTrackNum id) {
//	CurrentTrack = MusicList + (int) id;
//	Mix_PlayMusic(CurrentTrack->music, 1);
//}

//void Sound_PlayMusicLooped(MusicTrackNum id) {
//	if ( ! Flags.mask(SOUND_API_INIT) ) {
//	  cerr << "Sound_Init needs to be called before Sound_LoadMusic can be called.\n";
//	  return;
//	}
//	
//	// MTEnd isn't a real track, it's the count
//	if (id == MTEnd) {
//	  cerr << "Sound_PlayMusicLooped: invalid index, MTEnd\n";
//	  return;
//	}
//	
//	// to prevent a segmentation fault
//	if (SoundLock == nullptr) return;
//	
//	// play the current music track with looping enabled
//	CurrentTrack = MusicList + (int) id;
//	CurrentTrack->flags |= SBFLOOP;
//	
//	// what is the loop value?
//	CurrentTrack->loop = LevelInfo_MusicLoop(CurrentTrack->id);
//	
//	if ( Mix_PlayMusic(CurrentTrack->music, 1) == 0 ) {
//	  if ( SoundThread->create() == ThreadError )
//	  	cerr << SoundThread->get_error() << '\n';
//	  else
//	  	SoundLock->flag = SDL_FALSE;
//	}
//}

//void Sound_PlaySFX(SFXNum id, int loops, int chan) {
//	// loop == 1: play twice
//	if (loops >= 1)
//	  --loops;
//	
//	// SFXCount only used for counting the number of SFX
//	if (id == SFXCount) {
//	  cerr << "Sound_PlaySFX: invalid parameter, SFXCount is not meant to be used here\n";
//	  return;
//	}
//	
//	// find SFX
//	if ( Mix_PlayChannel(chan, SFXList[(int) id].sound, loops) < 0 )
//	  cerr << "Sound_PlaySFX: " << Mix_GetError() << '\n';
//}

//void Sound_Stop(int fadeout) {
//	if ( ! Flags.mask(SOUND_API_INIT) ) return;
//	
//	if (CurrentTrack != nullptr) {
//	  if (CurrentTrack->flags & SBFLOOP) {
//	  	CurrentTrack->flags |= SBFSTOPPED;
//	  	CurrentTrack->unset_flags(SBFLOOP);
//	  	
//	  	// make thread no longer wait for condition variable
//	  	SoundLock->flag = SDL_TRUE;
//	  	SoundLock->cond.signal();
//	  	
//	  	// kill the thread that should be running
//	  	SoundThread->detach();
//	  }
//	}
//	
//	// fade out if argument > 0, otherwise stop music instantly
//	if (fadeout > 0) 
//	  Mix_FadeOutMusic(fadeout);
//	else
//	  Mix_HaltMusic();
//}

//void Sound_Free(MusicTrackNum id) {
//	if ( ! Flags.mask(SOUND_API_INIT) ) return;
//	
//	MusicBlock* pMus = MusicList + (int) id;
//	pMus->clear();
//	CurrentTrack = CurrentTrack == pMus ? nullptr : CurrentTrack;
//}
