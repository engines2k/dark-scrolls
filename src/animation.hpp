#pragma once
#include <map>
#include <SDL2/SDL.h>
#include "game.hpp"
#include <string>


class Game;

class AnimationFrame {
	public:
	int frame_number;
	const char *frame_path, *sound;
	SDL_Texture *texture;
	AnimationFrame(int fn, const char *fpath, const char *spath);
	~AnimationFrame();
};

class Animation {
	public:
	int32_t start_tick, current_frame_index;
	Game &game;
	std::map<int, std::shared_ptr<AnimationFrame>> frames;
	int animation_l;
	Animation(Game &game, int nframes);
	Animation(const Animation &a);
	SDL_Texture * play();
	SDL_Rect rect;
	void set_frame(int fn, const char *fpath, const char *spath);
	SDL_Texture * get_frame();
	void reset();
};