#ifndef LEVELLOAD_HPP
#define LEVELLOAD_HPP

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

#define LEVELLOAD_LITTLE_ENDIAN	0x0000
#define LEVELLOAD_BIG_ENDIAN	0x1111

struct SprObj {
	uint8_t x;
	uint8_t y;
	uint8_t id;
	uint8_t flags;
};

struct Tile {
	uint16_t graphicID;
	uint16_t codeID;
	uint8_t flags;
	uint8_t empty;
};

struct LevelHeader {
	uint16_t endian;
	uint8_t  width;
	uint8_t  height;
	uint8_t  background;
	uint8_t  tileset;
	uint8_t  music;
	uint8_t  sprites;
};

extern int Level_WriteFile(const char* filename, LevelHeader header, Tile* tiles, SprObj* sprites);

extern int Level_ReadFile(const char* filename, LevelHeader* header, Tile** tiles, SprObj** sprites);

extern const char* Level_GetError();

#endif /* LEVELLOAD_HPP */
