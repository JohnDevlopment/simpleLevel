#ifndef PDMANAGER_H_INCLUDED
#define PDMANAGER_H_INCLUDED

#include "pdobject.h"
#include "pdcommandtype.h"

class PDComponentBuilder;
class PDCommandBuilder;
class PDCommand;

class PDObjectManager {
	static void Init();
	static void Shutdown();
	static void Update(float dt);
	static void Pause();
	static void Resume();
	
	PDObjectManager() {}
	~PDObjectManager() {}
	
	public:
	
	friend class PDGamemodeManager;
	friend class PDApp;
	
	// add object to the list of objects
	static void AddObject(PDObject* obj);
	
	// add object type
	static void AddObjectType(PDObjectType type, const char* file);
	
	// add component type and associated builder
	static void AddComponent(PDComponentType type, PDComponentBuilder* builder);
	
	// return a newly created object
	static PDObject* CreateObject(PDObjectType type);
	
	// adds a builder for commands
	static void AddCommand(PDCommandType type, PDCommandBuilder* builder);
	
	// returns a new command
	static PDCommand* NewCommand(PDCommandType type);
};

#endif /* PDMANAGER_H_INCLUDED */
