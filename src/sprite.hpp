#pragma once
#include "pos.hpp"
#include "collide.hpp"
#include <vector>

static uint64_t NEXT_SPRITE_ID = 0;

class Game;

class Sprite {
  public:
  Sprite(Game &game, Pos pos): game(game) {
    NEXT_SPRITE_ID++;
    this->pos = pos;
  }

  virtual void draw() = 0;
  virtual void tick() {}
  Pos get_pos() const {
    return pos;
  }
  void set_pos(Pos pos) {
    this->pos = pos;
  }
  void despawn() {
    spawn_flag = false;
  }
  bool is_spawned() const {
    return spawn_flag;
  }
  void move(Translation trans) {
    Translation x_axis = trans;
    x_axis.y = 0;
    Translation y_axis = trans;
    y_axis.x = 0;
    move_single_axis(x_axis);
    move_single_axis(y_axis);
  }

  virtual ~Sprite() {}

  const int id = NEXT_SPRITE_ID;

  protected:
  Pos pos;
  bool spawn_flag = true;
  Game &game;
  std::vector<ReactorCollideBox> reactors;
  private:
  void move_single_axis(Translation trans);
};

