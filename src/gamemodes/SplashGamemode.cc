#include "SplashGamemode.h"
#include "pdapp.h"
#include "pdinifile.h"
#include "pdgamemodemanager.h"
#include "game.hpp"
#include "log.hpp"
#include "objecthelper.h"

PDSplashGamemode::PDSplashGamemode() : m_counter(0), m_maxTime(0)
{
}

PDSplashGamemode::~PDSplashGamemode()
{
}

void PDSplashGamemode::Init() {
	PDGameData& gmd = PDGamemodeManager::GetGameData();
	
	PDIniFile ini;
	std::string sObj = GAMEMODE_DATA_PATH;
	ini.ReadFile(sObj + "splash.ini");
	
	// max time for gamemode
	float fVal;
	ini.GetValue("maxTime", fVal);
	m_maxTime = static_cast<int>(fVal * gmd.fps);
	
	// objects to define
	LoadObjects(ini);
}

void PDSplashGamemode::Update(float dt) {
	if (++m_counter == m_maxTime) {
	  game::Shutdown->DoAction();
	}
}

void PDSplashGamemode::Shutdown() {
}
