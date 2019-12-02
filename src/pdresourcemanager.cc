#include "pdresourcemanager.hpp"
#include "sdl_incs.h"
#include "string.h"
#include "log.hpp"

PDResourceManager::~PDResourceManager()
{
}

/**************************************************/
/* NewTexture
   Accepts a file name and a pointer to an integer.
   Creates a PDTexture and adds it to the manager.
   
   @param file		string file name
   @param colorkey	pointer to an integer of
   			format 0xRRGGBBAA; pixels
   			of that color will be trans-
   			parent
  @returns		-1 on failure, otherwise
  			literally any other value
*/
/**************************************************/
bool PDResourceManager::NewTexture(const char* file, const uint32_t* colorkey) {
	bool success = false;
	
	const char* sLastSlash = String_strrchr(file, '/');
	std::string sFileName = (sLastSlash != nullptr) ? (sLastSlash+1) : file;
	
	// if texture already exists
	TexItr found = m_textures.find(sFileName);
	if ( found != m_textures.end() ) {
	  return true;
	}
	
	PDTexture* myTexture = PDTexture::NewTexture(file,
	                                             static_cast<int>(colorkey != nullptr),
	                                             (colorkey ? *colorkey : 0U) );
	
	// insert texture into map
	if (myTexture != nullptr) {
	  TexPair entry = std::make_pair( std::move(sFileName), myTexture);
	  ResultPair result = m_textures.insert(entry);
	  success = result.second;
	  if (! success) {
	  	PDTexture::Free(myTexture);
	  	myTexture = nullptr;
	  }
	}
	
return success;
}

/**************************************************/
/* GetTexture
   Locates the texture specified by the file arg-
   ument and returns a clone of that texture.
   
   @param file	a string specifying which texture
   		to find
   @returns	a pointer to PDTexture which has
   		to be deleted by the caller.
   		Pointer is a result of calling
   		PDTexture::Clone
*/
/**************************************************/
PDTexture* PDResourceManager::GetTexture(const char* file) const {
	PDTexture* result = nullptr;
	
	// if the string is a full path to a file, change it to a basename
	const char* sLastSlash = String_strrchr(file, '/');
	if (sLastSlash == nullptr)
	  sLastSlash = file;
	else
	  ++sLastSlash;
	
	std::string sFileName = sLastSlash;
	
	// find texture based on base of file path
	auto found = m_textures.find(sFileName);
	if (found != m_textures.end() ) {
	  PDTexture* ogTexture = found->second;
	  result = ogTexture->Clone();
	}
	
return result;
}

/**************************************************/
/* Clear
   Clears every texture in the manager.
*/
/**************************************************/
void PDResourceManager::Clear() {
	TexItr beg = m_textures.begin();
	TexItr end = m_textures.end();
	
	while (beg != end) {
	  PDTexture::Free(beg->second);
	  ++beg;
	}
	
	m_textures.clear();
}
