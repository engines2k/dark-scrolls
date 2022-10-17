#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_image.h>
#include <vector>
#include "pos.hpp"
#include "game.hpp"
#include "mob.hpp"
#include "animation.hpp"

Player::Player(Game &game, Pos pos): Mob(game, pos) {
  Animation a(game, 30);

  a.set_frame(0, "img/player001.png", "NOSOUND");
  a.set_frame(5, "img/player002.png", "img/walk.wav");
  a.set_frame(18, "img/player003.png", "NOSOUND");
  animations.push_back(a);

  Animation b(animations[0]);

  int hitbox_width = SHAPE.w * SUBPIXELS_IN_PIXEL;
  int hitbox_height = SHAPE.h * SUBPIXELS_IN_PIXEL;

  ReactorCollideBox hitbox(
      ReactorCollideType::WALL | ReactorCollideType::HURT,
      hitbox_width,
      hitbox_height
  );

  reactors.push_back(std::move(hitbox));

  speed = (140 * FRAME_RATE) * SUBPIXELS_IN_PIXEL;
  speed_mod = 0;
  
}


void Player::tick() {
  Mob::tick();
  int mspeed = speed + int(speed_mod * FRAME_RATE * SUBPIXELS_IN_PIXEL);
  if(!is_immobile()) {
    Translation vel = Translation {.x = 0, .y = 0};
    if (game.keyboard.is_held(SDL_SCANCODE_W)) {
      vel.y = -mspeed;
    } else if (game.keyboard.is_held(SDL_SCANCODE_S)) {
      vel.y = mspeed;
    }

    if (game.keyboard.is_held(SDL_SCANCODE_A)) {
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

    // set moving bool for 'animation' in draw method
    if(abs((vel.x)) + abs(vel.y) != 0) {
      moving = true;
      // Mix_PlayChannel(-1, walk_sound, 1);
    } else moving = false;


    //Suicide test code
    if (game.keyboard.is_held(SDL_SCANCODE_0)) {
      despawn(); 
    }
  }
}

void Player::draw() {

    SDL_Rect my_rect = SHAPE;
    SDL_RendererFlip flip;
    if(facing_left)
      //flip the sprite to face player left 
      flip = SDL_FLIP_HORIZONTAL;
    else 
      flip = SDL_FLIP_NONE;

    Pos screen_pos = game.screen_pos(pos);
    my_rect.x = screen_pos.pixel_x();
    my_rect.y = screen_pos.pixel_y();

    SDL_DestroyTexture(texture);

    if(moving)
      texture = animations[0].play();
    else {
      // display first frame
      animations[0].reset();
      SDL_Surface *surface = IMG_Load("img/player000.png");
      texture = SDL_CreateTextureFromSurface(game.renderer, surface);
    }

    SDL_RenderCopyEx(game.renderer, texture, NULL, &my_rect, 0, NULL, flip);
  }
