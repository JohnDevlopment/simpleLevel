#ifndef PDOBJECT_H_INCLUDED
#define PDOBJECT_H_INCLUDED

// engine headers
#include "stdinc.h"
#include "sdl_incs.h"
#include "pdcomponent.h" // pdcomponent.h => pdcomponenttype.h
#include "pdobjecttype.h"

// other headers
#include <vector>
#include <algorithm>
#include "pointvector"

#define GRAVITY_SPEED	8

#define START_SIZE	5

/* Defines an object. It holds a variable number of pointers to
   components. Update() calls the Update() method for each active
   component. An object has its own position and speed. */
class PDObject {
	// vector of components
	typedef std::vector<PDComponent*> CompVect;
	
	// iterator and constant iterator of the vector of components
	typedef CompVect::iterator CompVectItr;
	typedef CompVect::const_iterator CompVectConstItr;
	
	CompVect m_components;
	PDObjectType m_type;
	int m_id;
	
	static int s_objectIDCounter;
	
	public:
	
	Point<float> m_pos;
	PVector<float> m_vel;
	bool m_isDead;
	
	// constructors
	PDObject(PDObjectType type);
	PDObject(const PDObject& src) = delete;
	PDObject(PDObject&& src);
	
	~PDObject();
	
	// update the object
	void Update(float dt);
	
	// add/remove components
	void AddComponent(PDComponent* comp);
	void RemoveComponent(PDComponent* comp);
	void RemoveAllComponents();
	void RemoveAllComponents(PDComponentType type);
	
	// accessors
	int GetID() const {return m_id;}
	PDObjectType GetType() const {return m_type;}
	template<typename T> void GetComponent(PDComponentType type, T*& comp);
	template<typename T> void GetAllComponents(PDComponentType type, std::vector<T*>& comps);
	PDObject* Clone() const;
};

template<typename T> void PDObject::GetComponent(PDComponentType type, T*& comp)
{
	const size_t szComp = m_components.size();
	comp = nullptr;
	for (size_t i = 0; i < szComp; ++i) {
	  if (m_components[i]->GetType() == type) {
	  	comp = static_cast<T*>(m_components[i]);
	  	break;
	  }
	}
}

template<typename T> void PDObject::GetAllComponents(PDComponentType type, std::vector<T*>& comps)
{
	const size_t szComp = m_components.size();
	for (size_t i = 0; i < szComp; ++i) {
	  if (m_components[i]->GetType() == type) {
	  	comps.push_back(static_cast<T*>(m_components[i]));
	  }
	}
}

#endif /* PDOBJECT_H_INCLUDED */
