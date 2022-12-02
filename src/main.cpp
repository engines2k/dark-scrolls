#include "camera.hpp"
#include "creep.hpp"
#include "game.hpp"
#include "item.hpp"
#include "keyboard_manager.hpp"
#include "level.hpp"
#include "mob.hpp"
#include "player.hpp"
#include "sprite.hpp"
#include "text.hpp"
#include "util.hpp"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <mutex>
#include <stdio.h>
#include <unordered_set>
#include <vector>

uint32_t game_timer(uint32_t rate, void *game_ptr) {
  Game &game = *static_cast<Game *>(game_ptr);

  auto frame_counter_lock = std::lock_guard(game.frame_counter_lock);

  if (game.frame_counter.scheduled_frames -
          game.frame_counter.rendered_frames >=
      2) {
    return rate;
  }

  game.frame_counter.scheduled_frames++;

  SDL_Event event;
  SDL_UserEvent userevent;

  userevent.type = SDL_USEREVENT;
  userevent.code = game.tick_event_id;
  userevent.data1 = nullptr;
  userevent.data2 = nullptr;

  event.type = SDL_USEREVENT;
  event.user = userevent;

  SDL_PushEvent(&event);
  return rate;
}

int main(int argc, char *argv[]) {
  if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
    printf("SDL_Init failed: %s\n", SDL_GetError());
    return 1;
  }

  if (TTF_Init() < 0) {
    printf("TTF_Init failed: %s\n", TTF_GetError());
    return 1;
  }

  int sdl_img_flags = IMG_INIT_PNG;
  if (IMG_Init(sdl_img_flags) != sdl_img_flags) {
    printf("IMG_Init failed: %s\n", IMG_GetError());
    return 1;
  }

  if (Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 2, 4096) == -1) {
    return false;
  }

  SDL_Window *window;

  window = SDL_CreateWindow("Dark Scrolls", SDL_WINDOWPOS_UNDEFINED,
                            SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT,
                            SDL_WINDOW_ALLOW_HIGHDPI);
  if (window == NULL) {
    printf("Could not create window: %s\n", SDL_GetError());
    return 1;
  }

  auto game_loc = std::make_unique<Game>(SDL_CreateRenderer(window, -1, 0));
  auto &game = *game_loc;
  game.tick_event_id = SDL_RegisterEvents(1);

  game.load_level("data/level/level_1.tmj");

  SDL_TimerID tick_timer = SDL_AddTimer(FRAME_RATE * 1000, game_timer, &game);

  SDL_Event event;
  while (1) {
    if (SDL_WaitEvent(&event)) {
      switch (event.type) {
      case SDL_QUIT:
        goto endgame;
      case SDL_KEYUP:
      case SDL_KEYDOWN:
        game.keyboard.handle_keyevent(event.key);
        break;
      case SDL_USEREVENT:
        if (event.user.code == game.tick_event_id) {
          game.tick();
        }
        break;
      case SDL_RENDER_TARGETS_RESET:
      case SDL_RENDER_DEVICE_RESET:
        game.media.flushTextureCache();
        break;
      }
    } else {
      printf("Event error: %s\n", SDL_GetError());
      break;
    }
  }
endgame:
  SDL_RemoveTimer(tick_timer);

  SDL_DestroyWindow(window);

  game_loc = nullptr;

  Mix_Quit();
  IMG_Quit();
  SDL_Quit();
  return 0;
}
