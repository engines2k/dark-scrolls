#include "game.hpp"
#include "pos.hpp"
#include <iostream>

Sprite::Sprite(Game &game, Pos pos) : game(game) {
  current_animation_index = 0;
  NEXT_SPRITE_ID++;
  this->pos = pos;
}

Pos Sprite::get_pos() const { return pos; }

void Sprite::set_pos(Pos pos) { this->pos = pos; }

void Sprite::despawn() { spawn_flag = false; }

bool Sprite::is_spawned() const { return spawn_flag; }

// Returns true if movement occured
bool Sprite::move(Translation trans) {
    Translation x_axis = trans;
    x_axis.y = 0;
    Translation y_axis = trans;
    y_axis.x = 0;
    bool moved_x = move_single_axis(x_axis);
    bool moved_y = move_single_axis(y_axis);
    return moved_x || moved_y;
  }

// Returns true if movement occured
bool Sprite::move_single_axis(Translation trans) {
  if (trans.x == 0 && trans.y == 0) {
    return false;
  }
  Pos orig_pos = pos;
  pos += trans;
  for (auto &reactor : reactors) {
    ReactorCollideBox wall_reactor = reactor;
    wall_reactor.type &= ReactorCollideType::WALL;

    Pos collide_visit;
    ActivatorCollideBox activator;
    if (game.collide_layers[pos.layer].overlaps_activator(
            wall_reactor, pos, &collide_visit, &activator)) {
      activator.on_recoil(collide_visit, wall_reactor);
      int base_x =
          collide_visit.tile_scaled_x() + activator.offset_x - reactor.offset_x;
      int base_y =
          collide_visit.tile_scaled_y() + activator.offset_y - reactor.offset_y;

      if (trans.x < 0) {
        pos.x = base_x + activator.width + SUBPIXELS_IN_PIXEL;
      }
      if (trans.x > 0) {
        pos.x = base_x - reactor.width - SUBPIXELS_IN_PIXEL;
      }
      if (trans.y < 0) {
        pos.y = base_y + activator.height + SUBPIXELS_IN_PIXEL;
      }
      if (trans.y > 0) {
        pos.y = base_y - reactor.height - SUBPIXELS_IN_PIXEL;
      }
      return orig_pos != pos;
    }
  }
  return true;
}

void Sprite::add_colliders() {
  for (auto &activator : activators) {
    if (facing_left) {
      activator.offset_x = (SHAPE.w * SUBPIXELS_IN_PIXEL) -
                           (activator.offset_x + activator.width);
    }
    // TODO: Assumes collide layer is always zero
    game.collide_layers[0].add_activator(activator, pos);
  }

}

void Sprite::set_activators(std::vector<ActivatorCollideBox> a) { activators = a; }

void Sprite::set_reactors(std::vector<ReactorCollideBox> r) { reactors = r; }

bool Sprite::never_paused() { return false; }
