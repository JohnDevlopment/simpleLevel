#include "levelinfo.hpp"
#include "str_const.hpp"

#define NUMBER_VALID_SONGS	3
#define NUMBER_GLOBAL_SFX	7

constexpr str_const _sfx_syms[NUMBER_GLOBAL_SFX] = {
	"_binary_chgjump_wav_start",
	"_binary_jump_small_wav_start",
	"_binary_player_hurt_wav_start",
	"_binary_startgame_wav_start",
	"_binary_diaclose_wav_start",
	"_binary_no_wav_start",
	"_binary_yes_wav_start"
};

constexpr str_const _music_files[NUMBER_VALID_SONGS+1] = {
	"...",
	"planning_partA.ogg",
	"death.wav",
	"gogo.ogg"
};

constexpr str_const _bg_files[3] = {
	"...",
	"clear_sky.png trees.png",
	"grassy_field.png"
};

static const double _loops[3] = {3.2, 0.0, 0.0};

static const float _bgmult[8] = {
	0, 0, 0, 0,
	0.2f, 0.95f, 0, 0
};

const char* LevelInfo_Background(const unsigned int index) {
	assert(index < 2);
	
return _bg_files[index].get();
}

std::string LevelInfo_Tileset(const unsigned int index) {
	std::string buffer;
	
	// index: 0-255
	if (index < 10) {
	  buffer = "00";
	}
	else if (index < 100) {
	  buffer = "0";
	}
	else if (index < 256) {
	  // nothing
	}
	else {
	  buffer = "000.png";
	  goto skip;
	}
	
	// add stringified number and extension
	buffer += std::to_string(index);
	buffer += ".png";
	
skip:
	
return std::move(buffer);
}

const char* LevelInfo_Music(const unsigned int index) {
	constexpr unsigned int _valid_count = 4;
	
	static_assert(_valid_count == (NUMBER_VALID_SONGS+1), "_valid_count is not equal to NUMBER_VALID_SONGS + 1");
	
	assert(index < _valid_count);
	
return _music_files[index].get();
}

int LevelInfo_MusicID(const std::string& str) {
	int retval;
	
	// locate a file name in an array and return its index number
	retval = -1;
	for (int x = 0; x < NUMBER_VALID_SONGS; ++x) {
	  if (str == _music_files[x].get()) {
	  	retval = x;
	  	x = NUMBER_VALID_SONGS;
	  }
	}
	
return retval;
}

const char* LevelInfo_Sfx(unsigned int id) {
	assert(id < NUMBER_GLOBAL_SFX);
	
return _sfx_syms[id].get();
}

bool LevelInfo_TexturePack(unsigned int value, TexurePackArgs& args) {
	static const char _colorkey[2] = {1, 1};
	
	assert(value < NUMBER_OF_TEXTURE_PACKS);
	
	// treat a 4-count 8-bit array like it`s a 32-bit integer
	uint32_t* rgb = reinterpret_cast<uint32_t*>(args.rgb);
		
	// string filename, rgb colorkeys
	switch (value) {
	  default:
	  	args.filename = "nothing";
	  	*rgb = 0;
	  	break;
	  
	  case 0:
	  	args.filename = "shadowflytrap.jpg";
	  	*rgb = 0xffffff00;
	  	break;
	}
	
	// colorkey true/false flag
	args.colorkey = _colorkey[value];
	
return true;
}

double LevelInfo_MusicLoop(const size_t id) {
	assert(id < 3);
	
return _loops[id];
}

const float* LevelInfo_BGScrollSpeeds(const size_t id) {
	assert(id < 4);
	
return reinterpret_cast<const float*>(_bgmult + (id * 4));
}
