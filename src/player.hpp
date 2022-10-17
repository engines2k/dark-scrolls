#pragma once
#include <SDL2/SDL_mixer.h>
#include "mob.hpp"
#include "animation.hpp"

class Game;

class Player: public Mob {
  public:
  float speed_mod;

  Player(Game &game, Pos pos);

  bool is_immobile() const {
    return this->IMMOBILE_FLAG;
  }

  void immobile(bool b) {
    this->IMMOBILE_FLAG = b;
  }

  virtual void draw();
  virtual void tick();

  private:
  std::vector<Animation> animations;
  bool IMMOBILE_FLAG = false;
  bool moving = false;
  bool facing_left = false;
  uint32_t speed;

  SDL_Surface *surface = nullptr;
  SDL_Texture *texture = nullptr;
  static constexpr SDL_Rect SHAPE = {.x = 0, .y = 0, .w = 64, .h = 64};
  static constexpr uint8_t RED = 126;
  static constexpr uint8_t GREEN = 219;
  static constexpr uint8_t BLUE = 222;
};

