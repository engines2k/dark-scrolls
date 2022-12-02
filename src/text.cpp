#include "text.hpp"
#include "game.hpp"
#include "util.hpp"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <memory>

Text::Text(char *n_text, Game &game, Pos pos, SDL_Color n_color)
    : Sprite(game, pos) {
  color = n_color;
  text = n_text;
  char font_path[261];
  // snprintf(font_path, 261, "%s\\fonts\\arial.ttf", getenv("WINDIR"));
  snprintf(font_path, 261, "./data/font/alagard.ttf");
  font = game.media.readFont(font_path, 25);
}

void Text::draw() {
  texture = game.media.showFont(
      font, text,
      color);

  texW = 0;
  texH = 0;
  SDL_QueryTexture(texture, NULL, NULL, &texW, &texH);
  dstrect = {pos.x, pos.y, texW, texH};
  game.camera->render(game.renderer, texture, NULL, &dstrect);

  // SDL_FreeSurface(surface);
  SDL_DestroyTexture(texture);
}

void Text::tick() {}

void Text::set_color(SDL_Color n_color) { color = n_color; }

int Text::get_w() { return texW; }

Incantation::Incantation(std::string n_phrase, Game &game, Pos pos)
    : Sprite(game, pos) {
  char font_path[261];
  // snprintf(font_path, 261, "%s\\fonts\\arial.ttf", getenv("WINDIR"));
  snprintf(font_path, 261, "./data/font/alagard.ttf");

  font = game.media.readFont(font_path, 25);

  type_sound = game.media.readWAV("data/sound/type.wav");
  type_finish_sound = game.media.readWAV("data/sound/type_finish.wav");
  type_init_sound = game.media.readWAV("data/sound/type_init.wav");

  index = 0;
  phrase = n_phrase;
  game.player->typing = true;

  Mix_PlayChannel(-1, type_init_sound, 0);
}

void Incantation::tick() {
  if(index >= phrase.length()) {
    game.player->typing = false;
    despawn();
  }


  for (auto key : game.keyboard.get_pressed())
    if (scancode_to_char(key) == toupper(phrase[index])) {
      Mix_PlayChannel(-1, type_sound, 0);
      index++;
    }

  if (index <= phrase.length()) draw();
}

void Incantation::draw() {
  if (index <= phrase.length()) {
    this->pos = game.player->get_pos();
    pos.y -= 30 * SUBPIXELS_IN_PIXEL;
    // Grab phrase snippets
    typed_surface = TTF_RenderText_Solid(font, phrase.substr(0, index).c_str(), color_red);
    untyped_surface = TTF_RenderText_Solid(font, phrase.substr(index).c_str(), color_grey);
    typed_texture = SDL_CreateTextureFromSurface(game.renderer, typed_surface);
    untyped_texture = SDL_CreateTextureFromSurface(game.renderer, untyped_surface);
    // Set rects
    SDL_QueryTexture(typed_texture, NULL, NULL, &typed_texW, &typed_texH);
    SDL_QueryTexture(untyped_texture, NULL, NULL, &untyped_texW, &untyped_texH);
    dstrect = {pos.x, pos.y, typed_texW, typed_texH};
    undstrect = {pos.x + (typed_texW * SUBPIXELS_IN_PIXEL), pos.y, untyped_texW, untyped_texH};
    // Center align 
    dstrect.x -= (dstrect.w + undstrect.w) * SUBPIXELS_IN_PIXEL / 3;
    undstrect.x -= (dstrect.w + undstrect.w) * SUBPIXELS_IN_PIXEL / 3;
  }

  game.camera->render(game.renderer, typed_texture, NULL, &dstrect);
  game.camera->render(game.renderer, untyped_texture, NULL, &undstrect);

  SDL_FreeSurface(typed_surface);
  SDL_FreeSurface(untyped_surface);
  typed_surface = nullptr;
  untyped_surface = nullptr;

  SDL_DestroyTexture(typed_texture);
  SDL_DestroyTexture(untyped_texture);
}
