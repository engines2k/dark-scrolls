#pragma once
#include <map>
#include <SDL2/SDL.h>
#include "game.hpp"
#include <string>


class Game;

class AnimationFrame {
	public:
	const char *frame_path;
	const char *sound_path;
	SDL_Surface *surface;
	AnimationFrame(const char *fpath, const char *spath);
	~AnimationFrame();
};

class Animation {
	Game &game;
	std::vector<std::shared_ptr<AnimationFrame>> frames;
	int animation_l;
	public:

	Animation(Game &game, int nframes);
	const char* frame_path(int f);
	void set_frame(int key, const char *filepath, const char *spath);
	const char* sound_path(int af);
};