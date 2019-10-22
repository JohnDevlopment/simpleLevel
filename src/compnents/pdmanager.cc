// engine headers
#include "pdmanager.h"
#include "registerpdobjecttypes.h"
#include "registerpdcomponenttypes.h"
#include "pdfactory.h"
#include "pdcomponentbuilder.h"

// other headers
#include <unordered_map>
#include <algorithm>

namespace {
	typedef std::vector<PDObject*> ObjectVec;
	typedef ObjectVec::iterator VecItr;
	typedef std::unordered_map<size_t, PDObject*> PrototypeMap;
	typedef PrototypeMap::iterator MapItr;
	
	const int _START_SIZE = 50;
	
	static PDFactory<PDComponentType, PDComponentBuilder, PDComponent> _CompFactory;
	static ObjectVec _Objects;
	static PrototypeMap _Prototypes;
}

/**************************************************/
/* Reserve a certain amount of space for the
object list. Also register components and
object prototypes with the manager. */
/**************************************************/
void PDObjectManager::Init() {
	CHECK_FUNC(1);
	_Objects.reserve(_START_SIZE);
	RegisterComponents();
	RegisterObjectTypes();
}

/**************************************************/
/* Destroy object prototypes. */
/**************************************************/
void PDObjectManager::Shutdown() {
	CHECK_FUNC(1);
	
	MapItr beg = _Prototypes.begin();
	MapItr end = _Prototypes.end();
	
	while (beg != end) {
	  delete beg->second;
	  beg->second = nullptr;
	  ++beg;
	}
}

/**************************************************/
/* Update each object using the given value
for time-based operations that may take place. */
/**************************************************/
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

/**************************************************/
/* Add the given object to the manager. */
/**************************************************/
void PDObjectManager::AddObject(PDObject* obj) {
	VecItr found = std::find(_Objects.begin(), _Objects.end(), obj);
	Log_Assert(found == _Objects.end(), "Object already added to the manager");
	
	_Objects.push_back(obj);
}

/**************************************************/
/* Add an object prototype to the prototype
list. */
/**************************************************/
void PDObjectManager::AddObjectType(PDObjectType type, const char* file) {
	std::string sComponents, sName;
	PDObject* newobject = nullptr;
	
	MapItr found = _Prototypes.find((size_t) type);
	Log_Assert(found == _Prototypes.end(), "trying to add a prototype that's already added");
	
	// read ini file
	PDIniFile inifile;
	inifile.ReadFile(file);
	
	// new object, read global traits from ini file
	newobject = new PDObject(type);
	
	inifile.GetValue("components", sComponents);
	inifile.GetValue("posx", newobject->m_pos[0]);
	inifile.GetValue("posy", newobject->m_pos[1]);
	inifile.GetValue("velx", newobject->m_vel[0]);
	inifile.GetValue("vely", newobject->m_vel[1]);
	
	// for each component name, turn it into an actual component
	std::stringstream ss (sComponents);
	while (ss >> sName) {
	  PDComponent* comp = _CompFactory.Build( StringToPDComponentType(sName) );
	  comp->FromFile(inifile);
	  newobject->AddComponent(comp);
	}
	
	// add prototype to prototype map
	_Prototypes.insert( std::make_pair(type, newobject) );
}

/**************************************************/
/* Add a builder for a component of the given
type. */
/**************************************************/
void PDObjectManager::AddComponent(PDComponentType type, PDComponentBuilder* builder) {
	_CompFactory.AddBuilder(type, builder);
}

/**************************************************/
/* Create an object based off of a prototype
of the given type. */
/**************************************************/
PDObject* PDObjectManager::CreateObject(PDObjectType type) {
	MapItr found = _Prototypes.find(type);
	Log_Assert(found != _Prototypes.end(), "Cannot find object prototype");
	
	PDObject* retval = found->second->Clone();
	_Objects.push_back(retval);
	
return retval;
}
