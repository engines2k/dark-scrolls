#pragma once
#include <SDL2/SDL.h>
#include <unordered_set>
#include <utility>

class KeyboardManager {
public:
  bool is_pressed(SDL_Scancode scan) { return pressed.count(scan) >= 1; }

  bool is_held(SDL_Scancode scan) { return held.count(scan) >= 1; }

  void handle_keyevent(const SDL_KeyboardEvent &env) {
    if (env.type == SDL_KEYDOWN && env.repeat == 0) {
      held.insert(env.keysym.scancode);
      pressed.insert(env.keysym.scancode);
    }
    if (env.type == SDL_KEYUP) {
      held.erase(env.keysym.scancode);
    }
  }

  void reset_pressed() { pressed.clear(); }

  const std::unordered_set<SDL_Scancode> &get_pressed() const {
    return pressed;
  }

private:
  std::unordered_set<SDL_Scancode> pressed;
  std::unordered_set<SDL_Scancode> held;
};
