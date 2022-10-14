#pragma once
#include <SDL2/SDL.h>
#include "mob.hpp"

class Game;

// Should probably share another inheritance with player for
// mutual properties, like health and speed, etc. but I'm going to leave this for later.
class Creep: public Mob {
  public:
  Creep(Game &game, Pos pos);

  virtual void draw();
  virtual void tick();

  private:
  // to make it easy to check if sprites in the sprite list are creeps
  // perhaps make a separate enemy sprite list?
  static const bool creep = true;
  bool returning = false;
  Pos og_pos;
  uint32_t speed;
  SDL_Rect shape = {.x = 160, .y = -160, .w = 30, .h = 30};
};


