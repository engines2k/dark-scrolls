#pragma once
#include "sprite.hpp"
#include <string>
#include <functional>
#include <vector>

class TitleScreen;

struct TitleScreenOption {
  std::string name;
  std::function<void(TitleScreen&)> action;
};

class TitleScreen: public Sprite {
public:
  TitleScreen(Game &game);

  virtual void draw();
  virtual void tick();
private:
  size_t selected_option = 0;
  static const std::vector<TitleScreenOption> OPTIONS;
};
