#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "game.hpp"
#include "creep.hpp"
#include "mob.hpp"

Creep::Creep(Game &game, Pos pos): Mob(game, pos) {
  Animation walk(game, 60);
  walk.set_frame(0, "img/clacker000.png", "NOSOUND");
  walk.set_frame(30, "img/clacker001.png", "NOSOUND");
  // walk.set_frame(24, "img/clacker002.png", "NOSOUND");
  // walk.set_frame(36, "img/clacker003.png", "NOSOUND");
  // walk.set_frame(48, "img/clacker004.png", "NOSOUND");
  animations.push_back(walk);

  speed = ((rand() % 20 + 30) * FRAME_RATE) * SUBPIXELS_IN_PIXEL;
  this->og_pos = pos;
}

void Creep::draw() {
  SDL_RendererFlip flip = SDL_FLIP_NONE;

  SDL_Rect rect = shape;
  Pos screen_pos = game.screen_pos(pos);
  rect.x = screen_pos.pixel_x();
  rect.y = screen_pos.pixel_y();

  texture = animations[0].play();
  SDL_RenderCopyEx(game.renderer, texture, NULL, &rect, 0, NULL, flip);
}


void Creep::tick() {
  Mob::tick();
  if(pos.y == og_pos.y && returning)
    returning = !returning;
  if(abs(og_pos.y - pos.y) / SUBPIXELS_IN_PIXEL >= 140)
    returning = true;
  if(abs(og_pos.y - pos.y) / SUBPIXELS_IN_PIXEL < 140 && !returning)
    pos.y += speed;
  else 
    pos.y += -speed;
}



