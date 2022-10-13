#include "game.hpp"
#include <iostream>

namespace mob_vars {

double FRAME_RATE;
int SUBPIXELS_IN_PIXEL;

}

int Mob_Init(const double frames, const int subpixels) {
  // if(frames < 0 || subpixels < 0) return -1;

  if(!mob_vars::FRAME_RATE && !mob_vars::SUBPIXELS_IN_PIXEL) {
    mob_vars::FRAME_RATE = frames;
    mob_vars::SUBPIXELS_IN_PIXEL = subpixels;
    return 0;
  } else return -1;
}

void Sprite::move_single_axis(Translation trans) {
  if (trans.x == 0 && trans.y == 0) {
    return;
  }
  pos += trans;
  Pos collide_visit = pos;

  int end_x = pos.x + hitbox.width;
  int end_y = pos.y + hitbox.height;

  bool last_row = false;
  while (true) {
    Tile& current_tile = game.current_level[collide_visit];

    if (current_tile.props().collide_type == TileCollideType::WALL) {
      if (trans.x < 0) {
        pos.x = collide_visit.tile_scaled_x() + TILE_SUBPIXEL_SIZE + SUBPIXELS_IN_PIXEL;
      }
      if (trans.x > 0) {
        pos.x = collide_visit.tile_scaled_x() - hitbox.width - SUBPIXELS_IN_PIXEL;
      }
      if (trans.y < 0) {
        pos.y = collide_visit.tile_scaled_y() + TILE_SUBPIXEL_SIZE + SUBPIXELS_IN_PIXEL;
      }
      if (trans.y > 0) {
        pos.y = collide_visit.tile_scaled_y() - hitbox.height - SUBPIXELS_IN_PIXEL;
      }
      break;
    }

    if (collide_visit.x == end_x) {
      collide_visit.x = pos.x;
      collide_visit.y += TILE_SUBPIXEL_SIZE;

      if (last_row) {
        break;
      }
      if (collide_visit.y >= end_y) {
        collide_visit.y = end_y;
        last_row = true;
      }
    } else {
      collide_visit.x += TILE_SUBPIXEL_SIZE;
      if (collide_visit.x > end_x) {
        collide_visit.x = end_x;
      }
    }
  }
}
