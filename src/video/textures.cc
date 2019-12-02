// engine headers
#include "textures.h"
#include "PDTexture.h"
#include "log.hpp"
#include "_RefTexture.h"
#include "_RotData.h"

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
   m_size(),
   m_opacity(255)
{
}

PDTexture::~PDTexture()
{
	m_texture = nullptr;
}

/**************************************************/
/* ResetBlitRect
   Resets the blit size to the original size of
   the texture.
*/
/**************************************************/
void PDTexture::ResetBlitSize() {
	Log_Assert(m_texture, "For some reason PDTexture::m_texture is NULL");
	int code = SDL_QueryTexture(m_texture->texture, nullptr, nullptr, &m_size[0], &m_size[1]);
	Log_Assert(! code, SDL_GetError() );
}

/**************************************************/
/* Clone
   Returns a clone of the source object by copying
   internal data and incrementing the internal
   reference count.
*/
/**************************************************/
PDTexture* PDTexture::Clone() const {
	PDTexture* pClone = reinterpret_cast<PDTexture*>( std::malloc( sizeof(PDTexture) ) );
	
	::new (pClone) PDTexture(m_texture, m_renderer);
	++(pClone->m_texture->count);
	
	return pClone;
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
using namespace video;

SDL_Texture* video::LoadTexture(SDL_Renderer* ren, const char* file, const uint32_t* colorkey)
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

SDL_Texture* video::LoadTexture(const char* file, const uint32_t* key) {
	return LoadTexture(GetRenderer(), file, key);
}
