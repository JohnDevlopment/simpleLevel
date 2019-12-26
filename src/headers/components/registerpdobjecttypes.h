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
	PDObjectManager::AddObjectType(PDOT_Blackfader, "data/objecttypes/blackfader.ini");
	PDObjectManager::AddObjectType(PDOT_Menubutton, "data/objecttypes/menubutton.ini");
	PDObjectManager::AddObjectType(PDOT_Splash, "data/objecttypes/splash.ini");
} 

#endif /* REGISTERPDOBJECTTYPES_H_INCLUDED */
