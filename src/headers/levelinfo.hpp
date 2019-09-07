#ifndef LEVELINFO_HPP
#define LEVELINFO_HPP

#include <iostream>
#include <custom/levelLoad.hpp>

#include "game.hpp"

#include "global_attributes.hpp"
#include "texture_packs.hpp"

const char*	LevelInfo_Background(const unsigned int index);
std::string	LevelInfo_Tileset(const unsigned int index);
const char*	LevelInfo_Music(const unsigned int index);
const char*	LevelInfo_Sfx(unsigned int id);
double		LevelInfo_MusicLoop(const size_t id);
bool		LevelInfo_TexturePack(unsigned int value, TexurePackArgs& args);
int		LevelInfo_MusicID(const std::string& str);
const float*	LevelInfo_BGScrollSpeeds(const size_t id);

#endif /* LEVELINFO_HPP */
