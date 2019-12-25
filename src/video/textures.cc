// engine headers
#include "textures.h"
#include "PDTexture.h"
#include "log.hpp"
#include "_RefTexture.h"
#include "_RotData.h"
#include "game.hpp"
#include "endian.hpp"
#include "memory.hpp"

/**************************************************/
/* PDTexture
   Accepts SDL_Texture and SDL_Renderer and renders
   the texture differently according to the type
   configuration.
*/
/**************************************************/
PDTexture::PDTexture(_RefTexture* ref, SDL_Renderer* ren)
 : m_texture(ref),
   m_renderer(ren),
   m_rotation(),
   m_clip({0}),
   m_size(),
   m_blend(SDL_BLENDMODE_NONE),
   m_mod(0),
   m_getMod(true),
   m_getBlend(true)
{
	int iCode = SDL_QueryTexture(ref->texture, nullptr, nullptr, &m_clip.w, &m_clip.h);
	Log_Assert(!iCode, "failed to query texture");
	ResetBlitSize();
}

PDTexture::~PDTexture()
{
	m_texture = nullptr;
}

/**************************************************/
/* SetBlitSize
   Change the blitting size of the texture.
*/
/**************************************************/
void PDTexture::SetBlitSize(const Point<int>& size) {
	m_size = size;
}

/**************************************************/
/* ResetBlitRect
   Resets the blit size to the original size of
   the texture.
*/
/**************************************************/
void PDTexture::ResetBlitSize() {
	int code = SDL_QueryTexture(m_texture->texture, nullptr, nullptr, &m_size[0], &m_size[1]);
	Log_Assert(! code, SDL_GetError() );
}

/**************************************************/
/* SetRotation
   Sets the rotation angle of the texture.
   
   @param angle	The rotation angle in degrees,
   		in clockwise direction
*/
/**************************************************/
void PDTexture::SetRotation(double angle) {
	m_rotation.use = true;
	m_rotation.angle = angle;
}

/**************************************************/
/* Sets the pivot of the rotation.
   
   @param pivot	A pointer to the X & Y point of
   		the rotation pivot. If NULL, the
   		center of the texture is used
   		instead.
*/
/**************************************************/
void PDTexture::SetPivot(SDL_Point* pivot) {
	SDL_Point temp = {m_size.x()/2, m_size.y()/2};
	
	m_rotation.use = true;
	m_rotation.pivot = (pivot) ? *pivot : temp;
}

/**************************************************/
/* DisableRotation
   Disables rotation of the texture.
*/
/**************************************************/
void PDTexture::DisableRotation() {
	m_rotation.use = false;
	m_rotation.angle = 0.0;
}

/**************************************************/
/* SetBlendMode
   Sets the texture blend mode.
   
   @param mode	A value of SDL_BlendMode that
   		indicates how to blend the texture
   @return	0 if the process works or -1 upon
  		failure
*/
/**************************************************/
int PDTexture::SetBlendMode(SDL_BlendMode mode) {
	int iCode = SDL_SetTextureBlendMode(m_texture->texture, mode);
	if (iCode < 0)
	  std::cout << "Error from SDL_SetTextureBlendMode: " << SDL_GetError() << '\n';
	else {
	  m_getMod = true;
	  m_getBlend = true;
	}
	
	return iCode;
}

/**************************************************/
/* SetModulation
   Sets the modulation of the current blend mode.
   
   @param value	Context-dependent value that's
   		applied to the current blend mode
*/
/**************************************************/
void PDTexture::SetModulation(int value) {
	switch ( GetBlendMode() ) {
	  case SDL_BLENDMODE_BLEND:
	  case SDL_BLENDMODE_ADD:
	  	SDL_SetTextureAlphaMod( GetTexture(), (uint8_t) value );
	  	m_getMod = true;
	  	break;
	  
	  case SDL_BLENDMODE_MOD: {
	  	uint8_t uiRgb[3];
	  	uiRgb[0] = static_cast<uint8_t>(value & 0x000000FF);
	  	uiRgb[1] = static_cast<uint8_t>(value & 0x0000FF00);
	  	uiRgb[2] = static_cast<uint8_t>(value & 0x00FF0000);
	  	SDL_SetTextureColorMod( GetTexture(), uiRgb[0], uiRgb[1], uiRgb[2]);
	  	m_getMod = true;
	  }
	  	break;
	  
	  default: break;
	}
}

/**************************************************/
/* GetBlitSize
   Returns a point with the width and height of
   the texture's blitting rectangle.
*/
/**************************************************/
Point<int> PDTexture::GetBlitSize() const {
	return m_size;
}

