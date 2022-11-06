#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include "game.hpp"
#include "animation.hpp"
#include <iostream>

AnimationFrame::AnimationFrame(int fn, const char *fpath, const char *spath)
{
	frame_path = fpath;
	sound = spath;
}

AnimationFrame::~AnimationFrame()
{
}

Animation::Animation(Game &game, int nframes, int l = 0) : game(game)
{
	if(l == 0)
		loops = false;
	else
		loops = true;

	current_frame_index = 0;
	start_tick = game.frame_counter.rendered_frames;
	animation_l = nframes;
}

Animation::Animation(const Animation &a) : game(a.game)
{
	current_frame_index = 0;
	start_tick = game.frame_counter.rendered_frames;
	frames = a.frames;
	animation_l = a.animation_l;
}

SDL_Texture* Animation::play()
{
	int frame_index = ((int)game.frame_counter.rendered_frames - start_tick) % animation_l;
	if(frames.find(frame_index) != frames.end()) {
		current_frame_index = frame_index;
		const char *p = frames[current_frame_index]->sound;
		
		Mix_Chunk *s = Mix_LoadWAV(p);
	 //    if(s == nullptr && p != "NOSOUND"){
		// 	printf("Sound error: %s\n", SDL_GetError());
		// 	abort();
		// }
		Mix_PlayChannel(-1, s, 0);
	}

    SDL_Surface *surface = IMG_Load(frames[current_frame_index]->frame_path);
    SDL_Texture *tex = SDL_CreateTextureFromSurface(game.renderer, surface);
	return tex;
}


void Animation::set_frame(int fn, const char *fpath, const char *spath)
{
	frames.emplace(fn, std::make_shared<AnimationFrame>(fn, fpath, spath));
}

void Animation::reset() {
	current_frame_index = 0;
	start_tick = game.frame_counter.rendered_frames;
}

bool Animation::is_over() {
	if (!loops)
	return (int)game.frame_counter.rendered_frames - start_tick > animation_l;
	return false;
}