#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <memory>
#include "util.hpp"
#include "text.hpp"
#include "game.hpp"

Text::Text(char *n_text, Game &game, Pos pos, SDL_Color n_color) : Sprite(game, pos) {
  color = n_color;
  text = n_text;
  char font_path[261];
  //snprintf(font_path, 261, "%s\\fonts\\arial.ttf", getenv("WINDIR"));
  snprintf(font_path, 261, "./data/font/alagard.ttf");
  font = mediaManager.readFont(font_path, 25);
}

void Text::draw() {
  //surface = TTF_RenderText_Solid(font, text, color);
  texture = mediaManager.showFont(game.renderer, font, text, color); //SDL_CreateTextureFromSurface(game.renderer, surface);

  texW = 0;
  texH = 0;
  SDL_QueryTexture(texture, NULL, NULL, &texW, &texH);
  dstrect = { 300, -40, texW, texH };
  SDL_RenderCopy(game.renderer, texture, NULL, &dstrect);

  //SDL_FreeSurface(surface);
  SDL_DestroyTexture(texture);
}

void Text::tick() {
}

void Incantation::tick() {
  if (game.keyboard.is_pressed(SDL_SCANCODE_RETURN)) {
      if(index >= phrase.length()) {
        Mix_PlayChannel(-1, type_finish_sound, 0);
        game.player->immobile(false);
        game.player->speed_mod = 60;
        despawn();
      } else if(typed_surface == NULL) {
        inc_btn_pressed = true;
        Mix_PlayChannel(-1, type_init_sound, 0);
        draw();
        game.player->immobile(true);
      }

  } else for (auto key: game.keyboard.get_pressed()) {
    if (scancode_to_char(key) == toupper(phrase[index])) {
      Mix_PlayChannel(-1, type_sound, 0);
      index++;
      draw();
    }
  }
}

void Incantation::draw() {
    if(!inc_btn_pressed) return;

    if(index <= phrase.length()) {
  
      const char *typed = phrase.substr(0, index).c_str();
      typed_surface = TTF_RenderText_Solid(font, typed, color_red);
      typed_texture = SDL_CreateTextureFromSurface(game.renderer, typed_surface);

      SDL_QueryTexture(typed_texture, NULL, NULL, &typed_texW, &typed_texH);
      this->pos = game.player->get_pos();
      this->pos += Translation {.x = SUBPIXELS_IN_PIXEL * 40, .y = SUBPIXELS_IN_PIXEL * -40 }; /* must be modified later to scale with player*/

      // std::cout << game.player->get_pos_x() << std::endl;
      dstrect = { pos.x, pos.y, typed_texW, typed_texH };

      const char *untyped = phrase.substr(index).c_str();
      untyped_surface = TTF_RenderText_Solid(font, untyped, color_grey);
      untyped_texture = SDL_CreateTextureFromSurface(game.renderer, untyped_surface);
      SDL_QueryTexture(untyped_texture, NULL, NULL, &untyped_texW, &untyped_texH);
      undstrect = { pos.x + typed_texW, pos.x, untyped_texW, untyped_texH };
      // if(index == 0) undstrect.x = pos.pixel_x();

      // center align text
      int offset_center = (dstrect.w + undstrect.w) / 2.05;
      dstrect.x -= offset_center;
      undstrect.x -= offset_center;
    }
    
    SDL_RenderCopy(game.renderer, typed_texture, NULL, &dstrect);
    SDL_RenderCopy(game.renderer, untyped_texture, NULL, &undstrect);

    SDL_FreeSurface(typed_surface);
    typed_surface = nullptr;
    SDL_FreeSurface(untyped_surface);
    untyped_surface = nullptr;

    SDL_DestroyTexture(typed_texture);
    SDL_DestroyTexture(untyped_texture);
  }
