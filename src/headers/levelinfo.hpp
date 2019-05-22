#ifndef LEVELINFO_HPP
#define LEVELINFO_HPP

#include <iostream>
#include <custom/levelLoad.hpp>

#include "game.hpp"

#include "global_attributes.hpp"
#include "texture_packs.hpp"

const char*	LevelInfo_Background(const LevelHeader& header);
const char*	LevelInfo_Tileset(const unsigned int index);
const char*	LevelInfo_Music(const unsigned int index);
const char*	LevelInfo_Sprite(unsigned int id);
const char*	LevelInfo_Sfx(unsigned int id);
double		LevelInfo_MusicLoop(const size_t id);
bool		LevelInfo_TexturePack(unsigned int value, TexurePackArgs& args);
int		LevelInfo_MusicID(const std::string& str);
Image*		LevelInfo_BackgroundLayerID(unsigned int id) PURE;

#endif /* LEVELINFO_HPP */