#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdlib.h>
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

  Animation attack(game, 60, 0);
  attack.set_frame(0, "data/sprite/clacker_attack000.png", "data/sound/poison_bloom.wav");
  attack.set_frame(20, "data/sprite/clacker_attack001.png", "NOSOUND");
  attack.set_frame(40, "data/sprite/clacker_attack000.png", "NOSOUND");


  ActivatorCollideBox hitbox(
    //The hitbox overlaps player to hit_good is required
    ActivatorCollideType::HIT_GOOD,
    32 * SUBPIXELS_IN_PIXEL,
    0 * SUBPIXELS_IN_PIXEL,
    32 * SUBPIXELS_IN_PIXEL,
    0 * SUBPIXELS_IN_PIXEL
  );

  CollideDamageProps damage;
  damage.hp_delt = 1;
  hitbox.damage = damage;
  attack.add_activator(0, hitbox);
  attack.add_activator(20, hitbox);
  attack.add_activator(40, hitbox);
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

// Merge with Player::switch animation creates strange errror
// Merge/fix later
bool Creep::switch_animation(int new_animation_index) {
  // WIP : Needs to return false if the animation currently playing cannot be interrupted.
  bool successful;
    if(new_animation_index != current_animation_index){
      animations[current_animation_index].reset();
      animations[new_animation_index].reset();
      current_animation_index = new_animation_index;
    }
    successful = true;
  
  return successful;
}

void Creep::add_colliders() {
  Sprite::add_colliders();
  //FIXME: Hack to allow hitbox
    
    for(auto hbox: activators)
      game.collide_layers[0].add_activator(hbox, pos);
}

void Creep::death() {
  Mix_Chunk *s = game.media.readWAV("data/sound/creep_death.wav");
  Mix_PlayChannel(-1, s, 0);
  despawn();
}

void Creep::attack() {
  switch_animation(1);
}

void Creep::tick() {
  set_activators(animations[current_animation_index].frame_activators());
  Mob::tick();
  for(auto hbox: activators) {
      game.collide_layers[0].add_activator(hbox, pos);
  }

  if(animations[current_animation_index].is_over())
    switch_animation(0);                    // Switch to patrol / idle if done with action

  if(rand() % 300 == 1)                     // 1/60 Chance of attacking
    attack();

  else if(current_animation_index != 1) {   // Patrol if not attacking
    patrol();
  }
}

void Creep::draw() {

  Mob::draw(shape);
  SDL_Rect rect = shape;
  rect.x = pos.x;
  rect.y = pos.y;

  texture = animations[current_animation_index].play();
  game.camera->render_ex(game.renderer, texture, NULL, &rect, 0, NULL, SDL_FLIP_NONE);
}