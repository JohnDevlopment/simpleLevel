#ifndef SPRITE_DEF_H_INCLUDED
#define SPRITE_DEF_H_INCLUDED

#include <pointvector>
#include "stdinc.h"
#include "sdl_incs.h"

#define GRAVITY_SPEED	8

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
class PDComponent {
	int m_id;
	PDComponentType m_type;
	static int s_compIdCounter;
	
	protected:
	
	//
	
	public:
	
	PDComponent(PDComponentType type);
	virtual ~PDComponent();
	
	bool m_isDead;
};

/* Defines a basic object */
class PDObject {
	typedef std::vector<PDComponent*> CompVect;
	typedef CompVect::iterator CompVectItr;
	
	CompVect m_components;
	PDObjectType m_type;
	int m_id;
	
	static int s_objectIDCounter;
	
	public:
	
	Point<float> m_pos;
	PVector<float> m_vel;
	SDL_Rect m_clip;
	SDL_Rect m_blit;
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
	int GetID() const;
	PDObjectType GetType() const;
	template<typename T> void GetComponent(PDComponentType type, T*& comp);
	template<typename T> void GetAllComponents(PDComponentType type, std::vector<T*>& comps);
	PDObject* Clone() const;
};

#endif /* SPRITE_DEF_H_INCLUDED */
