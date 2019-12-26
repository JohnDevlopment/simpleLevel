/****************************************************************************/
/*
file: pdgamemodemanager.h
author: John Russell
date: 10/23/2019

A singleton that controls the creation, modification and deletion of
gamemodes.
*/
/****************************************************************************/

#ifndef PDGAMEMODEMANAGER_H_INCLUDED
#define PDGAMEMODEMANAGER_H_INCLUDED

/* System headers */
#include <memory>

/* User-defined headers */
#include "game.hpp"
#include "timer_class.hpp"
#include "pdfactory.h"
#include "pdgamemode.h"
#include "pdgamemodetypes.h"
#include "pdgamemodebuilder.h"

class PDGamemodeManager {
	static PDGameData* s_gameData;
	
	// allocates an array of this type
	static void* operator new[](std::size_t sz) throw() {
		return nullptr;
	}
	
	static void InitGamemode();
	static void ChangeGamemode();
	static void Init(const PDGameData& srcData);
	static void Update();
	static void Shutdown();
	
	public:
	
	friend class PDApp;
	
	PDGamemodeManager();
	~PDGamemodeManager();
	
	// information about the manager
	static bool IsQuitting();
	static bool IsRestarting();
	
	// returns shared game data
	static PDGameData& GetGameData();
	
	// adding and removing gamemodes
	static void AddGamemode(PDGamemodeType type, PDGamemodeBuilder* builder);
	static void RemoveGamemode(PDGamemodeType type);
	static void ClearGamemodes();
	
	// switching gamemodes
	static void SetFirstGamemode(PDGamemodeType type);
	static void SetNextGamemode(PDGamemodeType type);
	static void PauseAndChangeGamemode(PDGamemodeType next, bool drawPaused = false);
	
	// resumes the paused gamemodes
	static void Resume();
	
	// quit the manager
	static void Quit();
};

#endif /* PDGAMEMODEMANAGER_H_INCLUDED */
