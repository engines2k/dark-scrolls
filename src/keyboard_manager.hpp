#pragma once
#include <SDL2/SDL.h>
#include <unordered_set>
#include <utility>

class KeyboardManager {
public:
  bool is_pressed(SDL_Scancode scan);

  bool is_held(SDL_Scancode scan);

  void handle_keyevent(const SDL_KeyboardEvent &env);

  void reset_pressed();

  const std::unordered_set<SDL_Scancode> &get_pressed() const;

private:
  std::unordered_set<SDL_Scancode> pressed;
  std::unordered_set<SDL_Scancode> held;
};
