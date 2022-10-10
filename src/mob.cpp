#include "game.hpp"

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
  } else return -2;
}

void Sprite::move_single_axis(Translation trans) {
  if (trans.x == 0 && trans.y == 0) {
    return;
  }
  pos += trans;
  Pos tile_pos = Pos { .layer = pos.layer, 
    .x = pos.tile_x() * TILE_SUBPIXEL_SIZE, 
    .y = pos.tile_y() * TILE_SUBPIXEL_SIZE 
  };

  Pos bottom_tile_pos = pos;
  if (trans.x != 0) {
    bottom_tile_pos.y += hitbox.height - 1;
  }
  if (trans.y != 0) {
    bottom_tile_pos.x += hitbox.width - 1;
  }

  if (trans.x > 0) {
    tile_pos.x += TILE_SUBPIXEL_SIZE;
    bottom_tile_pos.x += TILE_SUBPIXEL_SIZE;
  }
  if (trans.y > 0) {
    tile_pos.y += TILE_SUBPIXEL_SIZE;
    bottom_tile_pos.y += TILE_SUBPIXEL_SIZE;
  }
  
  Tile& current_tile = game.current_level[tile_pos];
  Tile& bottom_tile = game.current_level[bottom_tile_pos];
  if (current_tile.props().collide_type == TileCollideType::WALL || 
      bottom_tile.props().collide_type == TileCollideType::WALL) {
    if (trans.x < 0) {
      pos.x = tile_pos.x + TILE_SUBPIXEL_SIZE;
    }
    if (trans.x > 0) {
      pos.x = tile_pos.x - hitbox.width;
    }
    if (trans.y < 0) {
      pos.y = tile_pos.y + TILE_SUBPIXEL_SIZE;
    }
    if (trans.y > 0) {
      pos.y = tile_pos.y - hitbox.height;
    }
  }
}
