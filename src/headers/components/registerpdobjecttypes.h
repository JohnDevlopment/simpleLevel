/****************************************************************************************************/
/* Information
File: registerpdobjecttypes.h
Author: headers.sh
This file was generated from a script that checks ini files in the data/objecttypes directory
and registers different types with the object manager */
/****************************************************************************************************/
#ifndef REGISTERPDOBJECTTYPES_H_INCLUDED
#define REGISTERPDOBJECTTYPES_H_INCLUDED

#include "pdmanager.h" // pdmanager.h => pdobject.h => pdobjecttype.h

inline void RegisterObjectTypes() {
	PDObjectManager::AddObjectType(PDOT_Bullet, "data/objecttypes/bullet.ini");
	PDObjectManager::AddObjectType(PDOT_Enemy, "data/objecttypes/enemy.ini");
	PDObjectManager::AddObjectType(PDOT_Exit, "data/objecttypes/exit.ini");
	PDObjectManager::AddObjectType(PDOT_Player, "data/objecttypes/player.ini");
	PDObjectManager::AddObjectType(PDOT_Trigger, "data/objecttypes/trigger.ini");
} 

#endif /* REGISTERPDOBJECTTYPES_H_INCLUDED */
