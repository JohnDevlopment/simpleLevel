#include "objecthelper.h"
#include <sstream>
#include "pdmanager.h"

void LoadObjects(PDIniFile& ini) {
	std::string sVal1;
	
	// object list
	ini.GetValue("objects", sVal1);
	std::stringstream ssObjList(sVal1);
	std::string sCurObj;
	
	while (ssObjList >> sCurObj) {
	  int iCount;
	  ini.SetToSection(sCurObj);
	  ini.GetValue("count", iCount);
	  ini.GetValue("pos", sVal1);
	  std::stringstream ssPosList(sVal1);
	  PDObjectType type = StringToPDObjectType(sCurObj);
	  
	  for (int i = 0; i < iCount; ++i) {
	  	PDObject* newObject = PDObjectManager::CreateObject(type);
	  	// TODO change to: ssPosList >> newObject->m_pos;
	  	ssPosList >> newObject->m_pos[0];
	  	ssPosList >> newObject->m_pos[1];
	  }
	}
}
