#include "SplashGamemode.h"
#include "pdapp.h"
#include "pdinifile.h"
#include "pdgamemodemanager.h"
#include "game.hpp"
#include "log.hpp"
#include "objecthelper.h"
#include "textures.h"
#include "camera.h"

PDSplashGamemode::PDSplashGamemode() : m_counter(0), m_maxTime(0)
{
}

PDSplashGamemode::~PDSplashGamemode()
{
}

void PDSplashGamemode::Init() {
	PDIniFile ini;
	std::string sObj = GAMEMODE_DATA_PATH;
	ini.ReadFile(sObj + "splash.ini");
	
	// max time for gamemode
	float fVal;
	ini.GetValue("maxTime", fVal);
	m_maxTime = static_cast<int>(fVal * PDGamemodeManager::GetGameData().fps);
	
	// objects to define
	LoadObjects(ini);
}

void PDSplashGamemode::Update(float dt) {
	if (++m_counter == m_maxTime) {
	  PDGamemodeManager::Quit();
	}
}

void PDSplashGamemode::Shutdown() {
}
