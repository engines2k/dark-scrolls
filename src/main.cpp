#include <stdio.h>
#include <unordered_set>
#include <mutex>
#include <cstdint>
#include <cstdlib>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <memory>
#include <vector>
#include "level.hpp"
#include "keyboard_manager.hpp"
#include "mob.hpp"
#include <iostream>
//could be <SDL.h>


const int WIDTH = 800, HEIGHT = 600;

constexpr double FRAME_RATE = 1.0 / 60.0;

struct FrameCounter {
  uint64_t rendered_frames = 0;
  uint64_t scheduled_frames = 0;
};

class Player;

class Game {
  public:
  Game(SDL_Renderer *renderer): renderer(renderer), current_level(renderer) {
  }

  SDL_Renderer *renderer;
  KeyboardManager keyboard;
  int32_t tick_event_id;
  FrameCounter frame_counter;
  std::mutex frame_counter_lock;
  Level current_level;
  std::filesystem::path data_path = std::filesystem::path(_pgmptr).parent_path() / "data";
  std::vector<std::shared_ptr<Sprite>> sprite_list;
  std::shared_ptr<Player> player = NULL;

  //Text test_text;

  void tick();

  Pos screen_pos(Pos pos) {
    return pos + current_level.get_camera_offset();
  }
};

void Player::tick() {
  Mob::tick();
  if(!is_immobile()) {
    int x_speed = 0;
    int y_speed = 0;
    if (game.keyboard.is_held(SDL_SCANCODE_W)) {
      y_speed = -speed;
    } else if (game.keyboard.is_held(SDL_SCANCODE_S)) {
      y_speed = speed;
    }

    if (game.keyboard.is_held(SDL_SCANCODE_A)) {
      x_speed = -speed;
    } else if (game.keyboard.is_held(SDL_SCANCODE_D)) {
      x_speed = speed;
    }

    if (x_speed != 0 && y_speed != 0) {
      x_speed = x_speed / sqrt(2);
      y_speed = y_speed / sqrt(2);
    }

    pos.x += x_speed;
    pos.y += y_speed;

    //Suicide test code
    if (game.keyboard.is_held(SDL_SCANCODE_0)) {
      despawn(); 
    }
  }
}

