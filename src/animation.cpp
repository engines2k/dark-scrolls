#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include "game.hpp"
#include "animation.hpp"
#include <string>

Animation::Animation(Game &game, int nframes) : game(game) {
}

void Animation::set_frame(int fnum, AnimationFrame aframe) {
}

AnimationFrame::AnimationFrame(std::string filepath) {
	surface = IMG_Load(filepath.c_str());
}
AnimationFrame::~AnimationFrame() {
	// if(surface != NULL) SDL_FreeSurface(surface);
	// if(sound != NULL) Mix_FreeChunk(sound);
}