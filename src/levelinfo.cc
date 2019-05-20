#include "headers/levelinfo.hpp"

#define NUMBER_VALID_SONGS	3

static std::string buffer;

constexpr double LoopStart[4] = {
	0, 6.7, 0, 0
};

static Image* Images[] = {&BG_BG1, &BG_BG2};

static std::vector<std::string> sMusicFiles;

Image* LevelInfo_BackgroundLayerID(unsigned int id) {
	assert(id < 2);
	return Images[id];
}

const char* LevelInfo_Background(const LevelHeader& header) {
	buffer.clear();
	
	switch (header.background) {
	  default:
	  	buffer = "clear_sky.png";
	  	break;
	  
	  case 0:
	  	buffer = "clear_sky.png";
	  	break;
	  
	  case 1:
	  	buffer = "grassy_field.png";
	  	break;
	}
	
return buffer.c_str();
}

const char* LevelInfo_Tileset(const unsigned int index) {
	buffer.clear();
	
	switch(index) {
	  default:
	  case 0:
	  	buffer = "000.png";
	  	break;
	  
	  case 1:
	  	buffer = "001.png";
	  	break;
	  
	  case 2:
	  	buffer = "002.png";
	  	break;
	}
	
return buffer.c_str();
}

const char* LevelInfo_Music(const unsigned int index) {
	
	switch (index) {
	  default:
	  	buffer = "beta_level.ogg";
	  	break;
	  
	  case 0:
	  	buffer="death.wav";
	  	break;
	  
	  case 1:
	  	buffer = "gogo.ogg";
	  	break;
	  
	  case 2:
	  	buffer = "planning_partA.ogg";
	  	break;
	}
	
return buffer.c_str();
}

int LevelInfo_MusicID(const std::string& str) {
	using std::string;
	
	if ( sMusicFiles.empty() ) {
	  sMusicFiles.push_back( string("death.wav") );
	  sMusicFiles.push_back( string("gogo.ogg") );
	  sMusicFiles.push_back( string("planning_partA.ogg") );
	}
	
	const char* sFiles[] = {
	  "death.wav", "gogo.ogg", "planning_partA.ogg"
	};
	
	int ret = -1;
	
	for (int x = 0; x < NUMBER_VALID_SONGS; ++x) {
	  if (str == sFiles[x]) {
	  	ret = x;
	  	x = NUMBER_VALID_SONGS;
	  }
	}
	
return ret;
}

const char* LevelInfo_Sprite(unsigned int id) {
	switch(id) {
	  default:
	  	buffer = "beta_mario.bmp";
	  	break;
	  
	  case 0:
	  	buffer = "beta_mario.bmp";
	  	break;
	}
	
return buffer.c_str();
}

const char* LevelInfo_Sfx(unsigned int id) {
	switch (id) {
	  case 0:
	  	buffer = "_binary_chgjump_wav_start";
	  	break;
	  
	  case 1:
	  	buffer = "_binary_jump_small_wav_start";
	  	break;
	  
	  case 2:
	  	buffer = "_binary_player_hurt_wav_start";
	  	break;
	  
	  case 3:
	  	buffer = "_binary_startgame_wav_start";
	  	break;
	  
	  case 4:
	  	buffer = "_binary_diaclose_wav_start";
	  	break;
	  
	  case 5:
	  	buffer = "_binary_no_wav_start";
	  	break;
	  
	  case 6:
	  	buffer = "_binary_yes_wav_start";
	  	break;
	  
	  default:
	  	buffer = "_binary_hello_world_wav_start";
	  	break;
	}
	
return buffer.c_str();
}

inline uint32_t* get32bitPtr(void* const ptr) {
	return reinterpret_cast<uint32_t*>(ptr);
}

bool LevelInfo_TexturePack(unsigned int value, TexurePackArgs& args) {
	if (value >= NUMBER_OF_TEXTURE_PACKS) {
	  std::cerr << value << " too large; only " << NUMBER_OF_TEXTURE_PACKS \
	  << " texture packs are supported right now.\n";
	  return false;
	}
	
	uint32_t* rgb = get32bitPtr(args.rgb);
	
	// string filename, rgb colorkeys
	switch (value) {
	  default:
	  	args.filename = "nothing.bmp";
	  	*rgb = 0;
	  	break;
	  
	  case 0:
	  	args.filename = "sprites01.png";
	  	*rgb = 0xff00ff00;
	  	break;
	  
	  case 1:
	  	args.filename = "platforms.png";
	  	*rgb = 0xffffff00;
	  	break;
	}
	
	// colorkey true/false flag
	{
	  const char colorkey[] = { 1, 1 };
	  args.colorkey = colorkey[value];
	}
	
return true;
}

double LevelInfo_MusicLoop(const size_t id) {
	if (id >= NUMBER_VALID_SONGS) return 3.2;
	return LoopStart[id];
}
