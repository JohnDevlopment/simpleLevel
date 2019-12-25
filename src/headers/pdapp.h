#ifndef PDAPP_H_INCLUDED
#define PDAPP_H_INCLUDED

#include "stdinc.h"
#include "sdl_incs.h"
#include "program.hpp"
#include "bitfield_class.hpp"
#include "game.hpp"
#include "pdresourcemanager.hpp"

struct PDInitData {
	std::string title;
	PDGameData* gameData;
	int width;
	int height;
	int fps;
	bool success;
};

class PDApp {
	friend class PDGamemodeManager;
	
	static PDInitData* s_initData;
	static PROGRAM s_program;
	static Bitfield<uint8_t> s_flags;
	static bool s_isQuitting;
	
	static void DestroyWindow();
	
public:
	static PDResourceManager s_manager;
	
	PDApp();
	~PDApp();
	
	static void Init(PDInitData& initData);
	static void Shutdown();
	static void Update();
};

#endif /* PDAPP_H_INCLUDED */
