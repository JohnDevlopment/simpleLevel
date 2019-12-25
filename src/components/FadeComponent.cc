#include "FadeComponent.h"
#include "pdgamemodemanager.h"
#include "math.hpp"
#include <stdexcept>

#define str2func(str, ptr) \
	if (str == "solid") \
		ptr = solidPattern; \
	else \
		ptr = nullptr;

typedef void (*PatternFunction)(uint8_t*, int);

static void solidPattern(uint8_t* bytes, int count) {
	for (int i = 0; i < count; ++i) {
	  bytes[i] = 1;
	}
}

static CompFadeStatus strFadeIn(const std::string& val) {
	if (val == "IN") return COMP_FADEIN;
	else if (val == "OUT") return COMP_FADEOUT;

	return COMP_FADENULL;
}

int PDFadeComponent::s_msPerStep = 0;

PDFadeComponent::PDFadeComponent() : PDComponent(PDC_Fade),
                                     m_fade(COMP_FADENULL),
                                     m_fadeStep(0),
                                     m_fadeSteps(0),
                                     m_fadeDelay(0.0f),
                                     m_texture(nullptr),
                                     m_drawspace(PDDS_Null)
{
	if (! s_msPerStep) {
	  s_msPerStep = PDGamemodeManager::GetGameData().ticks;
	}
}

PDFadeComponent::~PDFadeComponent()
{
	
	video::UnregisterComponent(this);
	PDTexture::Free(m_texture);
	m_texture = nullptr;
	m_drawspace = PDDS_Null;
}

PDFadeComponent* PDFadeComponent::Clone() {
	PDFadeComponent* pClone = new PDFadeComponent;
	pClone->m_fade = m_fade;
	pClone->m_fadeSteps = m_fadeSteps;
	pClone->m_fadeDelay = m_fadeDelay;
	pClone->m_texture = m_texture->Clone();
	pClone->m_drawspace = m_drawspace;
	if (pClone->m_drawspace == PDDS_Hud)
	  video::RegisterHUDFader(pClone);

	return pClone;
}

void PDFadeComponent::Update(float dt) {
	if (m_fade == COMP_FADENULL) {
	  return;
	}
	
	if (m_fadeDelay) {
	  m_fadeDelay -= dt;
	  if (m_fadeDelay < 0.0f) m_fadeDelay = 0.0f;
	  return;
	}
	
	// fade still active
	if (m_fadeStep++ < m_fadeSteps) {
	  int iAlpha;
	  if (m_fade == COMP_FADEIN) {
	  	// fading in from black: decrease alpha value
	  	iAlpha = (255 * (m_fadeSteps - m_fadeStep)) / m_fadeSteps;
	  }
	  else {
	  	// fading out black: increase alpha value
	  	iAlpha = (255 * m_fadeStep) / m_fadeSteps;
	  }
	  
	  m_texture->SetModulation(iAlpha);
	  
	  // stop fade
	  if (m_fadeStep == m_fadeSteps) {
	  	m_fade = COMP_FADENULL;
	  }
	}
}

void PDFadeComponent::FromFile(PDIniFile& ini) {
//	std::string sPath = COMP_IMG_PATH;
	std::string sVal;
	PDGameData& gmd = PDGamemodeManager::GetGameData();
	
	ini.SetToSection("FadeComponent");
	
	ini.GetValue("type", sVal);
	Log_Assert(sVal == "file" || sVal == "pattern", "Incorrect fader type, must be a pattern or file");
	
	// create pattern texture
	if (sVal == "pattern") {
	  PatternArray pat = make_pattern(4, 4);
	  uint8_t uiRgb[3];
	  PatternFunction func = nullptr;
	  SDL_Texture* tex = nullptr;
	  Point<int> size;

	  // decide which pattern to make
	  ini.GetValue("pattern", sVal);
	  str2func(sVal, func);
	  Log_Assert(func != nullptr, "Function pointer is null");
	  func(pat.bytes, pat.count);
	  
	  // decide what color to make the pattern
	  ini.GetValue("color", sVal);
	  
{
	  std::stringstream ssColors(sVal);
	  ssColors >> uiRgb[0];
	  ssColors >> uiRgb[1];
	  ssColors >> uiRgb[2];
}
	  
	  // what's the size of the texture?
	  ini.GetValue("width", sVal);
	  if (sVal == "SCREEN") size[0] = gmd.width;
	  else size[0] = math::atoi(sVal);
	  
	  ini.GetValue("height", sVal);
	  if (sVal == "SCREEN") size[1] = gmd.height;
	  else size[1] = math::atoi(sVal);
	  
	  // draw space
	  ini.GetValue("drawspace", sVal);
	  Log_Assert(sVal == "hud", "Drawspace must be 'hud' for now");
	  m_drawspace = PDDS_Hud;
	  
	  // create texture
	  tex = video::NewColorScreen(uiRgb, size.x(), size.y(), SDL_PIXELFORMAT_RGBA32, pat);
	  Log_Assert(tex, SDL_GetError() );
	  m_texture = PDTexture::NewTexture(tex);
	}
	else {
	  // file
	}

	// get options for the component
	ini.GetValue("delay", m_fadeDelay);
	ini.GetValue("direction", sVal);

	// sVal must be either IN or OUT
	_Log_Assert( (sVal == "IN" || sVal == "OUT"),
	            "\"IN\" or \"OUT\" are the only valid options in ini file",
	            "PDFadeComponent::FromFile", __FILE__, __LINE__ );

	// number of fade steps
	int iLen;
	ini.GetValue("length", iLen);
	m_fade = strFadeIn(sVal);
	m_fadeStep = 0;
	
	if (m_fade == COMP_FADEIN) {
	  m_fadeSteps = (iLen + s_msPerStep - 1) / s_msPerStep;
	}
	else if (m_fade == COMP_FADEOUT) {
	  m_fadeSteps = iLen / s_msPerStep;
	  m_texture->SetModulation(0);
	}
}

void PDFadeComponent::Draw() {
	video::Draw(m_texture, m_parent->m_pos);
}
