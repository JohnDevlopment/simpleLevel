/***************************************************************************/
/*
file: pdgamemodebuilder.h
author: John Russell
date: 10/24/19
Defines a factory builder for type PDGamemode, called PDGamemodeBuilder.
Derived template class PDGamemodeTBuilder specializes gamemode building.
*/
/***************************************************************************/
#ifndef PDGAMEMODEBUILDER_H_INCLUDED
#define PDGAMEMODEBUILDER_H_INCLUDED

class PDGamemode;

/****************************/
/*
Interface to build gamemodes
(base type = PDGamemode)
*/
/****************************/
class PDGamemodeBuilder {
public:
	PDGamemodeBuilder() {}
	virtual ~PDGamemodeBuilder() {}
	
	// allocate a PDGamemode
	virtual PDGamemode* Build() = 0;
};

/****************************/
/*
Builds a new gamemode of type
T.
*/
/****************************/
template<typename T>
class PDGamemodeTBuilder : public PDGamemodeBuilder {
public:
	virtual T* Build();
};

template<typename T>
T* PDGamemodeTBuilder<T>::Build() {
	return new T();
}

#endif /* PDGAMEMODEBUILDER_H_INCLUDED */
