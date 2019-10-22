#include "pdobject.h"
#include "log.hpp"

int PDObject::s_objectIDCounter = 0;

PDObject::PDObject(PDObjectType type) : m_components(), m_type(type), m_id(++s_objectIDCounter), m_pos(0, 0),
                   m_vel(0, 0), m_isDead(false)
{
	m_components.reserve(START_SIZE);
}

PDObject::PDObject(PDObject&& src) {
	// move ctor
}

PDObject::~PDObject()
{
	RemoveAllComponents();
}

void PDObject::Update(float dt) {
	/* Call Update() in each component, unless that component is dead, in which case
	delete it from memory and remove it from the vector. */
	for (int i = 0; i < m_components.size(); ++i) {
	  if (m_components[i]->m_isDead) {
	  	delete m_components[i];
	  	m_components[i] = m_components[m_components.size()-1];
	  	m_components.pop_back();
	  }
	  else {
	  	m_components[i]->Update(dt);
	  }
	}
	
	// update position of the object
	m_pos[0] += m_vel.x() * dt;
	m_pos[1] += m_vel.y() * dt;
}

void PDObject::FromFile(PDIniFile& inifile) {
	inifile.SetToSection("");
	inifile.GetValue("posx", m_pos[0]);
	inifile.GetValue("posy", m_pos[1]);
	inifile.GetValue("velx", m_vel[0]);
	inifile.GetValue("vely", m_vel[1]);
}

void PDObject::AddComponent(PDComponent* comp) {
	// return if the specified component is already in the vector
	CompVectItr found = std::find(m_components.begin(), m_components.end(), comp);
	
	if (found != m_components.end())
	  return;
	
	// register component with the object
	comp->SetParent(this);
	m_components.push_back(comp);
}

void PDObject::RemoveComponent(PDComponent* comp) {
	// locate <comp> in the vector--hopefully it exists
	CompVectItr end = m_components.end();
	CompVectItr itr = std::find(m_components.begin(), end, comp);
	Log_Assert(itr != end, "component was not found");
	
	// swap itr's contents with end's and delete component
	std::iter_swap(itr, --end);
	m_components.pop_back();
	
	// free component from memory
	comp->SetParent(nullptr);
	delete comp;
}

void PDObject::RemoveAllComponents() {
	CompVectConstItr end = m_components.cend();
	for (CompVectItr itr = m_components.begin(); itr != end; ++itr) {
	  delete (*itr);
	}
	m_components.clear();
}

void PDObject::RemoveAllComponents(PDComponentType type) {
	// remove all components of a specific type
	for (size_t i = 0; i < m_components.size(); ++i) {
	  if (m_components[i]->GetType() == type) {
	  	delete m_components[i];
	  	m_components[i] = m_components[m_components.size()-1];
	  	m_components.pop_back();
	  	--i; // check the pointer that was swapped in
	  }
	}
}

PDObject* PDObject::Clone() const {
	PDObject* pClone = new PDObject(m_type);
	
	for (int i = 0; i < m_components.size(); ++i) {
	  PDComponent* pComp = m_components[i]->Clone();
	  pClone->AddComponent(pComp);
	}
	
return pClone;
}
