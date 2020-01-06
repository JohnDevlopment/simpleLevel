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
#include "pdapp.h"
#include "pdinifile.h"

// external code headers
#include "timer_class.hpp"
#include <functional>
#include <string>

// private function declarations
static void initializePDApp() COLD;

using namespace std;

static PDInitData _InitData;

static bool checkOption(const int argc, int x) {
	if ( (x + 1) == argc ) return false;
	
	return true;
}

static int helpMessage(const char* arg, const char* argv0) {
	hash<string> hstr;
	
	switch ( hstr(string(arg)) ) {
	  // "usage"
	  case 0x71d9787b: {
	  	const char* prg = String_strrchr(argv0, '/') + 1;
	  	cout << "Usage:\n" << prg << " [configuration] -- use input configuration\n";
	  	cout << prg << " -h|-?|--help -- print this help message\n";
	  	cout << prg << " --hash [arg ...] -- print the hash of one or more strings\n";
	  	cout << "See the man page dewpsi(1) for more information.\n";
	  	break;
	  }
	  
	  default:
	  	cerr << "Invalid help option '" << arg << "'\n";
	  	return 1;
	  	break;
	}
	
	return 0;
}

static int hashParamters(const int argc, int x, char* argv[]) {
	string sArg;
	hash<string> hstr;
	
	if (x == argc) {
	  cerr << "No options passed to '--hash'\n";
	  return 1;
	}
	
	while (x < argc) {
	  sArg = argv[x++];
	  printf("Hash of '%s' : %x\n", sArg.c_str(), hstr(sArg));
	}
	
	return 0;
}

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
	// process commandline arguments
	for (int x = 1; x < argc; ++x) {
	  string sNextArg = argv[x];
	  hash<string> hstr;
	  
	  // only one argument = name of configuration for inputs
	  if (argc == 2) {
	  	_InitData.mapConf = argv[x];
	  	continue;
	  }
	  
	  switch ( hstr(sNextArg) ) {
	  	// -?, -h, --help (in that order)
	  	case 0x2cddcf7f:
	  	case 0x88d50b68:
	  	case 0xb691c1ad:
	  	  if ( ! checkOption(argc, x) ) {
	  	  	cerr << "Option '" << sNextArg << "' without an argument\n";
	  	  	cerr << "Pass '" << sNextArg << " usage' to get a help message\n";
	  	  	return 1;
	  	  }
	  	  return helpMessage(argv[x+1], argv[0]);
	  	  break;
	  	
	  	// --hash
	  	case 0x39279b6:
	  	  ++x;
	  	  return hashParamters(argc, x, argv);
	  	
	  	default:
	  	  cerr << "Unknown option '" << sNextArg << "'\n";
	  	  return 1;
	  	  break;
	  }
	}
	
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
