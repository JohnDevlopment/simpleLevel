#ifndef PDCOMPONENT_H_INCLUDED
#define PDCOMPONENT_H_INCLUDED

#include "pdinifile.h"
#include "pdcomponenttype.h"

/* Component */
class PDObject;
class PDIniFile;

class PDComponent {
	int m_id;
	PDComponentType m_type;
	
	static int s_compIdCounter;
	
	protected:
	
	PDObject* m_parent;
	
	public:
	
	bool m_isDead;
	
	// construct and destroy the object
	PDComponent(PDComponentType type);
	virtual ~PDComponent();
	
	// clone component
	virtual PDComponent* Clone() = 0;
	
	// update internal data
	virtual void Update(float dt) = 0;
	
	// read ini file to set properties
	virtual void FromFile(PDIniFile& inifile);
	
	// set the object that this component belongs to
	void SetParent(PDObject* obj) {m_parent = obj;}
	
	// return the component's ID
	int GetID() const {return m_id;}
	
	// type of component
	PDComponentType GetType() const {return m_type;}
};

#endif /* PDCOMPONENT_H_INCLUDED */
