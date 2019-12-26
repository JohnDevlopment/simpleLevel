/****************************************************************************************************/
/* Information
File: pdfactory.h
Author: John Russell
Date: 05/10/19
The purpose of this document is clear: define a template class that keeps a map of builders. A
builder knows how to build a desired type of something. */
/****************************************************************************************************/
#ifndef PDFACTORY_H_INCLUDED
#define PDFACTORY_H_INCLUDED

// engine headers
#include "stdinc.h"
#include "log.hpp"

// other headers
#include <unordered_map>

// forward declarations
template<typename EnumType, class BuilderType, class ReturnType>
class PDFactory {
	typedef std::unordered_map<size_t, BuilderType*> BuildMap;
	typedef typename BuildMap::iterator BuildItr;
	BuildMap m_builders;
	
	public:
	
	PDFactory() = default;
	~PDFactory() {
		ClearBuilders();
	}
	
	// add a builder
	void AddBuilder(EnumType type, BuilderType* builder);
	
	// remove builder of specified type
	void RemoveBuilder(EnumType type);
	
	// return an associated builder of the specified type
	ReturnType* Build(EnumType type);
	
	// clear all the builders
	void ClearBuilders();
};

/********************************************************************************/
/* Adds a builder to the list of builders for a specific type. Associates
the given builder with the enumerated type provided. */
/********************************************************************************/
template<typename EnumType, class BuilderType, class ReturnType>
void PDFactory<EnumType, BuilderType, ReturnType>::AddBuilder(EnumType type, BuilderType* builder)
{
	std::pair<BuildItr, bool> ret
		 = m_builders.insert(std::make_pair((size_t) type, builder));
	Log_Assert(ret.second == true, "Trying to add a builder that already exists");
}

/********************************************************************************/
/* Removes a builder associated with the given type. */
/********************************************************************************/
template<typename EnumType, class BuilderType, class ReturnType>
void PDFactory<EnumType, BuilderType, ReturnType>::RemoveBuilder(EnumType type)
{
	BuildItr found = m_builders.find((size_t) type);
	Log_Assert(found != m_builders.end(), "Trying to remove a non-existant builder");
	
	// delete builder and remove from map
	delete found->second;
	found->second = nullptr;
	m_builders.erase(found);
}

/*********************************************************************************/
/* Returns an a pointer to base class ReturnType using the base class BuilderType
as a method of building the former. Uses EnumType to decide which builder to use. */
/*********************************************************************************/
template<typename EnumType, class BuilderType, class ReturnType>
ReturnType* PDFactory<EnumType, BuilderType, ReturnType>::Build(EnumType type)
{
	BuildItr itr = m_builders.find((size_t) type);
	Log_Assert(itr != m_builders.end(), "Trying to use a builder that doesn't exist");
	return itr->second->Build();
}

/*********************************************************************************/
/* Cycles through the list of registered builders and deletes them from memory
before clearing the entire list. */
/*********************************************************************************/
template<typename EnumType, class BuilderType, class ReturnType>
void PDFactory<EnumType, BuilderType, ReturnType>::ClearBuilders()
{
	BuildItr beg = m_builders.begin();
	BuildItr end = m_builders.end();
	
	while (beg != end) {
	  delete beg->second;
	  beg->second = nullptr;
	  ++beg;
	}
	
	m_builders.clear();
}

#endif /* PDFACTORY_H_INCLUDED */
