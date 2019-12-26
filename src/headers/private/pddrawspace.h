#ifndef PDDRAWSPACE_H_INCLUDED
#define PDDRAWSPACE_H_INCLUDED

#include <string>

enum PDDrawSpace {
	PDDS_Hud,
	PDDS_World,
	PDDS_Null
};

inline PDDrawSpace StringToDrawspace(const std::string& val) {
	if (val == "hud") return PDDS_Hud;
	else if (val == "world") return PDDS_World;
	return PDDS_Null;
}

#endif /* PDDRAWSPACE_H_INCLUDED */
