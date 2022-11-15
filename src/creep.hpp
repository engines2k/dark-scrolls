#pragma once
#include <SDL2/SDL.h>
#include "mob.hpp"
#include "animation.hpp"

class Game;

// Should probably share another inheritance with player for
// mutual properties, like health and speed, etc. but I'm going to leave this for later.
class Creep: public Mob {
  public:
  Creep(Game &game, Pos pos);

  void patrol();
  void attack();
  void add_colliders();
  bool switch_animation(int anim_idx);

  void death() override;
  virtual void draw();
  virtual void tick();

  private:
  std::vector<Animation> animations;
  SDL_Surface *surface = nullptr;
  SDL_Texture *texture = nullptr;
  bool returning = false;
  Pos og_pos;
  uint32_t speed;
  SDL_Rect shape = {.x = 160, .y = -160, .w = 54, .h = 54};
};


