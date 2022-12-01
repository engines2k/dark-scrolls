#include "animation.hpp"
#include "game.hpp"
#include "media_manager.hpp"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <iostream>

AnimationFrame::AnimationFrame(int fn, const char *frame_path,
                               const char *sound) {
  this->frame_path = frame_path;
  this->sound = sound;
  this->sprite_offset = {0, 0};
  this->velocity = 0;
}

AnimationFrame::AnimationFrame(int fn, const char *frame_path,
                               const char *sound, Translation sprite_offset) {
  this->frame_path = frame_path;
  this->sound = sound;
  this->sprite_offset = {sprite_offset.x * SUBPIXELS_IN_PIXEL,
                         sprite_offset.y * SUBPIXELS_IN_PIXEL};
  this->velocity = 0;
}

AnimationFrame::AnimationFrame(int fn, const char *frame_path,
                               const char *sound, Translation sprite_offset,
                               int velocity) {
  this->frame_path = frame_path;
  this->sound = sound;
  this->sprite_offset = {sprite_offset.x * SUBPIXELS_IN_PIXEL,
                         sprite_offset.y * SUBPIXELS_IN_PIXEL};
  this->velocity = velocity;
}

AnimationFrame::~AnimationFrame() {}

Animation::Animation(Game &game, int nframes, int l = 0) : game(game) {
  if (l == 0)
    loops = false;
  else
    loops = true;

  current_frame = 0;
  start_tick = game.frame_counter.rendered_frames;
  animation_l = nframes;
}

Animation::Animation(const Animation &a) : game(a.game) {
  current_frame = 0;
  start_tick = game.frame_counter.rendered_frames;
  frames = a.frames;
  animation_l = a.animation_l;
  loops = a.loops;
}

SDL_Texture *Animation::play() {
  int frame_index =
      ((int)game.frame_counter.rendered_frames - start_tick) % animation_l;
  if (frames.find(frame_index) != frames.end()) {
    current_frame = frame_index;
    const char *p = frames[current_frame]->sound;

    if (strcmp(p, "NOSOUND")) {
      Mix_Chunk *s = game.media.readWAV(p);
      Mix_PlayChannel(-1, s, 0);
    }
  }

  SDL_Texture *tex = game.media.readTexture(frames[current_frame]->frame_path);

  return tex;
}

void Animation::set_frame(int fn, const char *fpath, const char *spath) {
  frames.emplace(fn, std::make_shared<AnimationFrame>(fn, fpath, spath));
}

void Animation::set_frame(int fn, const char *fpath, const char *spath,
                          Translation sprite_offset) {
  frames.emplace(
      fn, std::make_shared<AnimationFrame>(fn, fpath, spath, sprite_offset));
}

void Animation::set_frame(int fn, const char *fpath, const char *spath,
                          Translation sprite_offset, int velocity) {
  frames.emplace(fn, std::make_shared<AnimationFrame>(fn, fpath, spath,
                                                      sprite_offset, velocity));
}

void Animation::set_frame_reactors(int fn, std::vector<ReactorCollideBox> r) {
  frames[fn]->reactors = r;
}

void Animation::set_frame_activators(int fn,
                                     std::vector<ActivatorCollideBox> a) {
  frames[fn]->activators = a;
}

AnimationFrame Animation::frame() { return *frames[current_frame]; }

std::vector<ReactorCollideBox> Animation::frame_reactors() {
  int frame_index =
      ((int)game.frame_counter.rendered_frames - start_tick) % animation_l;
  if (frames.find(frame_index) != frames.end()) {
    return frames[frame_index]->reactors;
  }

  std::vector<ReactorCollideBox> empty;
  return empty;
}

std::vector<ActivatorCollideBox> Animation::frame_activators() {
  if (frames.find(current_frame) != frames.end()) {
    // std::cout << "returning " << frames[current_frame]->activators.size() <<
    // " for frame index " << current_frame << std::endl;
    return frames[current_frame]->activators;
  }

  std::vector<ActivatorCollideBox> empty;
  return empty;
}

void Animation::add_activator(int fn, ActivatorCollideBox box) {
  frames[fn]->activators.push_back(box);
}

void Animation::add_reactor(int fn, ReactorCollideBox box) {
  frames[fn]->reactors.push_back(box);
}

void Animation::reset() {
  current_frame = 0;
  start_tick = game.frame_counter.rendered_frames;
}

bool Animation::is_over() {
  if (!loops) {
    // std::cout << (int)game.frame_counter.rendered_frames << " - " <<
    // start_tick << " > " << animation_l << std::endl;
    return (int)game.frame_counter.rendered_frames - start_tick >=
           animation_l - 1;
  }
  return false;
}
