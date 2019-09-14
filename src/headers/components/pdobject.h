#ifndef SPRITE_DEF_H_INCLUDED
#define SPRITE_DEF_H_INCLUDED

#include <pointvector>
#include <vector>
#include <algorithm>
#include "stdinc.h"
#include "sdl_incs.h"

#define GRAVITY_SPEED	8

#define START_SIZE	5

// enumerations for which bits are for which blocked status
enum {
	M_COLL_UP    = 0x08,
	M_COLL_DOWN  = 0x04,
	M_COLL_LEFT  = 0x02,
	M_COLL_RIGHT = 0x01
};

// type of object
enum PDObjectType {
	PDONull,
	PDOEnemy,
	PDOBullet,
	PDOTrigger,
	PDOExit,
	PDOPlayer
};

// type of component
enum PDComponentType {
	PDCNull,
	PDCGfx,
	PDCCollider,
	PDCActions
};

/* Component */
class PDObject;

class PDComponent {
	int m_id;
	PDComponentType m_type;
	
	
	static int s_compIdCounter;
	
	protected:
	
	PDObject* m_parent;
	
	public:
	
	bool m_isDead;
	
	PDComponent(PDComponentType type);
	virtual ~PDComponent();
	
	// set the object that this component belongs to
	void SetParent(PDObject* obj) {
		m_parent = obj;
	}
	
	// type of component
	PDComponentType GetType() const {return m_type}
};

/* Defines an object. It holds a variable number of pointers to
   components. Update() calls the Update() method for each active
   component. An object has its own position and speed. */
class PDObject {
	typedef std::vector<PDComponent*> CompVect;
	typedef CompVect::iterator CompVectItr;
	typedef CompVect::const_iterator CompVectConstItr
	
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

#endif /* SPRITE_DEF_H_INCLUDED */
