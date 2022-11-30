#pragma once
#include "level.hpp"
#include "pos.hpp"
#include "sprite.hpp"
#include <SDL2/SDL.h>

class Game;

class Mob : public Sprite {
public:
  Mob(Game &game, Pos pos);

  int get_health() const;
  void take_damage(int dmg);
  void heal(int healing);
  void set_health(int n_hp);
  virtual void death();

  virtual void tick();
  virtual void draw(SDL_Rect shape);

protected:
  bool show_health;
  bool dead = false;
  const int max_hp =
      100; // This should be changed to initialize in the constructor later.
  int hp = max_hp;

  virtual ~Mob() {}
};
