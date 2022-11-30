#include "collide.hpp"

ActivatorCollideType ActivatorCollideType::operator&(const ActivatorCollideType &other) const {
 return this->BitFlag<ActivatorCollideType>::operator&(other);
}

ReactorCollideType ReactorCollideType::operator&(const ReactorCollideType &other) const {
    return this->BitFlag<ReactorCollideType>::operator&(other);
}

ReactorCollideType ActivatorCollideType::activates() const {
  return ReactorCollideType(inner);
}

ActivatorCollideType ReactorCollideType::activated_by() const {
  return ActivatorCollideType(inner);
}

ActivatorCollideType ActivatorCollideType::operator&(ReactorCollideType counter_part) const {
  return *this & counter_part.activated_by();
}

ReactorCollideType ReactorCollideType::operator&(ActivatorCollideType counter_part) const {
  return *this & counter_part.activates();
}

// Collide Layer
CollideLayer::CollideLayer() {
  for (int i = 0; i < MAX_COLLIDE_Y; i++) {
    std::vector<std::vector<ActivatorCollideBox>> row;
    row.resize(MAX_COLLIDE_X);
    colliders.push_back(std::move(row));
  }
}

void CollideLayer::clear() {
  for (auto &row : colliders) {
    for (auto &tile : row) {
      tile.clear();
    }
  }
}

void CollideLayer::add_activator(ActivatorCollideBox activator, Pos here) {
  here.x += activator.offset_x;
  here.y += activator.offset_y;
  activator.offset_x = 0;
  activator.offset_y = 0;
  int end_x = here.x + activator.width;
  int end_y = here.y + activator.height;

  walk_tiles(activator, here, [&](Pos collide_visit) {
    int tile_x = collide_visit.tile_x();
    int tile_y = collide_visit.tile_y();

    int height = activator.height;
    int width = activator.width;
    if (height > TILE_SUBPIXEL_SIZE) {
      height = TILE_SUBPIXEL_SIZE;
    }
    if (width > TILE_SUBPIXEL_SIZE) {
      width = TILE_SUBPIXEL_SIZE;
    }
    if (collide_visit.x + width > end_x) {
      width = end_x - collide_visit.x;
    }
    if (collide_visit.y + height > end_y) {
      height = end_y - collide_visit.y;
    }
    ActivatorCollideBox new_hitbox = activator;
    new_hitbox.width = width;
    new_hitbox.height = height;
    new_hitbox.offset_x = collide_visit.x - collide_visit.tile_scaled_x();
    new_hitbox.offset_y = collide_visit.y - collide_visit.tile_scaled_y();
    colliders[tile_y][tile_x].push_back(new_hitbox);

    return true;
  });
}

bool CollideLayer::overlaps_activator(ReactorCollideBox react, Pos here,
                        Pos *collide_out,
                        ActivatorCollideBox *activator_out) {
  // A reactor without a type cannot overlap any activator
  if (react.type == 0) {
    return false;
  }

  here.x += react.offset_x;
  here.y += react.offset_y;
  react.offset_x = 0;
  react.offset_y = 0;

  bool ret = false;
  walk_tiles(react, here, [&](Pos collide_visit) {
    int tile_x = collide_visit.tile_x();
    int tile_y = collide_visit.tile_y();

    const std::vector<ActivatorCollideBox> &current_tile =
        colliders[tile_y][tile_x];

    Pos aligned_there = {.layer = 0,
                          .x = collide_visit.tile_scaled_x(),
                          .y = collide_visit.tile_scaled_y()};
    for (auto &activ : current_tile) {
      if (react.collides_with(here, activ, aligned_there)) {
        if (collide_out) {
          *collide_out = collide_visit;
        }
        if (activator_out) {
          *activator_out = activ;
        }
        ret = true;
        return false;
      }
    }
    return true;
  });
  return ret;
}

static void do_nothing_on_recoil(Pos here, ReactorCollideBox reactor) {}

const OnCollideRecoilFn DO_NOTHING_ON_COLLIDE_RECOIL = &do_nothing_on_recoil;
