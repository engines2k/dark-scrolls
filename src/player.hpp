#pragma once
#include <SDL2/SDL_mixer.h>
#include "mob.hpp"
#include "animation.hpp"
#include "creep.hpp"

class Game;

class Player: public Mob {
  public:
  float speed_mod;

  Player(Game &game, Pos pos);

  virtual void add_colliders() override;

  bool is_immobile() const {
    return this->IMMOBILE_FLAG;
  }

  void immobile(bool b) {
    this->IMMOBILE_FLAG = b;
  }

  bool switch_animation(int anim_idx);
  virtual void draw();
  virtual void tick();
  virtual void death();

  private:
  std::shared_ptr<Creep> test_creep; // FOR DEMONSTRATION PURPOSES
  std::vector<Animation> animations;
  bool IMMOBILE_FLAG = false;
  bool moving = false;
  uint32_t speed;

  SDL_Surface *surface = nullptr;
  SDL_Texture *texture = nullptr;
  static constexpr uint8_t RED = 126;
  static constexpr uint8_t GREEN = 219;
  static constexpr uint8_t BLUE = 222;
};

