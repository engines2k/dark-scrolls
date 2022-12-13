#pragma once
#include "camera.hpp"
#include "collide.hpp"
#include "inventory.hpp"
#include "keyboard_manager.hpp"
#include "media_manager.hpp"
#include "player.hpp"
#include <mutex>
#include <filesystem>

constexpr int WIDTH = 800, HEIGHT = 600;

constexpr double FRAME_RATE = 1.0 / 60.0;

struct FrameCounter {
  uint64_t rendered_frames = 0;
  uint64_t scheduled_frames = 0;
};

class Player;

class Game : public std::enable_shared_from_this<Game> {
public:
  Game(SDL_Renderer *renderer);

  std::shared_ptr<Camera> camera = NULL;
  SDL_Renderer *renderer;
  KeyboardManager keyboard;
  int32_t tick_event_id;
  FrameCounter frame_counter;
  std::mutex frame_counter_lock;
  Level current_level;
  std::filesystem::path level_path;
  std::vector<std::shared_ptr<Sprite>> sprite_list;
  std::shared_ptr<Player> player = NULL;
  std::shared_ptr<Inventory> inventory = NULL;
  std::vector<CollideLayer> collide_layers;
  MediaManager media;
  bool running = true;
  std::filesystem::path next_level;

  // Text test_text;

  void tick();

  void load_level(const std::filesystem::path &path);
  void reset_level();

private:
  static std::filesystem::path calc_data_path();
  double opacity;
  SDL_Rect death_fade;
};
