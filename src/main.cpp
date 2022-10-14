#include <stdio.h>
#include <unordered_set>
#include <mutex>
#include <cstdint>
#include <cstdlib>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <memory>
#include <vector>
#include "level.hpp"
#include "keyboard_manager.hpp"
#include "game.hpp"
#include "sprite.hpp"
#include "mob.hpp"
#include "util.hpp"
#include "creep.hpp"
#include "player.hpp"
#include "text.hpp"
#include <iostream>

const int WIDTH = 800, HEIGHT = 600;

uint32_t game_timer(uint32_t rate, void *game_ptr) {
  Game &game = *static_cast<Game*>(game_ptr);

  auto frame_counter_lock = std::lock_guard(game.frame_counter_lock);

  if (game.frame_counter.scheduled_frames - game.frame_counter.rendered_frames >= 2) {
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

void Game::tick() {
  SDL_SetRenderTarget(renderer, nullptr);
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
  SDL_RenderClear(renderer);

  current_level.draw();

  for (auto &sprite: sprite_list) {
    sprite->tick();
    sprite->draw();
  }

  std::vector<std::shared_ptr<Sprite>> next_sprite_list;

  for (auto &sprite: sprite_list) {
    if (sprite->is_spawned()) {
      next_sprite_list.push_back(std::move(sprite));
    }
  }

  sprite_list = std::move(next_sprite_list);
  keyboard.reset_pressed();

  SDL_RenderPresent(renderer);
  auto frame_counter_lock = std::lock_guard(this->frame_counter_lock);
  frame_counter.rendered_frames++;
}

int main(int argc, char *argv[]) {
  if(SDL_Init(SDL_INIT_EVERYTHING) < 0) {
    printf("SDL_Init failed: %s\n", SDL_GetError());
    return 1;
  }

  if(TTF_Init() < 0) {
    printf("TTF_Init failed: %s\n", TTF_GetError());
    return 1;
  }

  int sdl_img_flags = IMG_INIT_PNG;
  if(IMG_Init(sdl_img_flags) != sdl_img_flags) {
    printf("IMG_Init failed: %s\n", IMG_GetError());
    return 1;
  }

 if( Mix_OpenAudio( 22050, MIX_DEFAULT_FORMAT, 2, 4096 ) == -1 )
    {
        return false;    
    }

  SDL_Window *window;

  window = SDL_CreateWindow("Hello, World!",
                                        SDL_WINDOWPOS_UNDEFINED,
                                        SDL_WINDOWPOS_UNDEFINED,
                                        WIDTH, HEIGHT,
                                        SDL_WINDOW_ALLOW_HIGHDPI);
  if(window == NULL) {
    printf("Could not create window: %s\n", SDL_GetError());
    return 1;
  }

  Game game(SDL_CreateRenderer(window, -1, 0));
  game.current_level = Level(game.renderer, game.data_path / "level/test_room_2.tmj");
  for (unsigned layer_id = 0; layer_id < game.current_level.size(); layer_id++) {
    for (unsigned y = 0; y < game.current_level[layer_id].size(); y++) {
      for (unsigned x = 0; x < game.current_level[layer_id][y].size(); x++) {
        Pos pos;
        pos.layer = static_cast<int>(layer_id);
        pos.y = static_cast<int>(y) * TILE_SUBPIXEL_SIZE;
        pos.x = static_cast<int>(x) * TILE_SUBPIXEL_SIZE;
        if (game.current_level[pos].props().spawn_type == SpriteSpawnType::PLAYER) {
          Pos player_pos = pos;
          player_pos.layer -= 1;
          game.player = std::make_shared<Player>(game, player_pos);
          break;
        }
      }
    }
  }

  for (unsigned layer_id = 0; layer_id < game.current_level.size(); layer_id++) {
    for (unsigned y = 0; y < game.current_level[layer_id].size(); y++) {
      for (unsigned x = 0; x < game.current_level[layer_id][y].size(); x++) {
        Pos pos;
        pos.layer = static_cast<int>(layer_id);
        pos.y = static_cast<int>(y) * TILE_SUBPIXEL_SIZE;
        pos.x = static_cast<int>(x) * TILE_SUBPIXEL_SIZE;
        Pos sprite_pos = pos;
        sprite_pos.layer -= 1;
        if (game.current_level[pos].props().spawn_type == SpriteSpawnType::CREEP) {
          game.sprite_list.push_back(std::make_shared<Creep>(game, sprite_pos));
        }
      }
    }
  }

  if (!game.player) {
    std::cerr << "Player not found in level" << std::endl;
    abort();
  }

  game.sprite_list.push_back(game.player);
  game.sprite_list.push_back(std::make_shared<Text>(Text((char*)"Welcome to Dark Scrolls", game, Pos {.layer = 0, .x = 220 * SUBPIXELS_IN_PIXEL, .y = -27 * SUBPIXELS_IN_PIXEL})));
  game.sprite_list.push_back(std::make_shared<Incantation>(Incantation("This_is_an_incantation", game, Pos {.layer = 0, .x = 0, .y = 100})));

  game.tick_event_id = SDL_RegisterEvents(1);

  SDL_TimerID tick_timer = SDL_AddTimer(FRAME_RATE * 1000, game_timer, &game);

  
  SDL_Event event;
  while(1) {
    if(SDL_WaitEvent(&event)) {
      switch(event.type) {
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
          game.current_level.reload_texture();
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

  SDL_Quit();
  return 0;
}
