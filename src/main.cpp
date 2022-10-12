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
#include "mob.hpp"
#include "util.hpp"
#include <iostream>
//could be <SDL.h>


const int WIDTH = 800, HEIGHT = 600;

constexpr double FRAME_RATE = 1.0 / 60.0;


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
    if(SDL_GetTicks() % 1000 < 500 && moving)
      surface = IMG_Load("img/player001.png");
    else
      surface = IMG_Load("img/player000.png");

    SDL_DestroyTexture(texture);
    texture = SDL_CreateTextureFromSurface(game.renderer, surface);
    SDL_RenderCopyEx(game.renderer, texture, NULL, &my_rect, 0, NULL, flip);
  }

void Creep::draw() {
  SDL_Rect rect = shape;
  Pos screen_pos = game.screen_pos(pos);
  rect.x = screen_pos.pixel_x();
  rect.y = screen_pos.pixel_y();

  SDL_SetRenderDrawColor(game.renderer, 255, 0, 0, 255);
  SDL_RenderFillRect(game.renderer, &rect);
}


void Creep::tick() {
  Mob::tick();
  if(pos.y == og_pos.y && returning)
    returning = !returning;
  if(abs(og_pos.y - pos.y) / mob_vars::SUBPIXELS_IN_PIXEL >= 200)
    returning = true;
  if(abs(og_pos.y - pos.y) / mob_vars::SUBPIXELS_IN_PIXEL < 200 && !returning)
    pos.y += speed;
  else 
    pos.y += -speed;
}

// this class could use some work
class Incantation : public Sprite {
  SDL_Color color_red = { 255, 0, 0 };
  SDL_Color color_grey = { 200, 200, 200 };
  SDL_Surface *typed_surface = NULL;
  TTF_Font *font;
  SDL_Texture *typed_texture;
  int typed_texW,typed_texH;
  SDL_Rect dstrect;

  SDL_Surface *untyped_surface = NULL;
  SDL_Texture *untyped_texture;
  int untyped_texW,untyped_texH;
  SDL_Rect undstrect;

  std::string phrase;
  uint32_t index;
  bool inc_btn_pressed;
  std::shared_ptr<Sprite> player;

  public:
  Incantation(std::string n_phrase, Game &game, Pos pos) : Sprite(game, pos) {
    char font_path[261];
    // snprintf(font_path, 261, "%s\\fonts\\arial.ttf", getenv("WINDIR"));
    snprintf(font_path, 261, ".\\data\\font\\alagard.ttf");
    font = TTF_OpenFont(font_path, 25);
    if (font == nullptr) {
      printf("Font error: %s\n", SDL_GetError());
      abort();
    }
    index = 0;
    phrase = n_phrase;
  }

  void tick();
  void draw() {
    if(!inc_btn_pressed) return;

    if(index <= phrase.length()) {
  
      const char *typed = phrase.substr(0, index).c_str();
      typed_surface = TTF_RenderText_Solid(font, typed, color_red);
      typed_texture = SDL_CreateTextureFromSurface(game.renderer, typed_surface);
      SDL_QueryTexture(typed_texture, NULL, NULL, &typed_texW, &typed_texH);
      this->pos = game.player->get_pos();
      this->pos += Translation {.x = 0, .y = SUBPIXELS_IN_PIXEL * -40 }; /* must be modified later to scale with player*/

      Pos screen_pos = game.screen_pos(pos);

      // std::cout << game.player->get_pos_x() << std::endl;
      dstrect = { screen_pos.pixel_x(), screen_pos.pixel_y(), typed_texW, typed_texH };

      const char *untyped = phrase.substr(index).c_str();
      untyped_surface = TTF_RenderText_Solid(font, untyped, color_grey);
      untyped_texture = SDL_CreateTextureFromSurface(game.renderer, untyped_surface);
      SDL_QueryTexture(untyped_texture, NULL, NULL, &untyped_texW, &untyped_texH);
      undstrect = { screen_pos.pixel_x() + typed_texW, screen_pos.pixel_y(), untyped_texW, untyped_texH };
      if(index == 0) undstrect.x = pos.pixel_x();

      // center align text
      int offset_center = (dstrect.w + undstrect.w) / 2.05;
      dstrect.x -= offset_center;
      undstrect.x -= offset_center;
    }
    SDL_RenderCopy(game.renderer, typed_texture, NULL, &dstrect);
    SDL_RenderCopy(game.renderer, untyped_texture, NULL, &undstrect);

    SDL_FreeSurface(typed_surface);
    SDL_FreeSurface(untyped_surface);

    SDL_DestroyTexture(typed_texture);
    SDL_DestroyTexture(untyped_texture);
  }
};

void Text::draw() {
  surface = TTF_RenderText_Solid(font, text, color);
  texture = SDL_CreateTextureFromSurface(game.renderer, surface);

  texW = 0;
  texH = 0;
  SDL_QueryTexture(texture, NULL, NULL, &texW, &texH);
  Pos screen_pos = game.screen_pos(pos);
  dstrect = { screen_pos.pixel_x(), screen_pos.pixel_y(), texW, texH };
  SDL_RenderCopy(game.renderer, texture, NULL, &dstrect);

  SDL_FreeSurface(surface);
  SDL_DestroyTexture(texture);
}

void Text::tick() {
}

void Incantation::tick() {
  if (game.keyboard.is_pressed(SDL_SCANCODE_RETURN)) {
      if(typed_surface == NULL) {
        inc_btn_pressed = true;
        game.player->immobile(true);
        draw();
      }
      else if(index >= phrase.length()) {
        despawn();
        game.player->immobile(false);
        /* TEMP TEMP */
        game.sprite_list[3]->despawn();
      }

  } 

  for (auto key: game.keyboard.get_pressed()) {
    if (scancode_to_char(key) == toupper(phrase[index])) {
      index++;
      draw();
    }
  }
}

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

  if(Mob_Init(FRAME_RATE, SUBPIXELS_IN_PIXEL) < 0) {
    printf("Mob_Init failed!");
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
  game.current_level = Level(game.renderer, game.data_path / "level/test_room.tmj");
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
