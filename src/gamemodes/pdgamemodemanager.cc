#include "pdgamemodemanager.h"
#include "pdmanager.h"
#include <stack>
#include "registerpdgamemodetypes.h"
#include "math.hpp"

namespace {
	struct PauseInfo {
		PauseInfo(PDGamemode* ptr, PDGamemodeType t)
			: pGamemode(ptr), type(t) {}
		PDGamemode* pGamemode;
		PDGamemodeType type;
	};
	static PDFactory<PDGamemodeType, PDGamemodeBuilder, PDGamemode> _Factory;
	static std::stack<PauseInfo> _PauseStack;
	static PDGamemodeType _CurrMode;
	static PDGamemodeType _NextMode;
	static PDGamemode* _Gamemode;
	static Timer _Timer;
	static bool _IsChanging;
	static bool _IsQuitting;
	static bool _IsPausing;
	static bool _IsResuming;
	static bool _IsRestarting;
	static bool _DrawPaused;
}

PDGameData* PDGamemodeManager::s_gameData = nullptr;

// private
/**************************************************/
/* InitGamemode
   Called at the start of PDGamemodeManager::-
   Update. Restarts the gamemode, resumes it if it
   was paused, or starts a new gamemode if the old
   one was shutdown.
*/
/**************************************************/
void PDGamemodeManager::InitGamemode() {
	if (_IsRestarting) {
	  _Gamemode->Init();
	  _IsRestarting = false;
	}
	else if (_IsResuming) {
	  // resume normal operations
	  PDObjectManager::Resume();
//	  PDPhy::Resume(); // TODO implement class and method
	  video::Resume();
	  
	  // change flags
	  _IsResuming = false;
	  _IsChanging = false;
	  
	  // pause info
	  PauseInfo pi = _PauseStack.top();
	  _PauseStack.pop();
	  _CurrMode = _NextMode = pi.type;
	  _Gamemode = pi.pGamemode;
	}
	else if (_IsChanging) {
	  _Gamemode = _Factory.Build(_NextMode);
	  _Gamemode->Init();
	  _IsChanging = false;
	}
}

/**************************************************/
/* ChangeGamemode
   If the gamemode is paused, saves the current
   gamemode to a stack and pauses the other
   systems. If resuming from a pause, kill the
   active gamemode so InitGamemode can retrieve
   the paused one.
   If not restarting the gamemode, that means we're
   switching to a new one. So shutdown the current
   gamemode and shutdown any that are on the pause
   stack.
   If the gamemode is restarting, shut it down but
   leave it be otherwise.
*/
/**************************************************/
void PDGamemodeManager::ChangeGamemode() {
	if (_IsPausing) {
	  // pause engines
	  PDObjectManager::Pause();
//	  PDPhy::Pause(); // TODO implement this
	  video::Pause();
	  
	  // add to pause stack
	  PauseInfo pi(_Gamemode, _CurrMode);
	  _PauseStack.push(pi);
	  _IsPausing = false;
	}
	else if (_IsResuming) {
	  _Gamemode->Shutdown();
	  delete _Gamemode;
	  _Gamemode = nullptr;
	}
	else if (! _IsRestarting) {
	  _Gamemode->Shutdown();
	  delete _Gamemode;
	  _Gamemode = nullptr;
	  
	  if (! _PauseStack.empty()) {
	  	PDObjectManager::Resume();
//	  	PDPhy::Resume(); // TODO implement function
	  	video::Resume();
	  	
	  	// shutdown the gamemode and delete it
	  	PauseInfo pi = _PauseStack.top();
	  	pi.pGamemode->Shutdown();
	  	delete pi.pGamemode;
	  	_PauseStack.pop();
	  }
	}
	else if (_IsRestarting) {
	  _Gamemode->Shutdown();
	}
	
	_CurrMode = _NextMode;
}

