#include "title.hpp"
#include "game.hpp"
#include "util.hpp"

TitleScreen::TitleScreen(Game &game): Sprite(game, Pos {.layer = 0, .x = 0, .y = 0}) {}

extern const std::vector<TitleScreenOption> TitleScreen::OPTIONS = {
  TitleScreenOption {.name = "play", .action = [](TitleScreen& self){self.game.next_level = "data/level/level_1.tmj";}},
  TitleScreenOption {.name = "quit", .action = [](TitleScreen& self){self.game.running = false;}}
};

void TitleScreen::draw() {
  constexpr int TITLE_Y = HEIGHT * .28;
  constexpr int TITLE_X = WIDTH * .32;
  constexpr int TITLE_FONT_POINT = 40;

  constexpr SDL_Color TITLE_COLOR = SDL_Color {.r = 165, .g = 6, .b = 20, .a = 255};

  TTF_Font* title_font = game.media.readFont("data/font/alagard.ttf", TITLE_FONT_POINT);

  render_text_at(game, title_font, TITLE_X, TITLE_Y, TITLE_COLOR, "- dark scrolls -");

  constexpr int OPTIONS_START_Y = HEIGHT * .5;
  constexpr int OPTIONS_X = HEIGHT * .6;
  constexpr int OPTIONS_SELECTOR_X = HEIGHT * .55;
  constexpr int OPTIONS_GAP = 32;

  constexpr SDL_Color OPTIONS_SELECTED_COLOR = SDL_Color {.r = 192, .g = 192, .b = 192, .a = 255};
  constexpr SDL_Color OPTIONS_UNSELECTED_COLOR = SDL_Color {.r = 95, .g = 95, .b = 95, .a = 95};

  int option_y = OPTIONS_START_Y;
  for (size_t i = 0; i < OPTIONS.size(); i++) {
    bool selected = i == selected_option;

    SDL_Color option_color;
    if (selected) {
      option_color = OPTIONS_SELECTED_COLOR;
    } else {
      option_color = OPTIONS_UNSELECTED_COLOR;
    }

    if (selected) {
      render_text_at(game, title_font, OPTIONS_SELECTOR_X, option_y, option_color, "-");
    }

    render_text_at(game, title_font, OPTIONS_X, option_y, option_color, OPTIONS[i].name.c_str());

    option_y += OPTIONS_GAP;
  }
}

void TitleScreen::tick() {
  if (game.keyboard.is_pressed(SDL_SCANCODE_S)) {
    selected_option++;
    if (selected_option >= OPTIONS.size()) {
      selected_option = 0;
    }
  } else if (game.keyboard.is_pressed(SDL_SCANCODE_W)) {
    if (selected_option == 0) {
      selected_option = OPTIONS.size() - 1;
    } else {
      selected_option--;
    }
  }

  if (game.keyboard.is_pressed(SDL_SCANCODE_RETURN) || game.keyboard.is_pressed(SDL_SCANCODE_SPACE)) {
    OPTIONS[selected_option].action(*this);
  }
}
