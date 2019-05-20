#ifndef TRIGGERS_HPP_INCLUDED
#define TRIGGERS_HPP_INCLUDED

#include <SDL.h>
#include <iostream>
#include <vector>
#include <cstring>

#include "global_attributes.hpp"
#include "gamemode_def.hpp"

#define DIALOGUE_NAME_LEN	129

// type of event
typedef enum {
	ETTNULL      = -2,
	ETTFail      = -1,
	ETTCamera    =  0,
	ETTCutscene  =  1,
	ETTDialogue  =  2,
	ETTInput     =  3
} EventTriggerType;

// event to change the camera's position
struct CameraEvent {
	int8_t   speed; // if zero, snaps camera to position by <increments> amount, else it scrolls at <speed> speed
	uint16_t increments; // how many screen units to push the camera
	
	// ctor, dtor, copy, move
	CameraEvent() {}
	~CameraEvent() {}
	CameraEvent(const CameraEvent& _rhs) {}
	CameraEvent(CameraEvent&& _rhs) {}
};

// undefined event
struct CutsceneEvent {
	int8_t fadeType; // -1 = fade in, 0 = no fade, 1 = fade out
	char   temp; // placeholder
	
	// ctor, dtor, copy, move
	CutsceneEvent() {}
	~CutsceneEvent() {}
	CutsceneEvent(const CutsceneEvent& _rhs) {}
	CutsceneEvent(CutsceneEvent&& _rhs) {}
};

// event to spawn a dialogue box and print text to it
struct DialogueEvent {
	char name[DIALOGUE_NAME_LEN]; // name of a character whose portrait to show
	char** text; // each element represents a body of text to be displayed in a box
	uint16_t size; // size of the double array, how many "paragraphs" to display
	uint16_t off; // offset into the array, which string to display
	
	// ctor, dtor, copy, move
	DialogueEvent() {}
	~DialogueEvent();
	DialogueEvent(const DialogueEvent& _rhs) {
		std::cout << "DialogueEvent copy ctor\n";
	}
	DialogueEvent(DialogueEvent&& _rhs) {
		std::cout << "DialogueEvent move ctor\n";
	}
};

// main event structure; estimated size is 172 bytes
struct EventTrigger {
	EventTriggerType type;
	SDL_Rect bbox; // box representing the area of the trigger
	union {
	  CameraEvent camera;
	  CutsceneEvent cutscene;
	  DialogueEvent dialogue;
	};
	
	// ctor, dtor, copy, move
	EventTrigger();
	~EventTrigger() {}
	EventTrigger(const EventTrigger& _rhs);
	EventTrigger(EventTrigger&& _rhs) {
		move( std::move(_rhs) );
	}
	
	// operators
	EventTrigger& operator=(EventTrigger&& _rhs) {
		
		move( std::move(_rhs) ); 
		
	return *this;
	}
	
	// move function
	void move(EventTrigger&& src);
};

#endif /* TRIGGERS_HPP_INCLUDED */
