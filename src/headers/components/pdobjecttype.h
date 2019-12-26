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
	PDOT_Blackfader,
	PDOT_Menubutton,
	PDOT_Splash,
	PDOT_Invalid
};

inline PDObjectType StringToPDObjectType(const std::string& str) {
if (str == "Blackfader") return PDOT_Blackfader;
if (str == "Menubutton") return PDOT_Menubutton;
if (str == "Splash") return PDOT_Splash;

return PDOT_Invalid;
}
#endif /* PDOBJECTTYPE_H_INCLUDED */