/**************************************************/
/* GetClipRect
   Returns the clipping rectangle of the texture.
*/
/**************************************************/
SDL_Rect PDTexture::GetClipRect() const {
	return m_clip;
}

/**************************************************/
/* GetModulation
   Get the current modulation for the current
   blend mode.
*/
/**************************************************/
int PDTexture::GetModulation() {
	// same value was returned before
	if (! m_getMod) return m_mod;
	
	char iCode;
	
	// depending on the blend mode:
	switch ( GetBlendMode() ) {
	  case SDL_BLENDMODE_BLEND:
	  case SDL_BLENDMODE_ADD:
	  	// 0-255 alpha/color mod value
	  	iCode = SDL_GetTextureAlphaMod( GetTexture(),
	  	                       reinterpret_cast<uint8_t*>(&m_mod) );
	  	if (iCode < 0)
	  	  m_mod = 255;
	  	else
	  	  m_getMod = false;
	  	break;
	  
	  case SDL_BLENDMODE_MOD: {
	  	uint8_t uiRgb[3];
	  	iCode = SDL_GetTextureColorMod( GetTexture(), &uiRgb[0], &uiRgb[1], &uiRgb[2]);
	  	if (iCode < 0) {
	  	  m_mod = 0xFFFFFFFF;
	  	}
	  	else {
	  	  m_mod = 0xFF000000 | (uiRgb[2] << 16) | (uiRgb[1] << 8) | uiRgb[0];
	  	  m_getMod = false;
	  	}
	  }
	  	break;
	  
	  default:
	  	m_mod = 0;
	  	break;
	}
	
	return m_mod;
}

/**************************************************/
/* GetBlendMode
   Retrieves the texture's blend mode.
*/
/**************************************************/
SDL_BlendMode PDTexture::GetBlendMode() {
	if (! m_getBlend) return m_blend;
	
	int iCode = SDL_GetTextureBlendMode(GetTexture(), &m_blend);
	if (iCode < 0) {
	  SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to get texture blend mode: %s\n", SDL_GetError() );
	  m_blend = SDL_BLENDMODE_NONE;
	}
	else {
	  m_getBlend = false;
	}
	
	return m_blend;
}

/**************************************************/
/* GetRefCount
   Gets the reference count of the texture. If
   less than or equal to zero, that means no
   texture is loaded.
*/
/**************************************************/
int PDTexture::GetRefCount() const {
	return m_texture->count;
}

/**************************************************/
/* GetTexture
   Returns the drawing texture.
*/
/**************************************************/
SDL_Texture* PDTexture::GetTexture() const {
	return m_texture->texture;
}

/**************************************************/
/* Clone
   Returns a clone of the source object by copying
   internal data and incrementing the internal
   reference count.
*/
/**************************************************/
PDTexture* PDTexture::Clone() const {
	PDTexture* pClone = ::new PDTexture(m_texture, m_renderer);
	++(pClone->m_texture->count);
	pClone->m_size = m_size;
	
	return pClone;
}

/**************************************************/
/* NewTexture
   Returns a pointer to an instantiated texture
   on the heap.
   
   @param tex	An SDL_Texture to pack into
   		PDTexture
   @returns	A pointer to PDTexture that's
   		on the heap, or NULL if tex
   		is NULL
*/
/**************************************************/
PDTexture* PDTexture::NewTexture(SDL_Texture* tex) {
	PDTexture* retval = nullptr;
	
	if (tex) {
	  _RefTexture* ref = ::new _RefTexture;
	  ref->texture = tex;
	  ref->count = 1;
	  
	  retval = ::new PDTexture(ref, video::GetRenderer() );
	}
	
	return retval;
}

/**************************************************/
/* NewTexture
   Returns a freshly allocated PDTexture with
   graphics data based on the file argument.
   
   @param	file	string of the file to load
   @param	useCk	whether to use the following
   			argument (1+ for yes, 0 for
   			no)
   @param	ck	32-bit color value of
   			format 0xRRGGBBAA
   
   @returns	a pointer to a PDTexture allocated
   		on the heap
*/
/**************************************************/
PDTexture* PDTexture::NewTexture(const char* file, char useCk, uint32_t ck)
{
	PDTexture* retval = nullptr;
	SDL_Texture* img = video::LoadTexture(file, (useCk ? &ck : nullptr) );
	if (img) {
	  _RefTexture* ref = ::new _RefTexture;
	  ref->texture = img;
	  ref->count = 1;
	  
	  // construct a PDTexture
	  retval = ::new PDTexture(ref, video::GetRenderer() );
	}
	
	return retval;
}

