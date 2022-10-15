#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_image.h>
#include <vector>
#include "pos.hpp"
#include "game.hpp"
#include "mob.hpp"
#include "animation.hpp"

Player::Player(Game &game, Pos pos): Mob(game, pos) {
  Animation a(game, 1);
  a.set_frame(0, "img/player001.png", "NOSOUND");
  animations.push_back(a);

  hitbox.width = SHAPE.w * SUBPIXELS_IN_PIXEL;
  hitbox.height = SHAPE.h * SUBPIXELS_IN_PIXEL;
  speed = (170 * FRAME_RATE) * SUBPIXELS_IN_PIXEL;
  //hmm
  walk_sound = Mix_LoadWAV("img/crash.wav");
  if(walk_sound == nullptr){
    printf("Sound error: %s\n", SDL_GetError());
    abort();
  }
}


void Player::tick() {
  Mob::tick();
  if(!is_immobile()) {
    Translation vel = Translation {.x = 0, .y = 0};
    if (game.keyboard.is_held(SDL_SCANCODE_W)) {
      vel.y = -speed;
    } else if (game.keyboard.is_held(SDL_SCANCODE_S)) {
      vel.y = speed;
    }

    if (game.keyboard.is_held(SDL_SCANCODE_A)) {
      vel.x = -speed;
      facing_left = true;
    } else if (game.keyboard.is_held(SDL_SCANCODE_D)) {
      vel.x = speed;
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
      Mix_PlayChannel(-1, walk_sound, 1);
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

    //SDL_QueryTexture(texture, NULL, NULL, &my_rect.w, &my_rect.h);

    SDL_FreeSurface(surface);
    if(SDL_GetTicks() % 1000 < 500 && moving) {
      const char *surfacepath = animations[0].frame_path(0);
      surface = IMG_Load(surfacepath);
    }
    else
      surface = IMG_Load("img/player000.png");

    SDL_DestroyTexture(texture);
    texture = SDL_CreateTextureFromSurface(game.renderer, surface);
    SDL_RenderCopyEx(game.renderer, texture, NULL, &my_rect, 0, NULL, flip);
  }