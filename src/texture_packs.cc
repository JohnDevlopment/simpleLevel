#include "game.hpp"
#include "texture_packs.hpp"
#include "levelinfo.hpp"
#include "math.hpp"
#include "log.hpp"
#include "stdinc.h"

using namespace std;

TexturePack** SpriteTXPackList = nullptr;

void FreeTexturePacks() {
	if (SpriteTXPackList) {
	  // free texture packs
	  for (int x = 0; x < 2; ++x) {
	  	if (SpriteTXPackList[x])
	  	  TXPack_Free(SpriteTXPackList[x]);
	  }
	  
	  // delete list of pointers
	  delete[] SpriteTXPackList;
	  
	  // clear bit flag
	  game::Flags.unset(TEXTURE_PACKS);
	}
}

// public functions //
int NewTexturePacks(SDL_Renderer* renderer) {
	using game::Flags;
	
	uint8_t uipColor[4];
	
	// return if bit flag is set
	if (Flags.mask(TEXTURE_PACKS))
	  return 0;
	
	Log_Cout("### Allocating Texture Packs ###\n");
	
	// new list
	SpriteTXPackList = new TexturePack*[2];
	
	// load Shadow Fly Trap shared graphics
	uipColor[0] = 255;
	uipColor[1] = 255;
	uipColor[2] = 255;
	uipColor[3] = 255;
	TexturePack* pack = TXPack_NewTexturePack(renderer, "images/sprites/shadowflytrap.png", 0x01, uipColor);
	
	if (pack == nullptr) return 1;
	SpriteTXPackList[0] = pack;
	
	// null
	pack = nullptr;
	SpriteTXPackList[1] = pack;
	
	// set bit flag
	Flags.set(TEXTURE_PACKS);
	
	Log_Cout("Allocated texture packs: 0 = %p, 1 = %p\n",
	  SpriteTXPackList[0], SpriteTXPackList[1]);
	
return 0;
}
