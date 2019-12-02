#include "pdexitcmd.h"
#include "log.hpp"
#include "game.hpp"
#include "pdgamemodemanager.h"

PDExitCmd::PDExitCmd()
{

}

PDExitCmd::~PDExitCmd()
{

}

void PDExitCmd::DoAction() {
	PDGamemodeManager::Quit();
}
