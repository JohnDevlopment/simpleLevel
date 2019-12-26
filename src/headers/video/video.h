#ifndef VIDEO_H_INCLUDED
#define VIDEO_H_INCLUDED

// engine headers
#include "stdinc.h"
#include "sdl_incs.h"
#include "global_attributes.hpp"
#include "debug.hpp"

// other headers
#include "color.h"
#include "pointvector"

class PDGfxComponent;
class PDFadeComponent;
class PDTexture;

namespace video {
	// set background color
	void SetBgColor(const JColor& color);
	
	// save renderer
	void SetRenderer(SDL_Renderer* ren);
	
	// retrieve renderer
	SDL_Renderer* GetRenderer() PURE;
	
	// init video engine; can only be called once
	void Init(int width, int height, SDL_Renderer* ren);
	
	// update function
	void Update();
	
	// shut down video engine; can only be called once
	void Shutdown();
	
	// register components on the world level
	void RegisterWorldComponent(PDGfxComponent* pComp);
	
	// register components on the HUD level
	void RegisterHUDComponent(PDGfxComponent* pComp);
	
	// register fader component on the HUD level
	void RegisterHUDFader(PDFadeComponent* pComp);
	
	// remove world and/or HUD component
	void UnregisterComponent(PDGfxComponent* pComp);
	void UnregisterComponent(PDFadeComponent* pComp);
	
	// clear all components
	void ClearComponents();
	
	// pause the graphics engine
	void Pause(bool drawPaused = false);
	
	// resumes the graphics engine
	void Resume();
	
	// draw the given texture with an optional position
	void Draw(PDTexture* texture, const Point<int>& pos);
}

#endif /* VIDEO_H_INCLUDED */
