#ifndef PDTEXTURE_H_INCLUDED
#define PDTEXTURE_H_INCLUDED

#include "sdl_incs.h"
#include "pointvector"

struct _RefTexture;
//struct _RotData;
#include "_RotData.h"

/**************************************************/
/*
A data structure used by this engine to organize
texture data with methods to make, change, query,
or display textures.
*/
/**************************************************/
class PDTexture {
	_RefTexture* m_texture;
	SDL_Renderer* m_renderer;
	_RotData m_rotation;
	SDL_Rect m_clip;
	Point<int> m_size;

	SDL_BlendMode m_blend;
	uint32_t m_mod;
	bool m_getMod;
	bool m_getBlend;

	PDTexture(_RefTexture* ref, SDL_Renderer* ren);

	public:

	PDTexture(const PDTexture& src) = delete;
	PDTexture(PDTexture&& src) = default;
	~PDTexture();

	// functions to set clipping and blitting rectangles //

	// sets the blitting size of the texture
	void SetBlitSize(const Point<int>& size);

	// resets the blitting size to the texture's original size
	void ResetBlitSize();

	// rotates the texture clockwise by n degrees
	void SetRotation(double angle);

	// sets the pivot for the rotation; pass NULL for center
	void SetPivot(SDL_Point* pivot);

	// disables rotation for the texture
	void DisableRotation();

	// sets the texture blend mode
	int SetBlendMode(SDL_BlendMode mode);

	// sets modulation for the current blend mode
	void SetModulation(int value);

	// returns the blitting size
	Point<int> GetBlitSize() const;
	
	// fetch the clipping rectangle
	SDL_Rect GetClipRect() const;

	// get modulation for the current blend mode
	int GetModulation();

	// get blend mode, how texture will be blended onto screen
	SDL_BlendMode GetBlendMode();

	// get pointer to texture
	SDL_Texture* GetTexture() const;

	// get the texture reference count
	int GetRefCount() const;

	// returns an exact clone of the texture
	PDTexture* Clone() const;

	// packs an SDL_Texture into PDTexture
	static PDTexture* NewTexture(SDL_Texture* tex);

	// creates texture based on file
	static PDTexture* NewTexture(const char* file, char useCk, uint32_t ck = 0);

	// frees the texture from memory and decrements its internal reference count
	static void Free(PDTexture* texture);
};

#endif /* PDTEXTURE_H_INCLUDED */
