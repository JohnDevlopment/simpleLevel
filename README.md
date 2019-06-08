# simpleLevel
A game I'm developing, titled Project Dewpsi.

To execute this game in Linux, just go to the main directory and type in `make` on the commandline. Then type `./execute.sh` on the commandline to start up the game. Right now the only thing you can do is move the character left and right and interact with a few tiles.

The game requires these libraries to compile:
  * SDL2
  * SDL2_image
  * SDL2_ttf

EDIT: SDL2_mixer was removed from the list. This is because I read the source code for SDL_mixer 2.0.1 and incorporated it into my code.

The following files contain code that was taken from SDL_mixer version 2.0.1's source code:
  * src/sound.cc
  * src/sound/sound_music_fmt.cc
  * src/sound/sound_wavstream.cc
  * src/headers/sound.hpp
  * src/headers/sound/sound_channel_def.h
  * src/headers/sound/sound.ogg.h
  * src/headers/sound/sound.wav.h
