#ifndef SPRITE_DEF_H_INCLUDED
#define SPRITE_DEF_H_INCLUDED

#include "stdinc.h"
#include "object/object.hpp"

#define GRAVITY_SPEED	8

// enumerations for which bits are for which blocked status
enum {
	M_COLL_UP    = 0x08,
	M_COLL_DOWN  = 0x04,
	M_COLL_LEFT  = 0x02,
	M_COLL_RIGHT = 0x01
};

// data structure that represents a public version of Sprite
struct SpriteP {
	uint8_t m_id;         // sprite identifier
	uint8_t m_actv;       // sprite activation bit: the sprite is or is not currently defined
	uint8_t m_init;       // sprite activation bit: the sprite's activation bit has been set
	uint8_t m_offh;       // sprite offscreen horizontal: being non-zero means the sprite is off the left/right edges of the camera
	uint8_t m_offv;       // sprite offscreen vertical: being non-zero means the sprite is off the up/down edges of the camera
	uint8_t m_del;        // sprite deletion flag: if non-zero, the sprite is deleted
	uint8_t m_coll;       // collision flags: 0000 0000| sprite blocked status, set when it hits a wall, ceiling or floor
	                      //                       udlr|
	uint8_t m_state;      // sprite state: can be used to jump to different parts of the sprite
	uint8_t m_hp;         // sprite hp value
};

/* Data structure that encapsulates a sprite. Estimated size is 208 bytes based on a rough calculation. */
class Sprite {
protected:
	uint8_t m_id;         // sprite identifier
	uint8_t m_actv   : 1; // sprite activation bit: the sprite is or is not currently defined
	uint8_t m_init   : 1; // sprite activation bit: the sprite's activation bit has been set
	uint8_t m_offh   : 1; // sprite offscreen horizontal: being non-zero means the sprite is off the left/right edges of the camera
	uint8_t m_offv   : 1; // sprite offscreen vertical: being non-zero means the sprite is off the up/down edges of the camera
	uint8_t m_del    : 4; // sprite deletion flag: if non-zero, the sprite is deleted
	uint8_t m_coll;       // collision flags: 0000 0000| sprite blocked status, set when it hits a wall, ceiling or floor
	                      //                       udlr|
	uint8_t m_state;      // sprite state: can be used to jump to different parts of the sprite
	uint8_t m_hp;         // sprite hp value

public:
	// ctor
	Sprite(int id) : m_id(id), m_actv(0), m_init(0), m_offh(0), m_offv(0), m_del(0), m_coll(0), m_state(0), m_hp(0) {}

	// ctor
	virtual ~Sprite() {}
	
	// set INIT flag
	void SetInit() {
		m_init = 1;
	}
	
	// clear collision bits
	void ClearColl() {
		m_coll = 0;
	}
	
	// set a certain bit
	void SetColl(uint8_t bit) {
		m_coll |= bit;
	}
	
	// unset a certain bit
	void UnsetColl(uint8_t bit) {
		m_coll &= 0xff ^ bit;
	}
	
	// returns true if a certain collision bit is set
	bool GetColl(uint8_t bit) const {
		return m_coll & bit;
	}
	
	// returns a public copy of this structure with no methods
	SpriteP&& PublicCopy() const {
		SpriteP retval = {m_id, m_actv, m_init, m_offh, m_offv, m_del, m_coll, m_state, m_hp};
		
	return std::move(retval);
	}
	
	// get the id of the sprite
	uint8_t GetID() const {return m_id;}
	
	// get the hp of the sprite
	uint8_t GetHP() const {return m_hp;}
	
	// methods for derived classes //
	
	// function for the main sprite code
	virtual int Main(void*) = 0;
	
	// initializes the sprite
	virtual int Init() = 0;
	
	// hurt the sprite
	virtual int Hurt() = 0;
	
	// kills the sprite
	virtual int Kill() = 0;
};

#endif /* SPRITE_DEF_H_INCLUDED */
