// global game data
#include "headers/game.hpp"

// texture packs
#include "headers/texture_packs.hpp"

// info about a level
#include "headers/levelinfo.hpp"

// math functions
#include "headers/math.hpp"

// logging system
#include "headers/log.hpp"

using namespace std;

// texture pack list for all sprite types
StaticDArray<TexturePack*, NUMBER_OF_UNIQUE_SPRITES> SpriteTXPackList;

// texture packs
static StaticDArray<TexturePack, 2> _static_global_TXPacks;

// 
constexpr unsigned int _global_data_sprite_txpacks[NUMBER_OF_UNIQUE_SPRITES] = {
	0, 1, 1, 1, 1, 0
};

// public functions //
int NewTexturePacks(SDL_Renderer* renderer) {
	using game::Flags;
	
	typedef unsigned int UINT;
	
	// only execute this once
	if (Flags.mask(TEXTURE_PACKS))
	  return 0;
	
	// arguments to create a texture pack
	TexurePackArgs args;
	
	// load a texture pack
	for (UINT x = 0; x < 2; ++x) {
	  // directory path
	  const char* dir = "images/sprites/";
	  
	  // fill struct
	  if (! LevelInfo_TexturePack(x, args)) break;
	  
	  // combine directory path and file name
	  args.filename = dir + std::move(args.filename);
	  
	  // array of four 8-bit integers; swap if on little-endian architecture
	  #if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
	  Swap32bit(args.rgb);
	  #endif
	  
	  // create texture pack using the information from TexurePackArgs
	  TexturePack* tPack = TXPack_NewTexturePack(renderer, std::move(args.filename),
	                                             (char) args.colorkey, args.rgb);
	  
	  // return if the process fails
	  if (tPack == nullptr) {
	  	cerr << "Unable to load texture pack\n";
	  	return 1;
	  }
	  
	  // print debug information to file
	  Log_Cout("Texture pack #%d loaded from image file...file: %s...address: %p\n",
	           x, tPack->file, tPack);
	  
	  // move pack to array
	  {
	  	TexturePack& trPack = *tPack;
	  	_static_global_TXPacks[x] = std::move(trPack);
	  }
	  delete tPack;
	}
	
	/*
	    _global_data_sprite_txpacks contains a dynamic array of TexturePack structures. SpriteTXPackList holds a list of pointers
	    that are meant to keep the addresses of one or more elements from the former. txpack_idx is the difference between the
	    iterator's position within SpriteTXPackList, and the beginning of that array. _global_data_sprite_txpacks is a table
	    of values that offset the array inside _global_data_sprite_txpacks; this is where we get the pointers to put inside of
	    SpriteTXPackList. ptr_txpacks is a pointer to one of _static_global_TXPacks elements, offset by txpack_idx.
	*/
	for (auto itr = SpriteTXPackList.begin(); itr != SpriteTXPackList.end(); ++itr) {
	  UINT txpack_idx = (UINT) itr - (UINT) SpriteTXPackList.begin();
	  txpack_idx = _global_data_sprite_txpacks[txpack_idx/4];
	  
	  auto ptr_txpacks = _static_global_TXPacks.begin() + txpack_idx;
	  *itr = ptr_txpacks;
	}
	
return 0;
}
