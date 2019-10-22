// global game data
#include "game.hpp"

// Tcl scripting
#include "tcl.hpp"

// math functions
#include "math.hpp"

// level data
#include "levelcode.hpp"

// level gamemode
#include "gm_level.hpp"

// camera system
#include "camera.hpp"

// event system
#include "triggers.hpp"

// logging output
#include "log.hpp"

using namespace std;

// DialogueEvent class methods //
DialogueEvent::~DialogueEvent() {
	if (text != nullptr) {
	  // free elements in the array
	  for (size_t x = 0; x < size; ++x)
	  	delete[] text[x];
	  
	  // free pointer
	  delete[] text;
	}
}

// EventTrigger class methods //
EventTrigger::EventTrigger() {
	memset(this, 0, sizeof(EventTrigger) );
	type = ETTNULL;
}

EventTrigger::EventTrigger(const EventTrigger& _rhs) : type(_rhs.type), bbox(_rhs.bbox) {
	// copy contents over
	switch (type) {
	  case ETTCamera:
	  	camera.speed = _rhs.camera.speed;
	  	camera.increments = _rhs.camera.increments;
	  	break;
	  
	  case ETTCutscene:
	  	cutscene.fadeType = _rhs.cutscene.fadeType;
	  	cutscene.temp = 0;
	  	break;
	  
	  case ETTDialogue:
	  	std::strncpy(dialogue.name, _rhs.dialogue.name, DIALOGUE_NAME_LEN);
	  	dialogue.text = _rhs.dialogue.text;
	  	dialogue.size = _rhs.dialogue.size;
	  	dialogue.off  = _rhs.dialogue.off;
	  	break;
	  
	  default: break;
	}
}

void EventTrigger::move(EventTrigger&& src) {
	// transfer contents over
	type = src.type;
	bbox = src.bbox;
	
	switch (type) {
	  case ETTCamera:
	  	camera.speed = src.camera.speed;
	  	camera.increments = src.camera.increments;
	  	
	  	src.camera.speed	= 0;
	  	src.camera.increments	= 0;
	  	break;
	  
	  case ETTCutscene:
	  	cutscene.fadeType	= src.cutscene.fadeType;
	  	cutscene.temp		= 0;
	  	
	  	src.cutscene.fadeType	= 0;
	  	src.cutscene.temp	= 0;
	  	break;
	  
	  case ETTDialogue:
	  	strncpy(dialogue.name, src.dialogue.name, DIALOGUE_NAME_LEN);
	  	dialogue.text	= std::move(src.dialogue.text);
	  	dialogue.size	= src.dialogue.size;
	  	dialogue.off	= src.dialogue.off;
	  	
	  	memset(dialogue.name, 0, DIALOGUE_NAME_LEN);
	  	src.dialogue.size	= 0;
	  	src.dialogue.off	= 0;
	  	break;
	  
	  default: break;
	}
	
	// clear type and bbox
	src.type	= ETTNULL;
	src.bbox.x	= 0;
	src.bbox.y	= 0;
	src.bbox.w	= 0;
	src.bbox.h	= 0;
}

// public functions //
