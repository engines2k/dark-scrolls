#pragma once
#include "sprite.hpp"
#include <string>
#include <functional>
#include <vector>

class PauseScreen;

struct PauseScreenOption {
  std::string name;
  std::function<void(PauseScreen&)> action;
};

class PauseScreen: public Sprite {
public:
  PauseScreen(Game &game, SDL_Texture* background);

  virtual void draw();
  virtual void tick();
  virtual bool never_paused();
  ~PauseScreen();
private:
  size_t selected_option = 0;
  static const std::vector<PauseScreenOption> OPTIONS;
  SDL_Texture* background;
};
