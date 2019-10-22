#include "gfxresources.hpp"
#include "textures.h"
#include "log.hpp"

namespace {
	static const int _START_SIZE = 5;
	static std::vector<PDTexture> _Textures;
	static int _IDCounter = 0;
}

/**************************************************/
/* Save texture and return a valid id tag. */
/**************************************************/
int video::SavePDTexture(const PDTexture& pdtex) {
	int iID = -1;
	auto itr = _Textures.begin();
	
	if (! _Textures.size())
	  _Textures.reserve(_START_SIZE);
	
	
	// search for an element that's already empty
	itr = std::find_if(itr, _Textures.end(), [](PDTexture& tex) -> bool {
		return (tex.texture == nullptr) || (! tex.width) || (! tex.height);
	});
	
	
	if (itr == _Textures.end()) {
	  iID = _IDCounter++;
	  _Textures.push_back(pdtex);
	}
	else {
	  iID = static_cast<int>( itr - _Textures.begin() );
	  Log_Print(iID);
	  Log_Test(iID < _Textures.size());
	  Log_Test(iID >= 0);
	}
	
return iID;
}

/**************************************************/
/* Package SDL_Texture into a PDTexture which
contains the SDL_Texture and a width and height. */
/**************************************************/
PDTexture video::NewPDTexture(SDL_Texture* tex) {
	PDTexture retval = {tex, 0};
	SDL_QueryTexture(tex, nullptr, nullptr, &(retval.width), &(retval.height));
	
return std::move(retval);
}

/**************************************************/
/* Clears the PDTexture marked by the resource
ID. Just voids that structure, doesn't remove it
from the vector entirely. */
/**************************************************/
void video::RemoveTexture(int id) {
	Log_Assert(id < _Textures.size(), "ID is invalid");
	
	int x = 0;
	auto itr = _Textures.begin();
	for (; itr != _Textures.end(); ++itr) {
	  if (x++ == id) {
	  	SDL_DestroyTexture(itr->texture);
	  	itr->texture = nullptr;
	  	itr->width = 0;
	  	itr->height = 0;
	  	break;
	  }
	}
}

/**************************************************/
/* Retrieve texture using its id tag. */
/**************************************************/
PDTexture& video::GetTexture(int id) {
	Log_Assert(id < _Textures.size(), "ID is invalid");
	
return _Textures[id];
}
