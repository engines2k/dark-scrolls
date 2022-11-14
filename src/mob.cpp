#include "mob.hpp"
#include "game.hpp"

#include <cstdio>

void Mob::tick() {
  Sprite::tick();
  // Get hurt when overlapping hurtbox
  for (auto& reactor: reactors) {
    ReactorCollideBox hurt_reactor = reactor;
    hurt_reactor.type &= ReactorCollideType::HURT_BY_ANY;

    ActivatorCollideBox activator;
    Pos collide_at;
    if (game.collide_layers[pos.layer].overlaps_activator(hurt_reactor, pos, &collide_at, &activator)) {
      hp -= activator.damage.hp_delt;
      activator.on_recoil(collide_at, hurt_reactor);
    }
  }
  if(hp < 1) despawn();
}

void Mob::draw(SDL_Rect shape) {
  // Draw health bar
  if(show_health) {
    if(hp == 0 ) return;
    int bar_width = 30, bar_height = 4;
    int offset = (shape.w - bar_width) / 2 * SUBPIXELS_IN_PIXEL;
    int centered_x = pos.x + offset;
    double h_ratio = static_cast<float>(hp) / max_hp;

    SDL_Rect bar_green = {  centered_x,
                            pos.y - (10 * SUBPIXELS_IN_PIXEL), 
                            static_cast<int>(bar_width*h_ratio),
                            bar_height};

    SDL_Rect bar_red = {    centered_x + (bar_green.w * SUBPIXELS_IN_PIXEL),
                            pos.y - 10 * SUBPIXELS_IN_PIXEL,
                            static_cast<int>(bar_width*(1 - h_ratio)),
                            bar_height};

    SDL_SetRenderDrawColor(game.renderer, 255, 0, 0, 255);
    game.camera->fill_rect(game.renderer, &bar_red);
    SDL_SetRenderDrawColor(game.renderer, 0, 255, 0, 255);
    game.camera->fill_rect(game.renderer, &bar_green);

  }

}
