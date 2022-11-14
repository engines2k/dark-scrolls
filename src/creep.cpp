#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "game.hpp"
#include "creep.hpp"
#include "mob.hpp"

Creep::Creep(Game &game, Pos pos): Mob(game, pos) {
  Animation walk(game, 60, 1);
  walk.set_frame(0, "data/sprite/clacker000.png", "NOSOUND");
  walk.set_frame(30, "data/sprite/clacker001.png", "NOSOUND");
  // walk.set_frame(24, "data/sprite/clacker002.png", "NOSOUND");
  // walk.set_frame(36, "data/sprite/clacker003.png", "NOSOUND");
  // walk.set_frame(48, "data/sprite/clacker004.png", "NOSOUND");
  animations.push_back(walk);

  Animation attack(game, 30, 0);
  attack.set_frame(0, "data/sprite/clacker_attack000.png", "NOSOUND");
  attack.set_frame(15, "data/sprite/clacker_attack001.png", "NOSOUND");

  ActivatorCollideBox hitbox(
    //The hitbox overlaps player to hit_evil is required
    ActivatorCollideType::HIT_EVIL,
    32 * SUBPIXELS_IN_PIXEL,
    0 * SUBPIXELS_IN_PIXEL,
    32 * SUBPIXELS_IN_PIXEL,
    0 * SUBPIXELS_IN_PIXEL
  );

  CollideDamageProps damage;
  damage.hp_delt = 2;
  hitbox.damage = damage;
  attack.add_activator(0, hitbox);
  animations.push_back(attack);


  // FIXME: Placeholder reactor
  // Not accurate to the creep's model
  int hurtbox_width = 32 * SUBPIXELS_IN_PIXEL;
  int hurtbox_offset_x = 16 * SUBPIXELS_IN_PIXEL;
  int hurtbox_height = 54 * SUBPIXELS_IN_PIXEL;
  int hurtbox_offset_y = 8 * SUBPIXELS_IN_PIXEL;

  ReactorCollideBox hurtbox(
      ReactorCollideType::WALL | ReactorCollideType::HURT_BY_GOOD,
      hurtbox_offset_x,
      hurtbox_width,
      hurtbox_offset_y,
      hurtbox_height
  );



  reactors.push_back(std::move(hurtbox));

  speed = ((rand() % 20 + 30) * FRAME_RATE) * SUBPIXELS_IN_PIXEL;
  this->og_pos = pos;
}

void Creep::patrol() {
  if(pos.y == og_pos.y && returning)
    returning = !returning;
  if(abs(og_pos.y - pos.y) / SUBPIXELS_IN_PIXEL >= 140)
    returning = true;
  if(abs(og_pos.y - pos.y) / SUBPIXELS_IN_PIXEL < 140 && !returning)
    pos.y += speed;
  else 
    pos.y += -speed;
}

void Creep::attack() {
  // switch_animation(1);
}

void Creep::draw() {
  Mob::draw();
  SDL_Rect rect = shape;
  rect.x = pos.x;
  rect.y = pos.y;

  texture = animations[0].play();
  game.camera->render_ex(game.renderer, texture, NULL, &rect, 0, NULL, SDL_FLIP_NONE);
}

void Creep::tick() {
  Mob::tick();
  patrol();
}



