#pragma once
#include "collide.hpp"
#include "pos.hpp"
#include <SDL2/SDL.h>
#include <map>
#include <memory>

// This file is a mess. Needs:
// Constructor cleanup
// Method consolidation

class Game;

struct AnimationFrame {
  const char *frame_path, *sound;
  SDL_Texture *texture;
  Translation sprite_offset;
  int frame_number, velocity;
  std::vector<ReactorCollideBox> reactors;
  std::vector<ActivatorCollideBox> activators;

  AnimationFrame(int fn, const char *fpath, const char *spath);
  AnimationFrame(int fn, const char *frame_path, const char *sound,
                 Translation sprite_offset);
  AnimationFrame(int fn, const char *frame_path, const char *sound,
                 Translation sprite_offset, int velocity);
  ~AnimationFrame();
};

class Animation {
protected:
  Game &game;
  bool loops;
  int animation_l;
  int32_t start_tick, current_frame;
  std::map<int, std::shared_ptr<AnimationFrame>> frames;
  std::vector<ReactorCollideBox> cur_reactors;

public:
  AnimationFrame frame();
  SDL_Texture *play();
  SDL_Rect rect;
  std::vector<ReactorCollideBox> current_reactors();
  std::vector<ActivatorCollideBox> frame_activators();

  Animation(Game &game, int nframes, int l);
  Animation(const Animation &a);
  void reset();
  bool is_over();
  void add_reactor(int fn, ReactorCollideBox box);
  void add_activator(int fn, ActivatorCollideBox box);
  void set_frame(int fn, const char *fpath, const char *spath);
  void set_frame(int fn, const char *fpath, const char *spath,
                 Translation sprite_offset);
  void set_frame(int fn, const char *fpath, const char *spath,
                 Translation sprite_offset, int velocity);
};

struct FrameData {
  std::vector<Animation> animations;
};