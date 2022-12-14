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
  spritename = "player";
  frameData->readAnimations("data/animation/mobs.json", spritename);

  speed = (140 * FRAME_RATE) * SUBPIXELS_IN_PIXEL;
  speed_mod = 0;

  death_sfx = game.media.readWAV("data/sound/player_death.wav");
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
    frameData->animations[current_animation_index].reset();
    frameData->animations[new_animation_index].reset();
    current_animation_index = new_animation_index;
  }
  successful = true;

  return successful;
}

void Player::tick() {
  AnimationFrame frame = frameData->animations[current_animation_index].frame();

  set_activators(frame.activators);
  set_reactors(frameData->animations[current_animation_index].current_reactors());
  Mob::tick();

  if (game.keyboard.is_pressed(SDL_SCANCODE_RETURN) && !typing) {
    game.sprite_list.push_back(std::make_shared<Incantation>(Incantation("This_is_an_incantation", game, Pos{.layer = 0, .x = 0, .y = 100})));
  }

  if(frameData->animations[current_animation_index].immobilizes)
    return;
  if(typing) return;

  int mspeed = speed + int(speed_mod * FRAME_RATE * SUBPIXELS_IN_PIXEL);

  if (game.keyboard.is_pressed(SDL_SCANCODE_J) || current_animation_index == 2)
    switch_animation(2); // attack

  if (game.keyboard.is_pressed(SDL_SCANCODE_K))
    switch_animation(3); // dodge

  if (current_animation_index == 2)
    immobile(true); // Player cannot move while attacking

  if (current_animation_index < 2 ||
      frameData->animations[current_animation_index].is_over()) {
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

    if(facing_left)
      vel.x -= frame.velocity;
    else
      vel.x += frame.velocity;

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
  switch_animation(4);
  if(!despawn_time) {
    Mix_FadeOutMusic(1000);
    Mix_PlayChannel(-1, death_sfx, 0);
    despawn_time = game.frame_counter.rendered_frames + 90;
    IMMOBILE_FLAG = true;
    //despawn(); // replace with animation, disallow movement
  }
}

void Player::draw() {

  Mob::draw(SHAPE);
  AnimationFrame frame = frameData->animations[current_animation_index].frame();
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

  texture = frameData->animations[current_animation_index].play();

  SDL_QueryTexture(texture, NULL, NULL, &my_rect.w, &my_rect.h);
  my_rect.w *= 2; // Sprite w and h is set & upscaled here for the moment.
  my_rect.h *= 2;

  game.camera->render_ex(game.renderer, texture, NULL, &my_rect, 0, NULL, flip);
}

bool Player::is_immobile() const { return this->IMMOBILE_FLAG; }

void Player::immobile(bool b) { this->IMMOBILE_FLAG = b; }