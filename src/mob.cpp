#include "mob.hpp"
#include "game.hpp"

void Mob::tick() {
  Sprite::tick();
  // Get hurt when overlapping hurtbox
  for (auto& reactor: reactors) {
    ReactorCollideBox hurt_reactor = reactor;
    hurt_reactor.type &= ReactorCollideType::HURT_BY_ANY;
    if (hurt_reactor.type == 0) {
      continue;
    }

    ActivatorCollideBox activator;
    if (game.collide_layers[pos.layer].overlaps_activator(hurt_reactor, pos, nullptr, &activator)) {
      hp -= activator.damage.hp_delt;
    }
  }
  if(hp < 0) despawn();
}
