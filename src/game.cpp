#include "game.hpp"

Game::Game(SDL_Renderer *renderer)
     : renderer(renderer), current_level(*this), media(*this) {
  // FIXME: Figure out how to determine max collide layers
  collide_layers.resize(1);
}

void Game::tick() {
  SDL_SetRenderTarget(renderer, nullptr);
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
  SDL_RenderClear(renderer);

  for (auto &collide_layer : collide_layers) {
    collide_layer.clear();
  }
  current_level.add_colliders(collide_layers);

  for (auto &sprite : sprite_list) {
    sprite->add_colliders();
  }

  for (auto &sprite : sprite_list) {
    sprite->tick();
  }

  current_level.handle_reactions();

  current_level.draw();

  for (auto &sprite : sprite_list) {
    sprite->draw();
  }

  // set_cam_trans();
  camera->calc_offset();
  camera->calc_zoom();

  std::vector<std::shared_ptr<Sprite>> next_sprite_list;

  for (auto &sprite : sprite_list) {
    if (sprite->is_spawned()) {
      next_sprite_list.push_back(std::move(sprite));
    }
  }

  sprite_list = std::move(next_sprite_list);
  keyboard.reset_pressed();

  SDL_RenderPresent(renderer);
  auto frame_counter_lock = std::lock_guard(this->frame_counter_lock);
  frame_counter.rendered_frames++;
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
