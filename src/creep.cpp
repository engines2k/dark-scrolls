#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "game.hpp"
#include "creep.hpp"
#include "mob.hpp"

Creep::Creep(Game &game, Pos pos): Mob(game, pos) {
  speed = (200 * FRAME_RATE) * SUBPIXELS_IN_PIXEL;
  this->og_pos = pos;
}

void Creep::draw() {
  SDL_Rect rect = shape;
  Pos screen_pos = game.screen_pos(pos);
  rect.x = screen_pos.pixel_x();
  rect.y = screen_pos.pixel_y();

  SDL_SetRenderDrawColor(game.renderer, 255, 0, 0, 255);
  SDL_RenderFillRect(game.renderer, &rect);
}


void Creep::tick() {
  Mob::tick();
  if(pos.y == og_pos.y && returning)
    returning = !returning;
  if(abs(og_pos.y - pos.y) / SUBPIXELS_IN_PIXEL >= 200)
    returning = true;
  if(abs(og_pos.y - pos.y) / SUBPIXELS_IN_PIXEL < 200 && !returning)
    pos.y += speed;
  else 
    pos.y += -speed;
}



