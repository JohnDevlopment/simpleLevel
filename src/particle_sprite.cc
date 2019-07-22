#include "particle_sprite_def.h"
#include "camera.hpp"
#include "game.hpp"

ParticleSprite::ParticleSprite(size_t num, SDL_Renderer* renderer, SDL_Texture* texture, int w, int h) : m_count(num), m_ren(renderer), m_tex(texture)
{
	// ctor
	assert(num > 0 && (w > 0 && h > 0));
	assert(texture != nullptr);
	assert(renderer != nullptr);
	
	// allocate array of particle structures
	m_particles = new Particle*[num];
	for (size_t x = 0; x < m_count; ++x) {
	  Particle* ptr = new Particle;
	  std::memset(ptr, 0, sizeof(*ptr));
	  ptr->clip.w = w;
	  ptr->clip.h = h;
	  ptr->blit.w = w;
	  ptr->blit.h = h;
	  m_particles[x] = ptr;
	}
	m_blit.w = w;
	m_blit.h = h;
}

ParticleSprite::~ParticleSprite()
{
	// dtor
	for (size_t x = 0; x < m_count; ++x) {
	  delete m_particles[x];
	}
	delete[] m_particles;
	SDL_DestroyTexture(m_tex);
}

void ParticleSprite::set_locations(int x, int y, FPV_SetVals dist, void* udata) {
	Particle* ptr;
	for (size_t x = 0; x < m_count; ++x) {
	  ptr = m_particles[x];
	  ptr->blit.x = x;
	  ptr->blit.y = y;
	  if (dist) {
	  	dist(ptr, udata);
	  }
	}
}

void ParticleSprite::render() {
	Particle* ptr;
	SDL_Rect& camera = CAM_CAMERA;
	
	for (size_t x = 0; x < m_count; ++x) {
	  ptr = m_particles[x];
	  if (ptr->length == 0 || ptr->frame < 0) continue; // skip inactive particle
	  m_blit.x = ptr->blit.x - camera.x;
	  m_blit.y = ptr->blit.y - camera.y;
	  SDL_RenderCopy(m_ren, m_tex, &ptr->clip, &m_blit);	  
	}
}

void ParticleSprite::update(int flags) {
	Particle* ptr;
	int iFrame;
	
	for (size_t x = 0; x < m_count; ++x) {
	  ptr = m_particles[x];
	  iFrame = ptr->frame++;
	  if (iFrame >= 0) {
	  	if (iFrame == (int) ptr->length) {
	  	  ptr->length = 0;
	  	}
	  	else {
	  	  clip_frame(ptr);
	  	}
	  }
	  if (flags & 1 && game::FrameCounter & 1) {
	  	ptr->frame--;
	  }
	}
}

void ParticleSprite::clip_frame(Particle* ptr) {
	SDL_Rect& clip = ptr->clip;
	clip.x = ptr->frame * clip.w;
	clip.y = 0;
}
