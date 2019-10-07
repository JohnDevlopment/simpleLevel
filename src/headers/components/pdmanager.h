#ifndef PDMANAGER_H_INCLUDED
#define PDMANAGER_H_INCLUDED

// engine headers
#include "pdobject.h"
#include "debug.hpp"

class PDComponentBuilder;

class PDObjectManager {
	static void Init();
	static void Shutdown();
	static void Update(float dt);
	
	public:
	
	PDObjectManager() = default;
	~PDObjectManager() = default;
	
	// add object type
	static void AddObjectType(PDObjectType type, const char* file);
	
	// add component type and associated builder
	static void AddComponent(PDComponentType type, PDComponentBuilder* builder);
};

#endif /* PDMANAGER_H_INCLUDED */
