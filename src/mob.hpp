#pragma once
#include <SDL2/SDL.h>
#include "pos.hpp"
#include "level.hpp"
#include "game.hpp"
#include "sprite.hpp"

class Game;

class Mob: public Sprite {
  public:
  Mob(Game &game, Pos pos): Sprite(game, pos) {}

  int get_health() const  {
    return health;
  }

  void hurt (int dmg) {
    health -= dmg;
  }

  void set_health (int n_health) {
    health = n_health;
  }

  virtual void tick() {
    if(health < 0) despawn();
  }

  protected:
  int health = 100;

  virtual ~Mob () {
  }
};

