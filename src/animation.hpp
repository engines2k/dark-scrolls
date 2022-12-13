#pragma once
#include <map>
#include <memory>
#include <SDL2/SDL.h>
#include <nlohmann/json.hpp>
#include "pos.hpp"
#include "collide.hpp"
#include <vector>
#include <string>

using json = nlohmann::json;

// I'M A MESS!! DON'T LOOK AT ME!!
// Constructor cleanup
// Method consolidation

class Game;

struct AnimationFrame {
  std::string frame_path, sound_path;
  int frame_number, velocity;
  Translation sprite_offset;
  SDL_Texture *texture;
  std::vector<ReactorCollideBox> reactors;
  std::vector<ActivatorCollideBox> activators;
  bool invulnerable = false;

  AnimationFrame(int fn, std::string fpath, std::string spath);
  AnimationFrame(int fn, std::string frame_path, std::string sound_path,
                 Translation sprite_offset);
  AnimationFrame(int fn, std::string frame_path, std::string sound_path,
                 Translation sprite_offset, int velocity);
  ~AnimationFrame();
};

struct Animation {
  std::string name;
  Game &game;
  bool loops, immobilizes = false;
  int animation_l, loop_start = 0;
  int32_t start_tick, current_frame;
  std::map<int, std::shared_ptr<AnimationFrame>> frames;
  std::vector<ReactorCollideBox> cur_reactors;
  std::vector<ReactorCollideBox> default_reactors;
  std::vector<ActivatorCollideBox> default_activators;

  AnimationFrame frame();
  SDL_Texture *play();
  SDL_Rect rect;
  std::vector<ReactorCollideBox> current_reactors();
  std::vector<ActivatorCollideBox> frame_activators();


  Animation(Game &game, int nframes, bool l);
  Animation(const Animation &a);

  void reset();
  bool is_over();
  void add_reactor(int fn, ReactorCollideBox box);
  void add_activator(int fn, ActivatorCollideBox box);
  void set_frame(int fn, std::string fpath, std::string spath);
  void set_frame(int fn, std::string fpath, std::string spath,
                 Translation sprite_offset);
  void set_frame(int fn, std::string fpath, std::string spath,
                 Translation sprite_offset, int velocity);
};

struct FrameData {
  Game &game; 
  std::vector<Animation> animations;
  
  FrameData(Game &game);
  ActivatorCollideBox readActivatorBox(json j);
  ReactorCollideBox readReactorBox(json j);
  void readAnimations(const char *filepath, const char *spritename);
};