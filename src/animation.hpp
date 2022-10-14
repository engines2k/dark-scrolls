#pragma once
#include <map>
#include <SDL2/SDL.h>
#include "game.hpp"
#include <string>


class Game;

class AnimationFrame {
	public:
	SDL_Surface *surface;
	Mix_Chunk *sound;

	AnimationFrame(std::string filepath);

	~AnimationFrame();
};

class Animation {
	public:
	std::vector<AnimationFrame> frames;
	int start_frame;
	Game &game;

	Animation(Game &game, int nframes);
	AnimationFrame get_frame(int x);
	void set_frame(int key, AnimationFrame aframe);
};