#pragma once
#include <SDL2/SDL.h>
#include "pos.hpp"

namespace mob_vars {

double FRAME_RATE;
int SUBPIXELS_IN_PIXEL;

}

int Mob_Init(const double frames, const int subpixels) {
  // if(frames < 0 || subpixels < 0) return -1;

  if(!mob_vars::FRAME_RATE && !mob_vars::SUBPIXELS_IN_PIXEL) {
    mob_vars::FRAME_RATE = frames;
    mob_vars::SUBPIXELS_IN_PIXEL = subpixels;
    return 0;
  } else return -2;
}


class Game;

static uint64_t NEXT_SPRITE_ID = 0;

class Sprite {
  public:
  Sprite(Game &game, Pos pos): game(game) {
    NEXT_SPRITE_ID++;
    this->pos = pos;
  }

  virtual void draw() = 0;
  virtual void tick() {}
  Pos get_pos() const {
    return pos;
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
  Pos pos;
  bool spawn_flag = true;
  Game &game;
};

// This class is probably deprecated. But it survives for now.
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

  Text(char *n_text, Game &game, Pos pos, SDL_Color n_color = { 255, 255, 255 }) : Sprite(game, pos) {
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

class Mob: public Sprite {
  public:
  Mob(Game &game, Pos pos): Sprite(game, pos) {}

  int get_health() const  {
    return health;
  }

  void hurt (int dmg) {
    health -= dmg;
  }

  void set_health (int n_health) {
    health = n_health;
  }

  virtual void tick() {
    if(health < 0) despawn();
  }

  protected:
  int health = 100;

  virtual ~Mob () {
  }
};

class Player: public Mob {
  public:
  Player(Game &game, Pos pos): Mob(game, pos) {
    speed = (300 * mob_vars::FRAME_RATE) * mob_vars::SUBPIXELS_IN_PIXEL;
  }

  bool is_immobile() const {
    return this->IMMOBILE_FLAG;
  }

  void immobile(bool b) {
    this->IMMOBILE_FLAG = b;
  }

  virtual void draw();
  virtual void tick();

  private:
  bool IMMOBILE_FLAG = false;
  uint32_t speed;
  static constexpr SDL_Rect SHAPE = {.x = 0, .y = 0, .w = 30, .h = 30};
  static constexpr uint8_t RED = 126;
  static constexpr uint8_t GREEN = 219;
  static constexpr uint8_t BLUE = 222;
};

// Should probably share another inheritance with player for
// mutual properties, like health and speed, etc. but I'm going to leave this for later.
class Creep: public Mob {
  public:
  Creep(Game &game, Pos pos): Mob(game, pos) {
    speed = (200 * mob_vars::FRAME_RATE) * mob_vars::SUBPIXELS_IN_PIXEL;
    this->og_pos = pos;
  }

  virtual void draw();
  virtual void tick();

  private:
  // to make it easy to check if sprites in the sprite list are creeps
  // perhaps make a separate enemy sprite list?
  static const bool creep = true;
  bool returning = false;
  Pos og_pos;
  uint32_t speed;
  SDL_Rect shape = {.x = 160, .y = -160, .w = 30, .h = 30};
};
