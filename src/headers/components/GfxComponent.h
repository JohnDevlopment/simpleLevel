#ifndef GFXCOMPONENT_H_INCLUDED
#define GFXCOMPONENT_H_INCLUDED

#include "stdinc.h"
#include "pdcomponent.h"
#include "textures.h"
#include "pddrawspace.h"

class PDGfxComponent : public PDComponent {
	PDDrawSpace m_drawspace;
	PDTexture* m_texture;
	
public:
	
	PDGfxComponent() : PDComponent(PDC_Gfx), m_drawspace(PDDS_Null), m_texture(nullptr) {}
	~PDGfxComponent();
	
	// clone component
	virtual PDGfxComponent* Clone();
	
	// update component
	virtual void Update(float dt);
	
	// read ini file to get traits for component
	virtual void FromFile(PDIniFile& inifile);
	
	// draws the underlying texture
	void Draw();
};

#endif /* GFXCOMPONENT_H_INCLUDED */
