#ifndef GM_MENU_HPP
#define GM_MENU_HPP

#include <iostream>

#include "gamemode.hpp"
#include "program.hpp"

int gm_menu_event_mousebutton(SDL_Event& event, GameMode* const gm, uint8_t mask);

int gm_menu(void* const gamemode_void, const PROGRAM& program);

#endif /* GM_MENU_HPP */
