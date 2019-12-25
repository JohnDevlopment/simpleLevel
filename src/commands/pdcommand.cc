#include "exitcmd.h"
#include "log.hpp"
#include "game.hpp"
#include "pdgamemodemanager.h"

ExitCmd::ExitCmd()
{

}

ExitCmd::~ExitCmd()
{

}

void ExitCmd::DoAction() {
	PDGamemodeManager::Quit();
}
