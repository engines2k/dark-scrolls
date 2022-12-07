#pragma once

#include "media_manager.hpp"
#include "player.hpp"
#include "sprite.hpp"
#include "text.hpp"
#include <iostream>

class Item : public Sprite {
public:
  Item(Game &game, Pos p);
  virtual void draw();
  virtual void tick();
  virtual void use();
  std::string get_filename();

  Item &operator=(const Item &other);
  
protected:
  std::string filename;
  ReactorCollideBox reactbox;
  ActivatorCollideBox hitbox;
  bool player_collide = false;
  bool use_flag = false;
  static constexpr SDL_Rect SHAPE = {.x = 0, .y = 0, .w = 32, .h = 32};
};
