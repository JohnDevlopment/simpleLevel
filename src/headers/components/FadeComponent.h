#ifndef FADECOMPONENT_H_INCLUDED
#define FADECOMPONENT_H_INCLUDED

#include "stdinc.h"
#include "pdcomponent.h"
#include "pddrawspace.h"

class PDTexture;

enum CompFadeStatus {
	COMP_FADEIN,
	COMP_FADEOUT,
	COMP_FADENULL
};

class PDFadeComponent : public PDComponent {
	static int s_msPerStep;
	
	CompFadeStatus m_fade;
	int m_fadeStep;
	int m_fadeSteps;
	
	float m_fadeDelay;
	
	PDTexture* m_texture;
	PDDrawSpace m_drawspace;
	
	public:
	
	PDFadeComponent();
	virtual ~PDFadeComponent();
	
	virtual PDFadeComponent* Clone();
	virtual void Update(float dt);
	virtual void FromFile(PDIniFile& ini);
	
	void Draw();
};

#endif /* FADECOMPONENT_H_INCLUDED */
