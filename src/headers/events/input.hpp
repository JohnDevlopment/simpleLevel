#ifndef INPUT_HPP_INCLUDED
#define INPUT_HPP_INCLUDED

#include "stdinc.h"
#include "sdl_incs.h"

class PDIniFile;

class PDInput {
	static void Init();
	static void Shutdown();
	static void Reset(float dt);
	static void Update(float dt);
	static void FromFile(PDIniFile& ini);
	
	public:
	
	friend class PDApp;
	friend class PDGamemodeManager;
	
	static void SetPressed(int key, int status);
	static bool IsPressed(int key);
	static bool IsTriggered(int key);
};

#endif /* INPUT_HPP_INCLUDED */
