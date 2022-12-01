#pragma once
#include "collide.hpp"
#include "pos.hpp"
#include <SDL2/SDL.h>
#include <nlohmann/json.hpp>

class Game;

enum class SpriteSpawnType { NONE, PLAYER, CREEP, HEALTH_POTION, SPEED_POTION };

class TileLayer;

// Params are (x, y, current_layer, activator)
using OnTileReactFn = std::function<void(int, int, TileLayer &)>;
extern const OnTileReactFn DO_NOTHING_ON_TILE_REACT;

struct TileReactorData {
  ReactorCollideBox react_box;
  OnTileReactFn on_react;
};

struct TileCollideData {
  std::vector<ActivatorCollideBox> activators;
  std::vector<TileReactorData> reactors;
};

struct TileProperties {
  bool invisible = false;
  SpriteSpawnType spawn_type = SpriteSpawnType::NONE;
  TileCollideData colliders;
  OnTileReactFn lever_activate = DO_NOTHING_ON_TILE_REACT;
  OnTileReactFn lever_deactivate = DO_NOTHING_ON_TILE_REACT;
};

class Tile;

using Tilemap = std::unordered_map<uint32_t, std::shared_ptr<Tile>>;

class Tile {
public:
  Tile(Game &game, Tilemap &tilemap,
       const std::filesystem::path &tileset_loc, uint32_t id,
       const nlohmann::json &j);
  Tile(Game &game, SDL_Texture *texture, uint32_t id,
       TileProperties properties);

  //uint32_t get_id() const;
  uint32_t get_local_id() const;
  SDL_Texture *get_texture();
  TileProperties &props();
  const TileProperties &props() const;
  Tile horizontal_flip(Game &game);
  Tile vertical_flip(Game &game);
  Tile diagonal_flip(Game &game);

  Tile(const Tile &other) noexcept;
  Tile &operator=(const Tile &other) noexcept;
  Tile(Tile &&other) noexcept;
  Tile &operator=(Tile &&other) noexcept;

  void reload_texture();
  void handle_reactions();

  ~Tile() {
    if (texture) {
      SDL_DestroyTexture(texture);
    }
  }

private:
  friend class TileImpl;

  SDL_Renderer *renderer;
  SDL_Texture *texture;
  std::vector<uint32_t> texture_backup;
  uint32_t id;
  TileProperties properties;
};

class TileLayer {
public:
  TileLayer(std::vector<std::vector<std::shared_ptr<Tile>>> tiles);

  std::vector<std::shared_ptr<Tile>> &operator[](size_t layer_id);
  const std::vector<std::shared_ptr<Tile>> &operator[](size_t layer_id) const;
  size_t size() const;

private:
  std::vector<std::vector<std::shared_ptr<Tile>>> tile_data;

public:
  decltype(std::declval<TileLayer>().tile_data.begin()) begin() {
    return tile_data.begin();
  }

  decltype(std::declval<const TileLayer>().tile_data.begin()) begin() const {
    return tile_data.begin();
  }

  decltype(std::declval<TileLayer>().tile_data.end()) end() {
    return tile_data.end();
  }

  decltype(std::declval<const TileLayer>().tile_data.end()) end() const {
    return tile_data.end();
  }
};

class Level {
public:
  Level(Game &game);
  Level(Game &game, const std::filesystem::path &level_loc);
  Level(const Level &) = delete;
  Level &operator=(const Level &) = delete;
  Level(Level &&) = default;
  Level &operator=(Level &&) = default;

  void draw();
  TileLayer &operator[](size_t layer_id);
  const TileLayer &operator[](size_t layer_id) const;

  Tile &operator[](Pos pos);
  const Tile &operator[](Pos pos) const;

  size_t size() const;

  float get_camera_zoom();
  void set_camera_zoom(float scalar);

  void add_colliders(std::vector<CollideLayer> &layers);
  void handle_reactions();

  void reload_texture();

private:
  void load_tileset(const std::filesystem::path &tileset_loc,
                    uint32_t first_tid, uint32_t end_tid);

  uint32_t width;
  uint32_t height;
  Tilemap tilemap;
  // Index order is Layer id y x
  std::vector<TileLayer> layers;

  Game *game;

  Translation camera_offset = {.x = 32 * SUBPIXELS_IN_PIXEL,
                               .y = 32 * SUBPIXELS_IN_PIXEL};

  // This needs to be fixed.
  // A draw function needs to be created that handles
  // zoom for everything drawn (and probably transpositions as well.)
  float camera_zoom = 1;

public:
  decltype(std::declval<Level &>().layers.begin()) begin() {
    return layers.begin();
  }

  decltype(std::declval<const Level &>().layers.begin()) begin() const {
    return layers.begin();
  }

  decltype(std::declval<Level>().layers.end()) end() { return layers.end(); }

  decltype(std::declval<const Level>().layers.end()) end() const {
    return layers.end();
  }
};
