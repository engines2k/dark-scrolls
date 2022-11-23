#pragma once

#include "item.hpp"

class HealthPotion : public Item {
public:
  HealthPotion(Game &game, Pos pos);
  virtual void draw();
  virtual void tick();
  void set_heal_amount(int num);

protected:
  int heal_amount = 15;
};

class SpeedPotion : public Item {
public:
  SpeedPotion(Game &game, Pos pos);
  virtual void draw();
  virtual void tick();
  void set_speed_mod(int num);

protected:
  int effect_duration =
      15; // FIXME - Need to stop effects after duration time. In Player?
  float speed_mod = 100;
};