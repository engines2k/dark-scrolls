 #pragma once
#include <map>
#include <SDL2/SDL.h>
#include "game.hpp"
#include "collide.hpp"

class Game;

struct AnimationFrame {
	public:
	int frame_number;
	const char *frame_path, *sound;
	SDL_Texture *texture;
	AnimationFrame(int fn, const char *fpath, const char *spath);
	~AnimationFrame();
  	std::vector<ReactorCollideBox> reactors;
 	std::vector<ActivatorCollideBox> activators;
};

class Animation {
	protected:
	Game &game;
	bool loops;
	int animation_l;
	int32_t start_tick, current_frame;
	std::map<int, std::shared_ptr<AnimationFrame>> frames;
	
	public:
	Animation(Game &game, int nframes, int l);
	Animation(const Animation &a);

	SDL_Texture * play();
	SDL_Rect rect;
	void set_frame(int fn, const char *fpath, const char *spath);
	void set_frame_reactors(int fn, std::vector<ReactorCollideBox> r);
	void set_frame_activators(int fn, std::vector<ActivatorCollideBox> a);
	SDL_Texture * get_frame();
	std::vector<ReactorCollideBox> frame_reactors();
	std::vector<ActivatorCollideBox> frame_activators();
	void add_reactor(int fn, ReactorCollideBox box);
	void add_activator(int fn, ActivatorCollideBox box);
	void reset();
	bool is_over();
};