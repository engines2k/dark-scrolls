#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <string>
#include "sprite.hpp"

class Game;

// This class is probably deprecated. But it lives for now.
class Text : public Sprite {
  SDL_Surface *surface = NULL;
  SDL_Texture *texture = NULL;
  TTF_Font *font;

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

  Text(char *n_text, Game &game, Pos pos, SDL_Color n_color = { 255, 255, 255 });

  void draw();
  void tick();
};

class Incantation : public Sprite {
  Mix_Chunk *type_sound;
  Mix_Chunk *type_finish_sound;
  Mix_Chunk *type_init_sound;
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
  Incantation(std::string n_phrase, Game &game, Pos pos) : Sprite(game, pos) 
  {
    char font_path[261];
    // snprintf(font_path, 261, "%s\\fonts\\arial.ttf", getenv("WINDIR"));
    snprintf(font_path, 261, ".\\data\\font\\alagard.ttf");
    font = TTF_OpenFont(font_path, 25);
    if (font == nullptr) {
      printf("Font error: %s\n", SDL_GetError());
      abort();
    }

    type_sound = Mix_LoadWAV("img/type.wav");
    if(type_sound == nullptr){
      printf("Sound error: %s\n", SDL_GetError());
      abort();
    }
    type_finish_sound = Mix_LoadWAV("img/type_finish.wav");
    if(type_finish_sound == nullptr){
      printf("Sound error: %s\n", SDL_GetError());
      abort();
    }

    type_init_sound = Mix_LoadWAV("img/type_init.wav");
    if(type_init_sound == nullptr){
      printf("Sound error: %s\n", SDL_GetError());
      abort();
    }

    index = 0;
    phrase = n_phrase;
  }

  void tick();
  void draw();
};