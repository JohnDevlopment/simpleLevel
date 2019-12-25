#include "video.h"
#include "camera.h"
#include <memory>
#include <stack>
#include "GfxComponent.h"
#include "FadeComponent.h"
#include "PDTexture.h"

/**************************************/
/*
Types used by the graphics engine.
*/
/**************************************/
struct PDGfxState {
	float cameraX;
	float cmaeraY;
	uint8_t bgcolor[4];
	int hudStart;
	int worldStart;
};

//static int _private_width = 0;

//static int _private_height = 0;

static SDL_Renderer* _private_renderer = nullptr;

static PDGfxState _private_state = {0};

static std::stack<PDGfxState> _private_pause_stack;

// world and hud components
typedef std::vector<PDGfxComponent*> Components;
Components _private_world_components;
Components _private_hud_components;
PDFadeComponent* _private_fade_component = nullptr;

namespace video {

/**************************************/
/*
Sets the backdrop color.
*/
/**************************************/
void SetBgColor(const JColor& color) {
	SDL_SetRenderDrawColor(_private_renderer, color.red, color.green, color.blue, color.alpha);
	_private_state.bgcolor[0] = color.red;
	_private_state.bgcolor[1] = color.blue;
	_private_state.bgcolor[2] = color.green;
	_private_state.bgcolor[3] = color.alpha;
}

/**************************************/
/*
Set a renderer.
*/
/**************************************/
void SetRenderer(SDL_Renderer* ren) {
	_private_renderer = ren;
}

/**************************************/
/*
Retrieve a renderer.
*/
/**************************************/
SDL_Renderer* GetRenderer() {
	return _private_renderer;
}

/**************************************/
/*
Initialize the graphics engine and
prepare for future operations.
*/
/**************************************/
void Init(int width, int height, SDL_Renderer* ren) {
	CHECK_FUNC(1);
	video::SetWorld(width, height);
	video::SetHUD(width, height);
}

/**************************************/
/*
Draws everything in the HUD and world
list.
*/
/**************************************/
void Update() {
	size_t szComps;
	
	// start the drawing scene
	SDL_RenderClear(_private_renderer);
	
	szComps = _private_world_components.size();
	video::SetToWorldPerspective();
	
	// draw world components
	for (size_t i = _private_state.worldStart; i < szComps; ++i)
	{
	  _private_world_components[i]->Draw();
	}
	
	szComps = _private_hud_components.size();
	video::SetToHUDPerspective();
	
	// draw HUD components
	for (size_t i = _private_state.hudStart; i < szComps; ++i)
	{
	  _private_hud_components[i]->Draw();
	}
	
	// draw fader component
	if (_private_fade_component)
	  _private_fade_component->Draw();
	
	// end the drawing scene
	SDL_RenderPresent(_private_renderer);
}

/**************************************/
/*
Shut down the graphics engine.
*/
/**************************************/
void Shutdown() {
	CHECK_FUNC(1);
}

/**************************************/
/*
Registers components that render on the
world drawspace.
*/
/**************************************/
void RegisterWorldComponent(PDGfxComponent* pComp) {
	_private_world_components.push_back(pComp);
}

/**************************************/
/*
Registers components that render on the
HUD drawspace.
*/
/**************************************/
void RegisterHUDComponent(PDGfxComponent* pComp) {
	_private_hud_components.push_back(pComp);
}

/**************************************************/
/* RegisterHUDFader
   Registers a fade component to the HUD drawspace.
   
   @param pComp	A fade component to register
*/
/**************************************************/
void RegisterHUDFader(PDFadeComponent* pComp) {
	Log_Assert(! _private_fade_component, "A fader component is already registered");
	_private_fade_component = pComp;
}

/**************************************************/
/* UnregisterComponent
   Removes the given component from the HUD and
   world lists.
   
   @param pComp	The component to remove
*/
/**************************************************/
void UnregisterComponent(PDGfxComponent* pComp) {
	size_t szStop = _private_world_components.size();
	
	for (size_t i = _private_state.worldStart; i < szStop; ++i) {
	  if (pComp == _private_world_components[i]) {
	  	_private_world_components[i] = _private_world_components[szStop - 1];
	  	_private_world_components.pop_back();
	  }
	}
	
	szStop = _private_hud_components.size();
	
	for (size_t i = _private_state.hudStart; i < szStop; ++i) {
	  if (pComp == _private_hud_components[i]) {
	  	_private_hud_components[i] = _private_hud_components[szStop - 1];
	  	_private_hud_components.pop_back();
	  }
	}
}

/**************************************************/
/* UnregisterComponent
   Removes the given fader component from
   the queue.
   
   @param pComp	The component to remove
*/
/**************************************************/
void UnregisterComponent(PDFadeComponent* pComp) {
	if (pComp == _private_fade_component)
	  _private_fade_component = nullptr;
}

/**************************************/
/*
Unregisters every component in the
video engine.
*/
/**************************************/
void ClearComponents() {
//	size_t szComps = _private_world_components.size();
//	
//	// delete world components
//	for (size_t i = 0; i < szComps; ++i) {
//	  delete _private_world_components[i];
//	}
	_private_world_components.clear();
	
//	szComps = _private_hud_components.size();
//	
//	// delete HUD components
//	for (size_t i = 0; i < szComps; ++i) {
//	  delete _private_hud_components[i];
//	}
	_private_hud_components.clear();
	
	_private_fade_component = nullptr;
}

/**************************************************/
/*
Pauses the graphics engine and saves the current
state of the engine.
*/
/**************************************************/
void Pause(bool drawPaused /*= false*/) {
	_private_pause_stack.push(_private_state);
	if (! drawPaused) {
	  _private_state.hudStart = _private_hud_components.size();
	  _private_state.worldStart = _private_world_components.size();
	}
}

/**************************************************/
/*
Resumes the graphics engine and restores the
state of the engine.
*/
/**************************************************/
void Resume() {
	JColor newBgColor;
	
	_private_state = _private_pause_stack.top();
	_private_pause_stack.pop();
	
	newBgColor.red = _private_state.bgcolor[0];
	newBgColor.green = _private_state.bgcolor[1];
	newBgColor.blue = _private_state.bgcolor[2];
	newBgColor.alpha = _private_state.bgcolor[3];
	
	SetBgColor(newBgColor);
}

/**************************************************/
/* Draw
   Draws the texture at the given position on
   the screen relative to the active camera.
   
   @param texture	The texture to draw
   @param pos		A point where to draw the
   			texture
*/
/**************************************************/
void Draw(PDTexture* texture, const Point<int>& pos) {
	if (! texture || texture->GetRefCount() <= 0) return;
	
	Point<int> size = texture->GetBlitSize();
	const SDL_Rect* const pCam = GetCamera();
	SDL_Rect blit = {pos.x() - pCam->x, pos.y() - pCam->y, size.x(), size.y()};
	SDL_Rect clip = texture->GetClipRect();
	
	SDL_RenderCopy(_private_renderer, texture->GetTexture(), &clip, &blit);
}

} // end namespace video
