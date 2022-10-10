#pragma once
#include "mob.hpp"
#include "keyboard_manager.hpp"
#include <mutex>

struct FrameCounter {
  uint64_t rendered_frames = 0;
  uint64_t scheduled_frames = 0;
};

class Player;

class Game {
  public:
  Game(SDL_Renderer *renderer): renderer(renderer), current_level(renderer) {
  }

  SDL_Renderer *renderer;
  KeyboardManager keyboard;
  int32_t tick_event_id;
  FrameCounter frame_counter;
  std::mutex frame_counter_lock;
  Level current_level;
  std::filesystem::path data_path = std::filesystem::path(_pgmptr).parent_path() / "data";
  std::vector<std::shared_ptr<Sprite>> sprite_list;
  std::shared_ptr<Player> player = NULL;

  //Text test_text;

  void tick();

  Pos screen_pos(Pos pos) {
    return pos + current_level.get_camera_offset();
  }
};
