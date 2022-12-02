#pragma once
#include "collide.hpp"
#include "pos.hpp"
#include <SDL2/SDL.h>
#include <map>
#include <memory>

class Game;

struct AnimationFrame {
public:
  int frame_number, velocity;
  const char *frame_path, *sound;
  SDL_Texture *texture;
  Translation sprite_offset;
  AnimationFrame(int fn, const char *fpath, const char *spath);
  AnimationFrame(int fn, const char *frame_path, const char *sound,
                 Translation sprite_offset);
  AnimationFrame(int fn, const char *frame_path, const char *sound,
                 Translation sprite_offset, int velocity);
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
  // This file is a mess. Needs:
  // Constructor cleanup
  // Method consolidation
  std::vector<ReactorCollideBox> cur_reactors;

public:
  Animation(Game &game, int nframes, int l);
  Animation(const Animation &a);

  SDL_Texture *play();
  SDL_Rect rect;
  void set_frame(int fn, const char *fpath, const char *spath);
  void set_frame(int fn, const char *fpath, const char *spath,
                 Translation sprite_offset);
  void set_frame(int fn, const char *fpath, const char *spath,
                 Translation sprite_offset, int velocity);
  void set_frame_reactors(int fn, std::vector<ReactorCollideBox> r);
  void set_frame_activators(int fn, std::vector<ActivatorCollideBox> a);
  AnimationFrame frame();
  std::vector<ReactorCollideBox> current_reactors();
  std::vector<ActivatorCollideBox> frame_activators();
  void add_reactor(int fn, ReactorCollideBox box);
  void add_activator(int fn, ActivatorCollideBox box);

  void reset();
  bool is_over();
};
