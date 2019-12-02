// standard headers
#include "stdinc.h"
#include "sdl_incs.h"
#include "memory.hpp"
#include "check_global_defines.hpp"

// engine headers
#include "game.hpp"
#include "tcl.hpp"
#include "res.hpp"
#include "log.hpp"
#include "pdgamemode.h"
#include "windows_or_linux.hpp"
#include "pdexitcmd.h"
#include "pdapp.h"
#include "pdinifile.h"

// external code headers
#include "timer_class.hpp"

// private function declarations
static void initializePDApp() COLD;

using namespace std;

namespace game {
	Bitfield<uint16_t> Flags;
	uint8_t FrameCounter = 0;
	PDExitCmd* Shutdown = nullptr;
}

static PDInitData _InitData;

void initializePDApp() {
	PDIniFile initFile;
	
	// read initData.ini
	initFile.ReadFile("data/global/initData.ini");
	initFile.SetToSection("InitData");
	
	initFile.GetValue("title", _InitData.title);
	initFile.GetValue("fps", _InitData.fps);
	initFile.GetValue("width", _InitData.width);
	initFile.GetValue("height", _InitData.height);
	
	// start application
	PDApp::Init(_InitData);
}

int main (int argc, char* argv[]) {
	// initialize log buffers
	Log_Init();
	
	// start program
	initializePDApp();
	if (! _InitData.success) {
	  PDApp::Shutdown();
	  cerr << "Failed to initialize PDApp\n";
	  return 1;
	}
	
	// main loop and shutdown
	PDApp::Update();
	PDApp::Shutdown();
	
return 0;
}
