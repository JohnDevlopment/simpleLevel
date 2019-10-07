// engine headers
#include "registerpdobjecttypes.h" // pdmanager.h
#include "pdfactory.h"
#include "pdcomponentbuilder.h"

// other headers
#include <unordered_map>

#define _START_SIZE 50

namespace {
	typedef std::vector<PDObject*> ObjectVec;
	typedef ObjectVec::iterator VecItr;
	typedef std::unordered_map<size_t, PDObject*> PrototypeMap;
	typedef PrototypeMap::iterator MapItr;
	
	static PDFactory<PDComponentType, PDComponentBuilder, PDComponent> _CompFactory;
	static ObjectVec _Objects;
	static PrototypeMap _Prototypes;
}

// reserve space for objects and register new component and object types
void PDObjectManager::Init() {
	CHECK_FUNC(1);
	_Objects.reserve(_START_SIZE);
	//RegisterComponents();
	RegisterObjectTypes();
}

void PDObjectManager::Shutdown() {
	CHECK_FUNC(1);
	// destroy all objects
	
	MapItr beg = _Prototypes.begin();
	MapItr end = _Prototypes.end();
	
	while (beg != end) {
	  delete beg->second;
	  beg->second = nullptr;
	  ++beg;
	}
}

void PDObjectManager::Update(float dt) {
	for (size_t i = 0; i < _Objects.size(); ++i) {
	  // delete the "dead" object
	  if (_Objects[i]->m_isDead) {
	  	delete _Objects[i];
	  	_Objects[i] = _Objects[_Objects.size()-1];
	  	_Objects.pop_back();
	  	--i;
	  }
	  // update the object internals
	  else {
	  	_Objects[i]->Update(dt);
	  }
	}
}

void PDObjectManager::AddObjectType(PDObjectType type, const char* file) {
	MapItr found = _Prototypes.find((size_t) type);
	Log_Assert(found == _Prototypes.end(), "trying to add a prototype that's already added");
	
	// read ini file
	PDIniFile inifile;
	std::string sComponents;
	inifile.ReadFile(file);
	
	// new object, read global traits from ini file
	PDObject* newobject = new PDObject(type);
	inifile.GetValue("components", sComponents);
	inifile.GetValue("posx", newobject->m_pos[0]);
	inifile.GetValue("posy", newobject->m_pos[1]);
	inifile.GetValue("velx", newobject->m_vel[0]);
	inifile.GetValue("vely", newobject->m_vel[1]);
	
	// for each component name, turn it into an actual component
	std::stringstream ss (sComponents);
	std::string sName;
	while (ss >> sName) {
	  PDComponent* comp = _CompFactory.Build(StringToPDComponentType(sName));
	  comp->FromFile(inifile);
	  newobject->AddComponent(comp);
	}
	
	// add prototype to prototype map
	_Prototypes.insert(std::make_pair(type, newobject));
}

void PDObjectManager::AddComponent(PDComponentType type, PDComponentBuilder* builder) {
	_CompFactory.AddBuilder(type, builder);
}

#undef _START_SIZE
