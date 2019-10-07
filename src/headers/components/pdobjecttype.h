/****************************************************************************************************/
/* Information
File: pdobjecttype.h
Author: headers.sh
This file was autogenerated by headers.sh. It contains an enumerated type called PDObjectType.
The script looks at files matching the pattern *.ini in /home/john/programming/C++/simpleLevel/exec/data/objecttypes and makes
enumerations of the form PDOT_* */
/****************************************************************************************************/
#ifndef PDOBJECTTYPE_H_INCLUDED
#define PDOBJECTTYPE_H_INCLUDED

#include <string>

enum PDObjectType {
	PDOT_Bullet,
	PDOT_Enemy,
	PDOT_Exit,
	PDOT_Player,
	PDOT_Trigger,
	PDOT_Invalid
};

inline PDObjectType StringToPDObjectType(const std::string& str) {
	if (str == "Bullet") return PDOT_Bullet;
	if (str == "Enemy") return PDOT_Enemy;
	if (str == "Exit") return PDOT_Exit;
	if (str == "Player") return PDOT_Player;
	if (str == "Trigger") return PDOT_Trigger;

return PDOT_Invalid;
}
#endif /* PDOBJECTTYPE_H_INCLUDED */
