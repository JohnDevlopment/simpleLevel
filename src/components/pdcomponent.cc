#include "pdcomponent.h"

int PDComponent::s_compIdCounter = 0;

PDComponent::PDComponent(PDComponentType type)
	: m_id(++s_compIdCounter),
	m_type(type),
	m_parent(nullptr),
	m_isDead(false)
{
	// ctor
}

PDComponent::~PDComponent()
{
	// dtor
}

void PDComponent::FromFile(PDIniFile& inifile) {
	// empty for base class
}
