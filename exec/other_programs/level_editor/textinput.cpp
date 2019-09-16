#include "textinput.h"
#include "level.hpp"
#include <chrono>
#include <memory>

using level::Flags;

static std::string _text;
static Timer _timer;
static const int _ticks = 3000;

void popChar() {
    if ( _text.size() > 0 ) {
        _text.pop_back();
    }
}

const char* getText() {
    return _text.c_str();
}

int checkTextInput() {
    // stop the timer if it reaches the limit or text input was forcibly turned off
    if ( _timer.is_started() ) {
        if ( _timer.get() >= _ticks || ! Flags.mask(FLAGS_TEXT_INPUT) )
            return 1;
    }
    
return 0;
}

void addText(const char* str) {
    if ( str && std::strlen(str) > 0 ) {
        if ( ! _text.size() )
            _text = str;
        else
            _text += str;
    }
}

int startTextInput() {
    // set bit flag
    level::Flags.set(FLAGS_TEXT_INPUT);
    
    // clear string
    _text.clear();
    
    // start text input
    SDL_StartTextInput();
    
    // start timer
    _timer.start();
    
return 0;
}

void endTextInput() {
    int iThreadResult = 0;
    
    // stop timer
    _timer.stop();
    
    // clear bit flag
    level::Flags.unset(FLAGS_TEXT_INPUT);
    
    // stop text input
    SDL_StopTextInput();
}


