#pragma once
#include "collide.hpp"
#include "pos.hpp"
#include <memory>
#include <vector>

static uint64_t NEXT_SPRITE_ID = 0;

class Game;

class Sprite : public std::enable_shared_from_this<Sprite> {
public:
  Sprite(Game &game, Pos pos);

  virtual void draw() = 0;
  virtual void tick() {}
  bool switch_animation(int anim_idx);
  virtual void add_colliders();
  virtual void set_reactors(std::vector<ReactorCollideBox> r);
  virtual void set_activators(std::vector<ActivatorCollideBox> a);
  virtual bool never_paused();

  Pos get_pos() const;
  void set_pos(Pos pos);
  void despawn();
  bool is_spawned() const;
  bool move(Translation trans);

  virtual ~Sprite() {}

  const int id = NEXT_SPRITE_ID;

protected:
  static constexpr SDL_Rect SHAPE = {.x = 0, .y = 0, .w = 64, .h = 64};
  int current_animation_index;
  Pos pos;
  bool spawn_flag = true;
  bool facing_left = false;
  Game &game;
  std::vector<ReactorCollideBox> reactors;
  std::vector<ActivatorCollideBox> activators;

private:
  bool move_single_axis(Translation trans);
};
