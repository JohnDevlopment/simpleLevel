#include "game.hpp"
#include "texture_packs.hpp"
#include "levelinfo.hpp"
#include "math.hpp"
#include "log.hpp"
#include "stdinc.h"

using namespace std;

TexturePack** SpriteTXPackList = nullptr;

static void cleanup() {
	for (int x = 0; x < 2; ++x) {
	  TXPack_Free(SpriteTXPackList[x]);
	}
	delete[] SpriteTXPackList;
	game::Flags.unset(TEXTURE_PACKS);
}

// public functions //
int NewTexturePacks(SDL_Renderer* renderer) {
	using game::Flags;
	
	uint8_t uipColor[4];
//	uint8_t* uipColor = nullptr;
	
	if ( Flags.mask(TEXTURE_PACKS) ) return 0;
	std::atexit(cleanup);
	SpriteTXPackList = new TexturePack*[2];
	
	// load shadowflytrap.jpg
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
	
	PrintExpr(SpriteTXPackList[0]);
	PrintExpr(SpriteTXPackList[1]);
	
	Flags.set(TEXTURE_PACKS);
	
return 0;
}
