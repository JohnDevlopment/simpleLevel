#include "GfxComponent.h"
#include "pdapp.h"
#include "math.hpp"
#include "camera.h"

PDGfxComponent::~PDGfxComponent() {
	video::UnregisterComponent(this);
	PDTexture::Free(m_texture);
	m_texture = nullptr;
}

PDGfxComponent* PDGfxComponent::Clone() {
	PDGfxComponent* pClone = new PDGfxComponent;
	pClone->m_drawspace = m_drawspace;
	pClone->m_texture = m_texture->Clone();
	
	if (m_drawspace == PDDS_Hud)
	  video::RegisterHUDComponent(pClone);
	else
	  video::RegisterWorldComponent(pClone);
	
return pClone;
}

void PDGfxComponent::Update(float dt) {
}

void PDGfxComponent::FromFile(PDIniFile& inifile) {
	std::string sPath = COMP_IMG_PATH;
	std::string sVal;
	
	// get information from ini file
	inifile.SetToSection("GfxComponent");
	inifile.GetValue("texture", sVal);
	sPath += sVal;
	
	// request a reference to the specified texture
	Log_Assert( PDApp::s_manager.NewTexture( sPath.c_str(), nullptr), "failed to load texture");
	m_texture = PDApp::s_manager.GetTexture( sPath.c_str() );
	Log_Assert(m_texture, "m_texture is NULL");
	
	// register component to a drawspace
	inifile.GetValue("drawspace", sVal);
	m_drawspace = StringToDrawspace(sVal);
	
	// get the blitting width and height of the texture
	PVector<int> size;
	
	inifile.GetValue("width", sVal);
	if (sVal == "SCREEN") {
	  size[0] = WIDTH;
	}
	else if (sVal == "IMAGE") {
	  size[0] = m_texture->GetBlitSize().x();
	}
	else {
	  size[0] = math::atoi(sVal);
	}
	
	inifile.GetValue("height", sVal);
	if (sVal == "SCREEN") {
	  size[1] = HEIGHT;
	}
	else if (sVal == "IMAGE") {
	  size[1] = m_texture->GetBlitSize().y();
	}
	else {
	  size[1] = math::atoi(sVal);
	}
	
	m_texture->SetBlitSize(size);
}

void PDGfxComponent::Draw() {
	video::Draw(m_texture, m_parent->m_pos);
}
