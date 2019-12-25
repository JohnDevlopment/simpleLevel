/****************************************************************************************************/
/* * File = pdresourcemanager.hpp
   * Project = Project Dewpsi
   * File type = source
   * Description: This source file is part of the project Project Dewpsi. Here is defined the class
   * PDResourceManager, which holds a list of textures associated with some kind of textual or
   * numerical ID.
   *
   *
*/
/****************************************************************************************************/
#ifndef PDRESOURCEMANAGER_HPP_INCLUDED
#define PDRESOURCEMANAGER_HPP_INCLUDED

/* System headers */
#include <iostream>
#include <unordered_map>

/* User-defined headers */
#include "stdinc.h"
#include "sdl_incs.h"
#include "PDTexture.h"

/* Loads textures based on an image file. */
class PDResourceManager {
	typedef std::unordered_map<std::string, PDTexture*> TexMap;
	typedef TexMap::iterator TexItr;
	typedef std::pair<std::string, PDTexture*> TexPair;
	typedef std::pair<TexItr, bool> ResultPair;
	
	TexMap m_textures;
	
	public:
	
	// default constructor
	PDResourceManager() = default;
	~PDResourceManager();
	
	// load image file into a texture
	bool NewTexture(const char* file, const uint32_t* colorkey);
	
	// get a clone of a texture based on the file name
	PDTexture* GetTexture(const char* file) const;
	
	// clear all textures
	void Clear();
};

#endif /* PDRESOURCEMANAGER_HPP_INCLUDED */
