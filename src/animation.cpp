#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include "game.hpp"
#include "animation.hpp"


AnimationFrame::AnimationFrame(const char *fpath, const char *spath)
{
	frame_path = fpath;
	sound_path = spath;
}
AnimationFrame::~AnimationFrame() {
}

Animation::Animation(Game &game, int nframes) : game(game)
{
	for(int i=0;i<animation_l;i++)
		frames.push_back(std::make_shared<AnimationFrame>("NOFRAME", "NOSOUND"));
}

const char* Animation::sound_path(int af) {
	return frames[af]->sound_path;
}

const char* Animation::frame_path(int af) {
	return frames[af]->frame_path;
}

void Animation::set_frame(int af, const char *fpath, const char *spath)
{
	if(frames[af]->surface != NULL) printf("Frame %d was already set! Replacing.", af);
	frames[af] = std::make_shared<AnimationFrame>(fpath, spath);
}

