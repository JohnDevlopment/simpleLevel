// engine headers
#include "textures.h"
#include "log.hpp"

namespace {
	const size_t _CLEAR_SIZE = sizeof(TexWrapper_t) + sizeof(TexFade_t) + sizeof(TexMov_t);
	const uint8_t _FADE_DEFAULT = 255;
}

/**************************************************/
/* PDTexture
   Accepts SDL_Texture and SDL_Renderer and renders
   the texture differently according to the type
   configuration.
*/
/**************************************************/
void PDTexture::set_vals() {
	String_memset(this, 0, sizeof(PDTexture) );
	
	m_count = -1;
	m_type = INVALID;
}

void PDTexture::get_tex_ren(SDL_Renderer*& myRenderer, SDL_Texture*& myTexture) {
	switch (m_type) {
	  case TEXTURE: {
	  	myTexture  = m_wrapper.m_texture;
	  	myRenderer = m_wrapper.m_renderer;
	  	break;
	  }
	  
	  case FADER: {
	  	myTexture  = m_fader.m_wrapper.m_texture;
	  	myRenderer = m_fader.m_wrapper.m_renderer;
	  	break;
	  }
	  
	  case MOVER: {
	  	myTexture  = m_mover.m_wrapper.m_texture;
	  	myRenderer = m_mover.m_wrapper.m_renderer;
	  	break;
	  }
	  
	  default: {
	  	myTexture = nullptr;
	  	myRenderer = nullptr;
	  	break;
	  }
	}
}

void PDTexture::copy_direct(const PDTexture& src) {
	m_count = src.m_count;
	m_type = src.m_type;
	m_size = src.m_size;
	
	switch (m_type) {
	  case TEXTURE: {
	  	m_wrapper.m_texture = src.m_wrapper.m_texture;
	  	m_wrapper.m_renderer = src.m_wrapper.m_renderer;
	  	break;
	  }
	  
	  case FADER: {
	  	m_fader.m_wrapper.m_texture = src.m_fader.m_wrapper.m_texture;
	  	m_fader.m_wrapper.m_renderer = src.m_fader.m_wrapper.m_renderer;
	  	m_fader.m_fade = src.m_fader.m_fade;
	  	break;
	  }
	  
	  case MOVER: {
	  	m_mover.m_wrapper.m_texture = src.m_mover.m_wrapper.m_texture;
	  	m_mover.m_wrapper.m_renderer = src.m_mover.m_wrapper.m_renderer;
	  	m_mover.m_clip = src.m_mover.m_clip;
	  	m_mover.m_blit = src.m_mover.m_blit;
	  	break;
	  }
	  
	  default: break;
	}
}

void PDTexture::move(PDTexture&& src) {
	// don't copy src's values if src is not loaded
	if (src.m_type == INVALID) {
	  set_vals();
	  return;
	}
	
	// copy all values directly from the source without allocating any memory
	copy_direct(src);
	
	// reset src to its default state
	src.set_vals();
}

void PDTexture::SetTexture(SDL_Texture* tex, SDL_Renderer* ren, int type) {
	Free();
	
	m_count = 0;
	m_type = type;
	
	if (tex)
	  SDL_QueryTexture(tex, nullptr, nullptr, &m_size[0], &m_size[1]);
	
	switch (type) {
	  case TEXTURE: {
	  	m_wrapper.m_texture = tex;
	  	m_wrapper.m_renderer = ren;
	  	break;
	  }
	  
	  case FADER: {
	  	m_fader.m_wrapper.m_texture = tex;
	  	m_fader.m_wrapper.m_renderer = ren;
	  	m_fader.m_fade = _FADE_DEFAULT;
	  	SDL_SetTextureBlendMode(tex, SDL_BLENDMODE_BLEND);
	  	break;
	  }
	  
	  case MOVER: {
	  	m_mover.m_wrapper.m_texture = tex;
	  	m_mover.m_wrapper.m_renderer = ren;
	  	SDL_Rect temp = {0, 0, m_size.x(), m_size.y()};
	  	SetBlitRect(&temp);
	  	SetClipRect(&temp);
	  	break;
	  }
	  
	  default: break;
	}
	
	
}

void PDTexture::Free() {
	switch (m_type) {
	  case TEXTURE: {
	  	if (m_wrapper.m_texture)
	  	  SDL_DestroyTexture(m_wrapper.m_texture);
	  	break;
	  }
	  
	  case FADER: {
	  	if (m_fader.m_wrapper.m_texture)
	  	  SDL_DestroyTexture(m_fader.m_wrapper.m_texture);
	  	break;
	  }
	  
	  case MOVER: {
	  	if (m_mover.m_wrapper.m_texture)
	  	  SDL_DestroyTexture(m_mover.m_wrapper.m_texture);
	  	break;
	  }
	  
	  default: break;
	}
	
	set_vals();
}

uint8_t PDTexture::GetAlpha() const {
	return (m_type == FADER) ? m_fader.m_fade : 255;
}

void PDTexture::SetAlpha(uint8_t a) {
	if (m_type == FADER) {
	  if ( SDL_SetTextureAlphaMod(m_fader.m_wrapper.m_texture, a) < 0 ) {
	  	std::cerr << SDL_GetError() << '\n';
	  }
	  else {
	  	m_fader.m_fade = a;
	  }
	}
}

void PDTexture::Blit() {
	SDL_Texture*  myTexture  = nullptr;
	SDL_Renderer* myRenderer = nullptr;
	
	get_tex_ren(myRenderer, myTexture);
	
	SDL_RenderCopy(myRenderer, myTexture, nullptr, nullptr);
}

void PDTexture::Blit(const Point<int>& pos) {
	SDL_Texture*  myTexture  = nullptr;
	SDL_Renderer* myRenderer = nullptr;
	
	get_tex_ren(myRenderer, myTexture);
	
	m_mover.m_blit.x = pos.x();
	m_mover.m_blit.y = pos.y();
	
	SDL_RenderCopy(myRenderer, myTexture, &m_mover.m_clip, &m_mover.m_blit);
}

void PDTexture::SetClipRect(const SDL_Rect* rect) {
	if (rect) {
	  m_mover.m_clip = *rect;
	}
	else {
	  m_mover.m_clip.x = 0;
	  m_mover.m_clip.y = 0;
	  m_mover.m_clip.w = m_size.x();
	  m_mover.m_clip.h = m_size.y();
	}
}

void PDTexture::SetBlitRect(const SDL_Rect* rect) {
	if (rect) {
	  m_mover.m_blit = *rect;
	}
	else {
	  m_mover.m_blit.w = m_size.x();
	  m_mover.m_blit.h = m_size.y();
	}
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

/**************************************************/
/* video::NewTexMov
   Returns a pointer to a newly allocated TexMov_t.
   Returns NULL if the operation failed.
*/
/**************************************************/
TexMov_t* video::NewTexMov(const char* file, const uint32_t* colorkey) {
	TexMov_t* retval;
	SDL_Texture* texture = LoadTexture(file, colorkey);
	
	if (texture) {
	  retval = new TexMov_t;
	  retval->m_wrapper.m_texture = texture;
	  retval->m_wrapper.m_renderer = video::GetRenderer();
	}
	else
	  retval = nullptr;
	
return retval;
}
