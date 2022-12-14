#include "pause.hpp"
#include "game.hpp"
#include "util.hpp"

PauseScreen::PauseScreen(Game &game, SDL_Texture* background): Sprite(game, Pos {.layer = 0, .x = 0, .y = 0}) {
  this->background = background;
}

extern const std::vector<PauseScreenOption> PauseScreen::OPTIONS = {
  PauseScreenOption {.name = "resume", .action = [](PauseScreen& self){
    self.game.should_unpause = true;
    self.despawn();
  }},
  PauseScreenOption {.name = "restart", .action = [](PauseScreen& self){self.game.next_level = self.game.level_path;}},
  PauseScreenOption {.name = "quit", .action = [](PauseScreen& self){self.game.running = false;}}
};

void PauseScreen::draw() {
  constexpr int PAUSED_BACKGROUND_OPACITY = 75;
  SDL_RenderCopy(game.renderer, background, nullptr, nullptr);
  SDL_SetRenderDrawColor(game.renderer, 0, 0, 0, 255 - PAUSED_BACKGROUND_OPACITY);
  SDL_RenderFillRect(game.renderer, nullptr);

  constexpr int PAUSED_Y = 0;
  constexpr int PAUSED_X = 0;
  constexpr int TITLE_FONT_POINT = 40;

  constexpr SDL_Color PAUSED_COLOR = SDL_Color {.r = 192, .g = 192, .b = 192, .a = 255};

  TTF_Font* pause_font = game.media.readFont("data/font/alagard.ttf", TITLE_FONT_POINT);

  render_text_at(game, pause_font, PAUSED_X, PAUSED_Y, PAUSED_COLOR, "paused");

  constexpr int OPTIONS_START_Y = HEIGHT * .2;
  constexpr int OPTIONS_X = HEIGHT * .8;
  constexpr int OPTIONS_SELECTOR_X = HEIGHT * .75;
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
      render_text_at(game, pause_font, OPTIONS_SELECTOR_X, option_y, option_color, "-");
    }

    render_text_at(game, pause_font, OPTIONS_X, option_y, option_color, OPTIONS[i].name.c_str());

    option_y += OPTIONS_GAP;
  }
}

void PauseScreen::tick() {
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
  if (game.keyboard.is_pressed(SDL_SCANCODE_ESCAPE)) {
    game.should_unpause = true;
    despawn();
  }
}

bool PauseScreen::never_paused() {
  return true;
}

PauseScreen::~PauseScreen() {
  SDL_DestroyTexture(background);
}
