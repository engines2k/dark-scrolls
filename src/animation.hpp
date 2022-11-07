#pragma once
#include <map>
#include <SDL2/SDL.h>
#include "game.hpp"
#include "collide.hpp"


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
	protected:
	Game &game;
	bool loops;
	int animation_l;
	int32_t start_tick, current_frame_index;
	std::map<int, std::shared_ptr<AnimationFrame>> frames;
	
	public:
	Animation(Game &game, int nframes, int l);
	Animation(const Animation &a);

	SDL_Texture * play();
	SDL_Rect rect;
	void set_frame(int fn, const char *fpath, const char *spath);
	SDL_Texture * get_frame();
	void reset();
	bool is_over();
};