#pragma once
#include <SDL2/SDL.h>
#include "pos.hpp"
#include "level.hpp"
#include "sprite.hpp"

class Game;

class Mob: public Sprite {
  public:
  Mob(Game &game, Pos pos): Sprite(game, pos) {}

  int get_health() const  {
    return hp;
  }

  void damage(int dmg) {
    hp -= dmg;
  }

  void heal(int dhp) {
    hp += dhp;
  }

  void set_health(int n_hp) {
    hp = n_hp;
  }

  virtual void tick();

  protected:
  int hp = 100;

  virtual ~Mob () {
  }
};