void PDGamemodeManager::Init(const PDGameData& srcData) {
	s_gameData = ::new PDGameData(srcData);
	RegisterGamemodes();
	
	_IsChanging   = true;
	_IsQuitting   = false;
	_IsPausing    = false;
	_IsResuming   = false;
	_IsRestarting = false;
	_DrawPaused   = false;
	
	_CurrMode = _NextMode = PDGM_Invalid;
	_Gamemode = nullptr;
}

#include "sdl_incs.h"

void PDGamemodeManager::Update() {
	int iTicks = 0;
	float fElapsed;
	
	InitGamemode();
	
	while (! _IsChanging && ! _IsQuitting && ! _IsRestarting) {
	  _Timer.start();
	  
	  // update the input, 
//	  PDInput::Reset(); // TODO implement class and method
	  // TODO process messages
	  PDObjectManager::Update(fElapsed);
//	  PDPhy::Update(); // TODO implement PDPhy::Update
	  _Gamemode->Update(fElapsed);
	  video::Update();
	  
	  // wait for the rest of the frame
	  _Timer.pause();
	  iTicks = _Timer.get();
	  fElapsed = math::clamp( (float) iTicks, 0.01f, 0.33f);
	  iTicks -= s_gameData->ticks;
	  if (iTicks < 0) {
	  	SDL_Delay(-iTicks);
	  }
	}
	
	ChangeGamemode();
}

void PDGamemodeManager::Shutdown() {
	::delete s_gameData;
	s_gameData = nullptr;
}

// public
/**************************************************/
/*
Returns true if the gamemode manager is quitting.
*/
/**************************************************/
bool PDGamemodeManager::IsQuitting() {
	return _IsQuitting;
}

/**************************************************/
/*
Returns true if the manager is restarting.
*/
/**************************************************/
bool PDGamemodeManager::IsRestarting() {
	return _IsRestarting;
}

/**************************************************/
/*
Return shared game data.
*/
/**************************************************/
PDGameData& PDGamemodeManager::GetGameData() {
	return *s_gameData;
}

/**************************************************/
/*
Add a gamemode builder.
*/
/**************************************************/
void PDGamemodeManager::AddGamemode(PDGamemodeType type, PDGamemodeBuilder* builder) {
	_Factory.AddBuilder(type, builder);
}

/**************************************************/
/*
Remove a gamemode builder.
*/
/**************************************************/
void PDGamemodeManager::RemoveGamemode(PDGamemodeType type) {
	_Factory.RemoveBuilder(type);
}

/**************************************************/
/*
Remove all gamemode builders.
*/
/**************************************************/
void PDGamemodeManager::ClearGamemodes() {
	_Factory.ClearBuilders();
}

/**************************************************/
/*
Sets the starting gamemode. Should only be called
once at the beginning of the program.
*/
/**************************************************/
void PDGamemodeManager::SetFirstGamemode(PDGamemodeType type) {
	_NextMode = _CurrMode = type;
}

/**************************************************/
/*
Switches to a different gamemode.
*/
/**************************************************/
void PDGamemodeManager::SetNextGamemode(PDGamemodeType type) {
	_NextMode = type;
	_IsChanging = true;
}

/**************************************************/
/*
Pauses the current gamemode and changes to a
different one.
*/
/**************************************************/
void PDGamemodeManager::PauseAndChangeGamemode(PDGamemodeType next, bool drawPaused /*= false*/) {
	_IsPausing = true;
	_IsChanging = true;
	_NextMode = next;
	_DrawPaused = drawPaused;
}

/**************************************************/
/*
Resumes the previous gamemode.
*/
/**************************************************/
void PDGamemodeManager::Resume() {
	Log_Assert( ! _PauseStack.empty(), "No gamemode was paused");
	_IsChanging = true;
	_IsResuming = true;
}

/**************************************************/
/*
Quits the gamemode manager.
*/
/**************************************************/
void PDGamemodeManager::Quit() {
	_IsQuitting = true;
}