/**************************************************/
/* Free
   Frees the given PDTexture pointer, freeing any
   memory associated with it.
   
   @param texture	The pointer to free
*/
/**************************************************/
void PDTexture::Free(PDTexture* texture) {
	if (texture == nullptr)
	  return;
	
	int iCount = texture->m_texture->count;
	
	// free internal texture
	if (--iCount == 0) {
	  SDL_DestroyTexture(texture->m_texture->texture);
	  ::delete texture->m_texture;
	}
	else
	  texture->m_texture->count = iCount;
	
	// delete PDTexture
	::delete texture;
}

/**************************************************/
/* video::LoadTexture
   Returns an SDL_Texture based on the file
   provided with optional colorkey.
*/
/**************************************************/
namespace video {

SDL_Texture* LoadTexture(SDL_Renderer* ren, const char* file, const uint32_t* colorkey)
{
	SDL_Texture* tex = nullptr;
	SDL_Surface* suf;
	
	// load image
	suf = IMG_Load(file);
	if (suf == nullptr)
	  Log_Error("Error in video::LoadTexture: no surface created: %s\n", IMG_GetError());
	else {
	  // set the colorkey per demand
	  if (colorkey) {
	  	uint32_t uiCk = *colorkey;
	  	uiCk = SDL_MapRGB(suf->format, uiCk >> 16, uiCk >> 8, uiCk);
	  	if ( SDL_SetColorKey(suf, SDL_TRUE, uiCk) < 0 )
	  	  Log_Error("Error in %s: failed to set surface colorkey--reason: %s\n", __FUNCTION__, IMG_GetError());
	  }
	  
	  // convert surface to texture
	  tex = SDL_CreateTextureFromSurface(ren, suf);
	  if (! tex)
	  	Log_Error("Error in %s: failed to convert surface to texture--reason: %s\n", __FUNCTION__, SDL_GetError());
	}
	SDL_FreeSurface(suf);
	
return tex;
}

SDL_Texture* LoadTexture(const char* file, const uint32_t* key) {
	return LoadTexture(GetRenderer(), file, key);
}

SDL_Texture* NewColorScreen(const uint8_t* color, int width, int height, uint32_t format, PatternArray& pattern) {
	SDL_Texture* tex = nullptr;
	SDL_Surface* suf;
	
	// create temp surface to hold all the pixels
	suf = SDL_CreateRGBSurfaceWithFormat(0, width, height, SDL_BITSPERPIXEL(format), format);
	if (!suf) {
	  const char* sError = SDL_GetError();
	  SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to construct surface: %s\n", sError);
	  Log_Cerr("Failed to construct surface in video::NewColorScreen : %s\n", sError);
	}
	else {
	  std::unique_ptr<uint32_t[]> uipPixels(new uint32_t[width * height]);
	  uint32_t uiColor = static_cast<uint32_t>(color[0]) | static_cast<uint32_t>(color[1]) | \
	                     static_cast<uint32_t>(color[2]) | 0xff;
	  
	  // for each pixel that matches the pattern, fill it with the specified color
	  for (int y = 0; y < height; ++y) {
	  	int iPixel = y * width;
	  	int iPatt;
	  	
	  	for (int x = 0; x < width; ++x) {
	  	  iPatt = pattern.offset(x, y);
	  	  if ( pattern.bytes[iPatt] )
	  	  	uipPixels[iPixel] = uiColor;
	  	  else
	  	  	uipPixels[iPixel] = 0;
	  	  ++iPixel;
	  	}
	  }
	  
	  // transfer pixels to the surface
	  if ( SDL_ConvertPixels(width, height, SDL_PIXELFORMAT_RGBA8888,
	                         (void*) uipPixels.get(), width * 4, format,
	                         suf->pixels, suf->pitch) < 0 ) {
	  	const char* sError = SDL_GetError();
	  	SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to transfer pixels to surface: %s\n", sError);
	  	Log_Cerr("Error in video::NewColorScreen : %s\n", sError);
	  }
	  else {
	  	// convert to texture
	  	tex = SDL_CreateTextureFromSurface( GetRenderer(), suf);
	  	if (! tex) {
	  	  const char* sError = SDL_GetError();
	  	  SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to convert surface to texture: %s\n", sError);
	  	  Log_Cerr("Error in video::NewColorScreen : %s\n", sError);
	  	}
	  }
	}
	
	SDL_FreeSurface(suf);
	
return tex;
}

}
