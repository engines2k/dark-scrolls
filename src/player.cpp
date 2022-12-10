#include "animation.hpp"
#include "game.hpp"
#include "mob.hpp"
#include "pos.hpp"
#include "text.hpp"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <iostream>
#include <vector>

Player::Player(Game &game, Pos pos) : Mob(game, pos) {
  // FIXME: Placeholder reactor
  int hitbox_width = 32 * SUBPIXELS_IN_PIXEL;
  int hitbox_offset_x = 17 * SUBPIXELS_IN_PIXEL;
  int hitbox_height = 54 * SUBPIXELS_IN_PIXEL;
  int hitbox_offset_y = 8 * SUBPIXELS_IN_PIXEL;

  ReactorCollideBox hurtbox(
      ReactorCollideType::WALL | ReactorCollideType::HURT_BY_EVIL,
      hitbox_offset_x, hitbox_width, hitbox_offset_y, hitbox_height);

  ActivatorCollideBox hitbox(
      // The hitbox overlaps player to hit_evil is required
      ActivatorCollideType::HIT_EVIL | ActivatorCollideType::INTERACT,
      55 * SUBPIXELS_IN_PIXEL, 12 * SUBPIXELS_IN_PIXEL, 20 * SUBPIXELS_IN_PIXEL,
      19 * SUBPIXELS_IN_PIXEL);

  CollideDamageProps damage;
  damage.hp_delt = 40;
  hitbox.damage = damage;

  Animation walk(game, 48, 1);
  walk.set_frame(0, "data/sprite/player_run000.png", "NOSOUND");
  walk.add_reactor(0, hurtbox);
  walk.set_frame(6, "data/sprite/player_run001.png", "data/sound/walk.wav");
  walk.set_frame(13, "data/sprite/player_run002.png", "NOSOUND");
  walk.set_frame(20, "data/sprite/player_run003.png", "NOSOUND");
  walk.set_frame(24, "data/sprite/player_run004.png", "NOSOUND");
  walk.set_frame(30, "data/sprite/player_run005.png", "data/sound/walk.wav");
  walk.set_frame(37, "data/sprite/player_run006.png", "NOSOUND");
  walk.set_frame(44, "data/sprite/player_run007.png", "NOSOUND");

  Animation idle(game, 70, 1);
  idle.set_frame(0, "data/sprite/player_idle000.png", "NOSOUND");
  idle.add_reactor(0, hurtbox);
  idle.set_frame(35, "data/sprite/player_idle001.png", "NOSOUND");

  Animation attack(game, 30, 0);
  attack.set_frame(0, "data/sprite/player_slash000.png", "NOSOUND", {-7, 0});
  attack.add_reactor(0, hurtbox);
  attack.set_frame(7, "data/sprite/player_slash001.png", "NOSOUND", {-7, 0});
  attack.set_frame(9, "data/sprite/player_slash001.png", "data/sound/swing.wav",{-7, 0});
  attack.set_frame(14, "data/sprite/player_slash002.png", "NOSOUND", {-7, 0});
  attack.set_frame(15, "data/sprite/player_slash002.png", "NOSOUND", {-7, 0});
  attack.set_frame(23, "data/sprite/player_slash003.png", "NOSOUND", {-7, 0});
  attack.add_activator(14, hitbox);

  Animation dodge(game, 37, 0);
  dodge.set_frame(0, "data/sprite/player_dodge000.png", "data/sound/dodge.wav");
  dodge.add_reactor(0, hurtbox);
  dodge.set_frame(4, "data/sprite/player_dodge001.png", "NOSOUND", {0, 0}, 4);
  dodge.set_frame(11, "data/sprite/player_dodge003.png", "NOSOUND");
  dodge.set_frame(17, "data/sprite/player_dodge004.png", "data/sound/land.wav", {0, 0}, 1);
  dodge.set_frame(23, "data/sprite/player_dodge005.png", "NOSOUND");

  animations.push_back(idle);
  animations.push_back(walk);
  animations.push_back(attack);
  animations.push_back(dodge);

  speed = (140 * FRAME_RATE) * SUBPIXELS_IN_PIXEL;
  speed_mod = 0;
}

void Player::add_colliders() {

  std::shared_ptr<Player> self =
      std::static_pointer_cast<Player>(shared_from_this());
  for (auto &activator : activators) {

    // FIXME!
    activator.on_recoil = [self](Pos pos, ReactorCollideBox reactor) {
      Mix_Chunk *s = self->game.media.readWAV("data/sound/attack_hit.wav");
      Mix_PlayChannel(-1, s, 0);
    };
  }

  Sprite::add_colliders();
}

