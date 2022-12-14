#pragma once
#include "media_manager.hpp"
#include "sprite.hpp"
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <string>

class Game;

// This class is probably deprecated. But it lives for now.
class Text : public Sprite {
  protected:
    SDL_Surface *surface = NULL;
    SDL_Texture *texture = NULL;
    TTF_Font *font;

    int texW, texH;
    SDL_Rect dstrect;
    SDL_Color color = {255, 255, 255};

public:
  char *text;
  void set_color(SDL_Color n_color);
  int get_w();

  Text(char *n_text, Game &game, Pos pos, SDL_Color n_color = {255, 255, 255});

  void draw();
  void tick();
};

class Incantation : public Sprite {
  Mix_Chunk *type_sound;
  Mix_Chunk *type_finish_sound;
  Mix_Chunk *type_init_sound;
  SDL_Color color_red = {255, 0, 0};
  SDL_Color color_grey = {200, 200, 200};
  SDL_Surface *typed_surface = NULL;
  TTF_Font *font;
  SDL_Texture *typed_texture;
  int typed_texW, typed_texH;
  SDL_Rect dstrect;

  SDL_Surface *untyped_surface = NULL;
  SDL_Texture *untyped_texture;
  int untyped_texW, untyped_texH;
  SDL_Rect undstrect;

  std::string phrase;
  uint32_t index;
  bool inc_btn_pressed = false;
  std::shared_ptr<Sprite> player;

public:
  Incantation(std::string n_phrase, Game &game, Pos pos);

  void tick();
  void draw();
};


class AppearingText : public Text {
  public:
    AppearingText(char *n_text, Game &game, Pos pos, Pos display, int n_radius = 1, bool only_once = false, SDL_Color n_color = {255, 255, 255});

    void tick();
    void draw();
  
  protected:
    bool once = false; // If you only want the text to appear one time
    int radius = 1;
    const int block_size = 32; // size of a tile in pixels
    ActivatorCollideBox hitbox;
    ReactorCollideBox reactbox;
    bool player_inside = false;
    int count = 0;
};