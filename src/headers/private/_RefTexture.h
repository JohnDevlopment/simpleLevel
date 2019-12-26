#ifndef _REFTEXTURE_H_INCLUDED
#define _REFTEXTURE_H_INCLUDED

/**************************************************/
/*
Holds a texture and an integer that counts how
many other textures refer to this one.
*/
/**************************************************/
struct _RefTexture {
	SDL_Texture* texture;
	int count;
	
	_RefTexture() : texture(nullptr), count(-1) {}
};

#endif /* _REFTEXTURE_H_INCLUDED */
