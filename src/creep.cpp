#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdlib.h>
#include "game.hpp"
#include "creep.hpp"
#include "mob.hpp"

Creep::Creep(Game &game, Pos pos): Mob(game, pos) {

  Animation walk(game, 120 /* Animation length */, 1 /* Loops?*/);
  walk.set_frame(0, "data/sprite/clacker_walk000.png", "NOSOUND");
  walk.set_frame(16, "data/sprite/clacker_walk001.png", "NOSOUND");
  walk.set_frame(30, "data/sprite/clacker_walk002.png", "NOSOUND");
  walk.set_frame(40, "data/sprite/clacker_walk003.png", "NOSOUND");
  walk.set_frame(58, "data/sprite/clacker_walk004.png", "NOSOUND");
  walk.set_frame(84, "data/sprite/clacker_walk005.png", "NOSOUND");
  walk.set_frame(109, "data/sprite/clacker_walk006.png", "NOSOUND");

  Animation die(game, 60, 0);
  die.set_frame(0, "data/sprite/clacker_die000.png", "data/sound/creep_deathslash.wav");
  die.set_frame(10, "data/sprite/clacker_die001.png", "data/sound/creep_death.wav");
  die.set_frame(20, "data/sprite/clacker_die002.png", "NOSOUND");
  die.set_frame(30, "data/sprite/clacker_die003.png", "NOSOUND");
  die.set_frame(40, "data/sprite/clacker_die004.png", "NOSOUND");
  die.set_frame(50, "data/sprite/clacker_die005.png", "NOSOUND");

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
  animations.push_back(walk);
  animations.push_back(die);
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
  dead = true;
  switch_animation(1);
  if(animations[1].is_over())  // Play death animation before despawning.
    despawn();
}

void Creep::attack() {
  switch_animation(2);
}

void Creep::tick() {
  set_activators(animations[current_animation_index].frame_activators());
  Mob::tick();

  for(auto hbox: activators) {
      game.collide_layers[0].add_activator(hbox, pos);
  }

  if(dead) return; 

  if(animations[current_animation_index].is_over())
    switch_animation(0);                    // Switch to patrol / idle if done with action

  if(rand() % 300 == 1)                     // 1/60 Chance of attacking
    attack();

  else if(current_animation_index != 2) {   // Patrol if not attacking
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