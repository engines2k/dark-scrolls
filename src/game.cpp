#include "game.hpp"

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