void Player::draw() {
    SDL_Rect my_rect = SHAPE;
    Pos screen_pos = game.screen_pos(pos);
    my_rect.x = screen_pos.pixel_x();
    my_rect.y = screen_pos.pixel_y();

    SDL_SetRenderDrawColor(game.renderer, RED, GREEN, BLUE, 255);
    SDL_RenderFillRect(game.renderer, &my_rect);
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
    snprintf(font_path, 261, "%s\\fonts\\arial.ttf", getenv("WINDIR"));
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

  } else if (game.keyboard.is_pressed(SDL_SCANCODE_A) && toupper(phrase[index]) == 'A') {
    index++;
    draw();
  }
  else if (game.keyboard.is_pressed(SDL_SCANCODE_B) && toupper(phrase[index]) == 'B') {
    index++;
    draw();
  }
  else if (game.keyboard.is_pressed(SDL_SCANCODE_C) && toupper(phrase[index]) == 'C') {
    index++;
    draw();
  }
  else if (game.keyboard.is_pressed(SDL_SCANCODE_D) && toupper(phrase[index]) == 'D') {
    index++;
    draw();
  }
  else if (game.keyboard.is_pressed(SDL_SCANCODE_E) && toupper(phrase[index]) == 'E') {
    index++;
    draw();
  }
  else if (game.keyboard.is_pressed(SDL_SCANCODE_F) && toupper(phrase[index]) == 'F') {
    index++;
    draw();
  }
  else if (game.keyboard.is_pressed(SDL_SCANCODE_G) && toupper(phrase[index]) == 'G') {
    index++;
    draw();
  }
  else if (game.keyboard.is_pressed(SDL_SCANCODE_H) && toupper(phrase[index]) == 'H') {
    index++;
    draw();
  }
  else if (game.keyboard.is_pressed(SDL_SCANCODE_I) && toupper(phrase[index]) == 'I') {
    index++;
    draw();
  }
  else if (game.keyboard.is_pressed(SDL_SCANCODE_J) && toupper(phrase[index]) == 'J') {
    index++;
    draw();
  }
  else if (game.keyboard.is_pressed(SDL_SCANCODE_K) && toupper(phrase[index]) == 'K') {
    index++;
    draw();
  }
  else if (game.keyboard.is_pressed(SDL_SCANCODE_L) && toupper(phrase[index]) == 'L') {
    index++;
    draw();
  }
  else if (game.keyboard.is_pressed(SDL_SCANCODE_M) && toupper(phrase[index]) == 'M') {
    index++;
    draw();
  }
  else if (game.keyboard.is_pressed(SDL_SCANCODE_N) && toupper(phrase[index]) == 'N') {
    index++;
    draw();
  }
  else if (game.keyboard.is_pressed(SDL_SCANCODE_O) && toupper(phrase[index]) == 'O') {
    index++;
    draw();
  }
  else if (game.keyboard.is_pressed(SDL_SCANCODE_P) && toupper(phrase[index]) == 'P') {
    index++;
    draw();
  }
  else if (game.keyboard.is_pressed(SDL_SCANCODE_Q) && toupper(phrase[index]) == 'Q') {
    index++;
    draw();
  }
  else if (game.keyboard.is_pressed(SDL_SCANCODE_R) && toupper(phrase[index]) == 'R') {
    index++;
    draw();
  }
  else if (game.keyboard.is_pressed(SDL_SCANCODE_S) && toupper(phrase[index]) == 'S') {
    index++;
    draw();
  }
  else if (game.keyboard.is_pressed(SDL_SCANCODE_T) && toupper(phrase[index]) == 'T') {
    index++;
    draw();
  }
  else if (game.keyboard.is_pressed(SDL_SCANCODE_U) && toupper(phrase[index]) == 'U') {
    index++;
    draw();
  }
  else if (game.keyboard.is_pressed(SDL_SCANCODE_V) && toupper(phrase[index]) == 'V') {
    index++;
    draw();
  }
  else if (game.keyboard.is_pressed(SDL_SCANCODE_W) && toupper(phrase[index]) == 'W') {
    index++;
    draw();
  }
  else if (game.keyboard.is_pressed(SDL_SCANCODE_X) && toupper(phrase[index]) == 'X') {
    index++;
    draw();
  }
  else if (game.keyboard.is_pressed(SDL_SCANCODE_Y) && toupper(phrase[index]) == 'Y') {
    index++;
    draw();
  }
  else if (game.keyboard.is_pressed(SDL_SCANCODE_Z) && toupper(phrase[index]) == 'Z') {
    index++;
    draw();
  }
  else if (game.keyboard.is_pressed(SDL_SCANCODE_SPACE) && phrase[index] == '_') {
    index++;
    draw();
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
          game.player = std::make_shared<Player>(Player(game, player_pos));
          break;
        }
      }
    }
  }

  if (!game.player) {
    std::cerr << "Player not found in level" << std::endl;
    abort();
  }

  game.sprite_list.push_back(game.player);
  game.sprite_list.push_back(std::make_shared<Text>(Text((char*)"Welcome to Dark Scrolls", game, Pos {.layer = 0, .x = -32 * SUBPIXELS_IN_PIXEL, .y = -32 * SUBPIXELS_IN_PIXEL})));
  game.sprite_list.push_back(std::make_shared<Incantation>(Incantation("This_is_an_incantation", game, Pos {.layer = 0, .x = 0, .y = 100})));
  game.sprite_list.push_back(std::make_shared<Creep>(Creep(game, Pos {.layer = 0, .x = 380 * mob_vars::SUBPIXELS_IN_PIXEL, .y = 390 * mob_vars::SUBPIXELS_IN_PIXEL})));

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
