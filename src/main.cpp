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
#include <iostream>
//could be <SDL.h>

const int WIDTH = 800, HEIGHT = 600;

constexpr double FRAME_RATE = 1.0 / 60.0;
constexpr int SUBPIXELS_IN_PIXEL = 1 << 15;

class KeyboardManager {
  public:
  bool is_pressed(SDL_Scancode scan) {
    return pressed.count(scan) >= 1;
  }

  bool is_held(SDL_Scancode scan) {
    return held.count(scan) >= 1;
  }

  void handle_keyevent(const SDL_KeyboardEvent &env) {
    if(env.type == SDL_KEYDOWN && env.repeat == 0) {
      held.insert(env.keysym.scancode);
      pressed.insert(env.keysym.scancode);
    }
    if(env.type == SDL_KEYUP) {
      held.erase(env.keysym.scancode);
    }
  }

  void reset_pressed() {
    pressed.clear();
  }

  private:
  std::unordered_set<SDL_Scancode> pressed;
  std::unordered_set<SDL_Scancode> held;
};

struct FrameCounter {
  uint64_t rendered_frames = 0;
  uint64_t scheduled_frames = 0;
};

class Game;

static uint64_t NEXT_SPRITE_ID = 0;

class Sprite {
  public:
  Sprite(Game &game, int x, int y): game(game) {
    NEXT_SPRITE_ID++;
    this->pos_x = x;
    this->pos_y = y;
  }

  virtual void draw() = 0;
  virtual void tick() {}
  int get_pos_x() const {
    return pos_x;
  }
  int get_pos_y() const {
    return pos_y;
  }
  void despawn() {
    spawn_flag = false;
  }
  bool is_spawned() const {
    return spawn_flag;
  }

  virtual ~Sprite() {}

  const int id = NEXT_SPRITE_ID;

  protected:
  int pos_x;
  int pos_y;
  bool spawn_flag = true;
  Game &game;
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
};

class Text : public Sprite {
  SDL_Surface *surface = NULL;
  TTF_Font *font;
  SDL_Texture *texture;
  int texW,texH;
  SDL_Rect dstrect;
  SDL_Color color = { 255, 255, 255 };

public:
  char *text;
  void set_color(SDL_Color n_color) {
    color = n_color;
  }

  int get_w() {
    return texW;
  }

  Text(char *n_text, Game &game, int pos_x, int pos_y, SDL_Color n_color = { 255, 255, 255 }) : Sprite(game, pos_x, pos_y) {
    color = n_color;
    text = n_text;
    char font_path[261];
    snprintf(font_path, 261, "%s\\fonts\\arial.ttf", getenv("WINDIR"));
    font = TTF_OpenFont(font_path, 25);
    if (font == nullptr) {
      printf("Font error: %s\n", SDL_GetError());
      abort();
    }
  }

  void draw();
  void tick();
};

class Player: public Sprite {
  public:
  Player(Game &game, int pos_x, int pos_y): Sprite(game, pos_x, pos_y) {}

  bool is_immobile() const {
    return this->IMMOBILE_FLAG;
  }

  void immobile(bool b) {
    this->IMMOBILE_FLAG = b;
  }

  virtual void tick() override {
    if(!is_immobile()) {
      int x_speed = 0;
      int y_speed = 0;
      if (game.keyboard.is_held(SDL_SCANCODE_W)) {
        y_speed = -SPEED;
      } else if (game.keyboard.is_held(SDL_SCANCODE_S)) {
        y_speed = SPEED;
      }

      if (game.keyboard.is_held(SDL_SCANCODE_A)) {
        x_speed = -SPEED;
      } else if (game.keyboard.is_held(SDL_SCANCODE_D)) {
        x_speed = SPEED;
      }

      pos_x += x_speed;
      pos_y += y_speed; 
    }

    //Suicide test code
    if (game.keyboard.is_held(SDL_SCANCODE_0)) {
      despawn(); 
    }
  }

  virtual void draw() override {
    SDL_Rect my_rect = SHAPE;
    my_rect.x = pos_x / SUBPIXELS_IN_PIXEL;
    my_rect.y = pos_y / SUBPIXELS_IN_PIXEL;

    SDL_SetRenderDrawColor(game.renderer, RED, GREEN, BLUE, 255);
    SDL_RenderFillRect(game.renderer, &my_rect);
  }

  private:
  bool IMMOBILE_FLAG = false;
  static constexpr uint32_t SPEED = (300 * FRAME_RATE) * SUBPIXELS_IN_PIXEL;
  static constexpr SDL_Rect SHAPE = {.x = 0, .y = 0, .w = 30, .h = 30};
  static constexpr uint8_t RED = 126;
  static constexpr uint8_t GREEN = 219;
  static constexpr uint8_t BLUE = 222;
};

// this class is trash
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
  Incantation(std::string n_phrase, Game &game, int pos_x, int pos_y) : Sprite(game, pos_x, pos_y) {
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
      this->pos_x = game.player->get_pos_x() / SUBPIXELS_IN_PIXEL;
      this->pos_y = game.player->get_pos_y() / SUBPIXELS_IN_PIXEL -40; /* must be modified later to scale with player*/
      dstrect = { pos_x, pos_y, typed_texW, typed_texH };

      const char *untyped = phrase.substr(index).c_str();
      untyped_surface = TTF_RenderText_Solid(font, untyped, color_grey);
      untyped_texture = SDL_CreateTextureFromSurface(game.renderer, untyped_surface);
      SDL_QueryTexture(untyped_texture, NULL, NULL, &untyped_texW, &untyped_texH);
      undstrect = { pos_x + typed_texW, pos_y, untyped_texW, untyped_texH };
      if(index == 0) undstrect.x = pos_x;

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
  dstrect = { pos_x, pos_y, texW, texH };
  SDL_RenderCopy(game.renderer, texture, NULL, &dstrect);
}

void Text::tick() {
}

void Incantation::tick() {
  if (game.keyboard.is_pressed(SDL_SCANCODE_RETURN)) {
      if(typed_surface == NULL) {
        inc_btn_pressed = true;
        draw();
        game.player->immobile(true);
      }
      else if(index >= phrase.length()) {
        despawn();
        game.player->immobile(false);
        game.player->despawn();
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
  game.player = std::make_shared<Player>(Player(game, 0, 0));

  game.sprite_list.push_back(game.player);
  game.sprite_list.push_back(std::make_shared<Text>(Text((char*)"Welcome to Dark Scrolls", game, 0, 0)));
  game.sprite_list.push_back(std::make_shared<Incantation>(Incantation("This_is_an_incantation", game, 0, 100)));

  game.tick_event_id = SDL_RegisterEvents(1);

  SDL_TimerID tick_timer = SDL_AddTimer(FRAME_RATE * 1000, game_timer, &game);

  game.current_level = Level(game.renderer, game.data_path / "level/test_room.tmj");
  
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
