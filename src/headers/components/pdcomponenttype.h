/****************************************************************************************************/
/* Information
File: pdcomponenttype.h
Author: headers.sh
This file was autogenerated by headers.sh. It contains an enumerated type called PDComponentType.
The script looks at files matching the pattern *Component.h in /home/john/programming/C++/simpleLevel/src/headers/components and makes
enumerations of the form PDC_* */
/****************************************************************************************************/
#ifndef PDCOMPONENTTYPE_H_INCLUDED
#define PDCOMPONENTTYPE_H_INCLUDED

#include <string>

enum PDComponentType {
	PDC_Gfx,
	PDC_Invalid
};

inline PDComponentType StringToPDComponentType(const std::string& str) {
	if (str == "Gfx") return PDC_Gfx;

return PDC_Invalid;
}
#endif /* PDCOMPONENTTYPE_H_INCLUDED */
