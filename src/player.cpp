#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_image.h>
#include <vector>
#include "pos.hpp"
#include "game.hpp"
#include "mob.hpp"
#include "animation.hpp"

Player::Player(Game &game, Pos pos): Mob(game, pos)
{
  Animation walk(game, 48, 1);
  walk.set_frame(0, "data/sprite/player_run000.png", "NOSOUND");
  walk.set_frame(6, "data/sprite/player_run001.png", "data/sound/walk.wav");
  walk.set_frame(13, "data/sprite/player_run002.png", "NOSOUND");
  walk.set_frame(20, "data/sprite/player_run003.png", "NOSOUND");
  walk.set_frame(24, "data/sprite/player_run004.png", "NOSOUND");
  walk.set_frame(30, "data/sprite/player_run005.png", "data/sound/walk.wav");
  walk.set_frame(37, "data/sprite/player_run006.png", "NOSOUND");
  walk.set_frame(44, "data/sprite/player_run007.png", "NOSOUND");

  Animation idle(game, 70, 1);
  idle.set_frame(0, "data/sprite/player_idle000.png", "NOSOUND");
  idle.set_frame(35, "data/sprite/player_idle001.png", "NOSOUND");

  Animation attack(game, 60, 0);
  attack.set_frame(0, "data/sprite/player_attack000.png", "NOSOUND");

  animations.push_back(idle);
  animations.push_back(walk);
  animations.push_back(attack);
  current_animation_index = 0;            // Player is initialized with idle animation

  // FIXME: Placeholder reactor
  int hitbox_width = 32 * SUBPIXELS_IN_PIXEL;
  int hitbox_offset_x = 16 * SUBPIXELS_IN_PIXEL;
  int hitbox_height = 54 * SUBPIXELS_IN_PIXEL;
  int hitbox_offset_y = 8 * SUBPIXELS_IN_PIXEL;

  ReactorCollideBox hitbox(
    ReactorCollideType::WALL | ReactorCollideType::HURT_BY_EVIL,
    hitbox_offset_x,
    hitbox_width,
    hitbox_offset_y,
    hitbox_height
  );

  reactors.push_back(std::move(hitbox));

  speed = (140 * FRAME_RATE) * SUBPIXELS_IN_PIXEL;
  speed_mod = 0;
}

bool Player::switch_animation(int new_animation_index) {
  // WIP : Needs to return false if the animation currently playing cannot be interrupted.
  bool successful;
    if(new_animation_index != current_animation_index){
      animations[current_animation_index].reset();
      current_animation_index = new_animation_index;
    }
    successful = true;
  
  return successful;
}

void Player::tick() {
  Mob::tick();
  int mspeed = speed + int(speed_mod * FRAME_RATE * SUBPIXELS_IN_PIXEL);
  if(!is_immobile())
  {
    Translation vel = Translation {.x = 0, .y = 0};

    if (game.keyboard.is_held(SDL_SCANCODE_W)) {    // Up and down
      vel.y = -mspeed;
    } else if (game.keyboard.is_held(SDL_SCANCODE_S)) {
      vel.y = mspeed;
    }

    if (game.keyboard.is_held(SDL_SCANCODE_A))     // Left and right
    {
      vel.x = -mspeed;
      facing_left = true;
    } 
    else if (game.keyboard.is_held(SDL_SCANCODE_D))
    {
      vel.x = mspeed;
      facing_left = false;
    }

    if (vel.x != 0 && vel.y != 0)
    {
      vel.x = vel.x / sqrt(2);
      vel.y = vel.y / sqrt(2);
    }

    move(vel);

    if(abs((vel.x)) + abs(vel.y) != 0)
      moving = true;    
    else
      moving = false;
    
    if (game.keyboard.is_held(SDL_SCANCODE_0))     // Suicide test code
    {
      despawn(); 
    }
  }
}

void Player::draw()
{
    SDL_Rect my_rect = SHAPE;
    Pos screen_pos = game.screen_pos(pos);
    my_rect.x = screen_pos.pixel_x();
    my_rect.y = screen_pos.pixel_y();

<<<<<<< HEAD
    SDL_RendererFlip flip;    // Flip the sprite if player facing left 
    if(facing_left)
      flip = SDL_FLIP_HORIZONTAL;
    else 
      flip = SDL_FLIP_NONE;

    if (game.keyboard.is_pressed(SDL_SCANCODE_J))
        switch_animation(2);  // attack

    if (current_animation_index != 2 || animations[current_animation_index].is_over()){
      if(animations[current_animation_index].is_over())
        immobile(false);
      if (moving)
        switch_animation(1);  // walk
      else
        switch_animation(0);  // idle 

    else immobile(true);      // Player cannot move while attacking

    SDL_DestroyTexture(texture);
    texture = animations[current_animation_index].play();

    SDL_RenderCopyEx(game.renderer, texture, NULL, &my_rect, 0, NULL, flip);
}