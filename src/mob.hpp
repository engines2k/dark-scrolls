#pragma once
#include <SDL2/SDL.h>
#include "pos.hpp"
#include "level.hpp"
#include "sprite.hpp"

class Game;

class Mob: public Sprite {
  public:
  Mob(Game &game, Pos pos): Sprite(game, pos) {
    show_health = true;
  }

  int get_health() const  {
    return hp;
  }

  void damage(int dmg) {
    if(hp - dmg < 0) hp = 0;
    else hp -= dmg;
  }

  void heal(int healing) {
    if(hp + healing > max_hp) hp = max_hp;
    else hp += healing;
  }

  void set_health(int n_hp) {
    hp = n_hp;
  }

  virtual void death() {
    despawn();
  }
  
  virtual void tick();
  virtual void draw(SDL_Rect shape);

  protected:
  bool show_health;
  const int max_hp = 100; // This should be changed to initialize in the constructor later.
  int hp = 80;

  virtual ~Mob () {
  }
};

