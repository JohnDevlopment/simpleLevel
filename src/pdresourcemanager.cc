#include "pdresourcemanager.hpp"
#include "sdl_incs.h"
#include "string.h"
#include "log.hpp"

/**************************************************/
/* Loads a texture from the given file and returns
a pointer to SDL_Texture. UnloadTexture() must be
called for every instance this was called for the
same texture. */
/**************************************************/
PDTexture& PDResourceManager::LoadTexture(int type, const char* file, const uint32_t* colorkey) {
	std::string sFileName = file;
	
	// just return the texture if it already exists
	TexItr itrFound = m_textures.find(sFileName);
	if (itrFound != m_textures.end() ) {
	  PDTexture& refTexture = itrFound->second;
	  refTexture.Increment();
	  return refTexture;
	}
	
	// get texture from file
	SDL_Texture* mySDLTexture = video::LoadTexture(file, colorkey);
	Log_Assert(mySDLTexture, "Failed to create SDL_Texture");
	
	// else insert PDTexture to map
	{
	  //std::unique_ptr<PDTexture> myTexture(new PDTexture);
	  PDTexture myTexture;
	  myTexture.SetTexture(mySDLTexture, video::GetRenderer(), type);
	  
	  // pair that gets inserted into map
	  auto pairToInsert = std::make_pair( std::move(sFileName), std::move(myTexture) );
	  // insert pair into map
	  std::pair<TexItr, bool> insertWorked 
	  	= m_textures.insert( std::move(pairToInsert) );
	  
	  // when this fails, throw an exception
	  if (! insertWorked.second) {
	  	std::string sExecption = "failed to insert texture into map | texture file: ";
	  	sExecption += file;
	  	throw std::runtime_error( std::move(sExecption) );
	  }
	  // get iterator to the inserted element
	  itrFound = insertWorked.first;
	}
	
	// return value is a reference
	PDTexture& retval = itrFound->second;
	
	// increment reference count
	retval.Increment();
	
return retval;
}

/**************************************************/
/* Unloads a texture from the internal map if
the given name exists. If unloaded as many times
as it was loaded, the texture deletes itself. */
/**************************************************/
void PDResourceManager::UnloadTexture(const char* name) {
	TexItr found = m_textures.find(name);
	if (found != m_textures.end() ) {
	  if (found->second.Decrement() == 0) {
	  	found->second.Free();
	  	m_textures.erase(found); // erase from hash table
	  }
	}
}

/**************************************************/
/* Delete every texture in the map and then empty
it. */
/**************************************************/
void PDResourceManager::Clear() {
	TexItr itr = m_textures.begin();
	for (; itr != m_textures.end(); ++itr)
	  itr->second.Free();
	
	m_textures.clear();
}
