#include "game.hpp"
#include "levelcode.hpp"
#include "sprite_router.hpp"

using level::ThePlayer;
using namespace std;

typedef int (*SprInitFunc)(Sprite_t* const);

typedef int (*SprMainFunc)(Sprite_t* const);

static const SprInitFunc _Funcs[6] = {
	nullptr, nullptr, nullptr,
	nullptr, nullptr, nullptr
};

static const SprMainFunc _MFuncs[6] = {
	nullptr, nullptr, nullptr,
	nullptr, nullptr, nullptr
};

int Sprite_Init(Sprite_t* const spr) {
	assert(spr->id == 0);
	
return _Funcs[spr->id](spr);
}
