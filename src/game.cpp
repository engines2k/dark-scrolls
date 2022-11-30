#include "game.hpp"

Game::Game(SDL_Renderer *renderer)
     : renderer(renderer), current_level(*this), media(renderer) {
  // FIXME: Figure out how to determine max collide layers
  collide_layers.resize(1);
}

#ifdef DARK_SCROLLS_WINDOWS
std::filesystem::path Game::calc_data_path() {
  return std::filesystem::path(_pgmptr).parent_path() / "data";
}
#endif

#ifdef DARK_SCROLLS_LINUX
std::filesystem::path Game::calc_data_path() {
  return std::filesystem::read_symlink("/proc/self/exe").parent_path() / "data";
}
#endif
