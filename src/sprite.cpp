#include "game.hpp"
#include "pos.hpp"
#include <iostream>

//Returns true if movement occured
bool Sprite::move_single_axis(Translation trans) {
  if (trans.x == 0 && trans.y == 0) {
    return false;
  }
  Pos orig_pos = pos;
  pos += trans;
  for (auto& reactor: reactors) {
    ReactorCollideBox wall_reactor = reactor;
    wall_reactor.type &= ReactorCollideType::WALL;
    if (wall_reactor.type == 0) {
      continue; // If this hitbox is not effected by walls optimization
    }

    Pos collide_visit;
    ActivatorCollideBox activator;
    if (game.collide_layers[pos.layer].overlaps_activator(wall_reactor, pos, &collide_visit, &activator)) {
      int base_x = collide_visit.tile_scaled_x() + activator.offset_x - reactor.offset_x;
      int base_y = collide_visit.tile_scaled_y() + activator.offset_y - reactor.offset_y;

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
  for (auto& activator: activators) {
    Pos pos = {.layer = 0, .x = static_cast<int>(x * TILE_SUBPIXEL_SIZE), 
      .y = static_cast<int>(y * TILE_SUBPIXEL_SIZE)
  };

  //TODO: Assumes collide layer is always zero
  collide_layers[0].add_activator(activator, pos);
}

void Sprite::set_activators(std::vector<ActivatorCollideBox> a) {
  activators = a;
}

void Sprite::set_reactors(std::vector<ReactorCollideBox> r) {
  reactors = r;
}