bool Player::switch_animation(int new_animation_index) {
  // WIP : Needs to return false if the animation currently playing cannot be
  // interrupted.
  bool successful;
  if (new_animation_index != current_animation_index) {
    animations[current_animation_index].reset();
    animations[new_animation_index].reset();
    current_animation_index = new_animation_index;
  }
  successful = true;

  return successful;
}

void Player::tick() {
  AnimationFrame frame = animations[current_animation_index].frame();
  set_activators(frame.activators);
  set_reactors(animations[current_animation_index].current_reactors());
  Mob::tick();

  if (game.keyboard.is_pressed(SDL_SCANCODE_RETURN) && !typing) {
    game.sprite_list.push_back(std::make_shared<Incantation>(Incantation("This_is_an_incantation", game, Pos{.layer = 0, .x = 0, .y = 100})));
  }

  if(typing) return;

  int mspeed = speed + int(speed_mod * FRAME_RATE * SUBPIXELS_IN_PIXEL);

  if (game.keyboard.is_pressed(SDL_SCANCODE_J) || current_animation_index == 2)
    switch_animation(2); // attack

  if (game.keyboard.is_pressed(SDL_SCANCODE_K))
    switch_animation(3); // dodge
  if (current_animation_index == 3) {
    if (facing_left)
      frame.velocity = -frame.velocity;
    Translation vel = {frame.velocity * SUBPIXELS_IN_PIXEL, 0};
    move(vel);
  }

  if (current_animation_index == 2)
    immobile(true); // Player cannot move while attacking

  if (current_animation_index < 2 ||
      animations[current_animation_index].is_over()) {
    immobile(false);
    if (moving)
      switch_animation(1); // walk
    else
      switch_animation(0); // idle
  }

  if (!is_immobile()) {
    Translation vel = Translation{.x = 0, .y = 0};

    if (game.keyboard.is_held(SDL_SCANCODE_W)) { // Up and down
      vel.y = -mspeed;
    } else if (game.keyboard.is_held(SDL_SCANCODE_S)) {
      vel.y = mspeed;
    }

    if (game.keyboard.is_held(SDL_SCANCODE_A)) // Left and right
    {
      vel.x = -mspeed;
      facing_left = true;
    } else if (game.keyboard.is_held(SDL_SCANCODE_D)) {
      vel.x = mspeed;
      facing_left = false;
    }

    if (vel.x != 0 && vel.y != 0) {
      vel.x = vel.x / sqrt(2);
      vel.y = vel.y / sqrt(2);
    }

    move(vel);

    if (abs((vel.x)) + abs(vel.y) != 0)
      moving = true;
    else
      moving = false;

    if (game.keyboard.is_pressed(SDL_SCANCODE_0)) // Suicide test code
    {
      death();
    }

    // FOR DEMONSTRATION PURPOSES
    if (game.keyboard.is_pressed(SDL_SCANCODE_Z)) {
      game.camera->zoom_enabled = !game.camera->zoom_enabled;
    }

    if (game.keyboard.is_pressed(SDL_SCANCODE_9)) {
      if (!test_creep || !test_creep->is_spawned()) {
        test_creep = std::make_shared<Creep>(game, pos);
        game.sprite_list.push_back(test_creep);
        game.camera->add_focus(test_creep);
      }
    }
  }
}

void Player::death() {
  if(!despawn_time){
    Mix_FadeOutMusic(1000);
    Mix_Chunk *s = game.media.readWAV("data/sound/player_death.wav");
    Mix_PlayChannel(-1, s, 0);
    despawn_time = game.frame_counter.rendered_frames + 90;
    despawn(); // replace with animation, disallow movement
  }
}

void Player::draw() {

  Mob::draw(SHAPE);
  AnimationFrame frame = animations[current_animation_index].frame();
  Translation offset = frame.sprite_offset;
  SDL_RendererFlip flip;
  SDL_Rect my_rect = SHAPE;
  // Flip the sprite if player facing left
  if (facing_left) {
    flip = SDL_FLIP_HORIZONTAL;
    offset = {-(my_rect.x - offset.x), -(my_rect.y - offset.y)};
  } else
    flip = SDL_FLIP_NONE;

  my_rect.x = pos.x + offset.x;
  my_rect.y = pos.y + offset.y;

  texture = animations[current_animation_index].play();

  SDL_QueryTexture(texture, NULL, NULL, &my_rect.w, &my_rect.h);
  my_rect.w *= 2; // Sprite w and h is set & upscaled here for the moment.
  my_rect.h *= 2;

  game.camera->render_ex(game.renderer, texture, NULL, &my_rect, 0, NULL, flip);
}

bool Player::is_immobile() const { return this->IMMOBILE_FLAG; }

void Player::immobile(bool b) { this->IMMOBILE_FLAG = b; }