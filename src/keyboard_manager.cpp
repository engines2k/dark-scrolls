#include "keyboard_manager.hpp"

bool KeyboardManager::is_pressed(SDL_Scancode scan) { return pressed.count(scan) >= 1; }

bool KeyboardManager::is_held(SDL_Scancode scan) { return held.count(scan) >= 1; }

void KeyboardManager::handle_keyevent(const SDL_KeyboardEvent &env) {
	if (env.type == SDL_KEYDOWN && env.repeat == 0) {
		held.insert(env.keysym.scancode);
		pressed.insert(env.keysym.scancode);
	}
	if (env.type == SDL_KEYUP) {
		held.erase(env.keysym.scancode);
	}
}

void KeyboardManager::reset_pressed() { pressed.clear(); }

const std::unordered_set<SDL_Scancode> &KeyboardManager::get_pressed() const { return pressed; }