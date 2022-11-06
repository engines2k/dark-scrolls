#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include "game.hpp"
#include "animation.hpp"
#include "MediaManager.hpp"

#include <iostream>

AnimationFrame::AnimationFrame(int fn, const char *fpath, const char *spath)
{
	frame_path = fpath;
	sound = spath;
}
AnimationFrame::~AnimationFrame() {
}

Animation::Animation(Game &game, int nframes) : game(game)
{
	current_frame_index = 0;
	start_tick = game.frame_counter.rendered_frames;
	animation_l = nframes;
}

Animation::Animation(const Animation &a) : game(a.game) {
	current_frame_index = 0;
	start_tick = game.frame_counter.rendered_frames;
	frames = a.frames;
	animation_l = a.animation_l;
}

SDL_Texture* Animation::play() {
	int frame_index = ((int)game.frame_counter.rendered_frames - start_tick) % animation_l;
	if(frames.find(frame_index) != frames.end()) {
		current_frame_index = frame_index;
		const char *p = frames[current_frame_index]->sound;
		
		if (strcmp(p, "NOSOUND")) {
			Mix_Chunk *s = mediaManager.readWAV(p);
			Mix_PlayChannel(-1, s, 0);
		}
	}

    SDL_Texture *tex = mediaManager.readIMG(game.renderer, frames[current_frame_index]->frame_path, rect);
	return tex;
}


void Animation::set_frame(int fn, const char *fpath, const char *spath)
{
	frames.emplace(fn, std::make_shared<AnimationFrame>(fn, fpath, spath));
}

SDL_Texture *Animation::get_frame() {
	SDL_Texture *texture = mediaManager.readIMG(game.renderer, frames[current_frame_index]->frame_path, rect);
	return texture;
}

void Animation::reset() {
	current_frame_index = 0;
}