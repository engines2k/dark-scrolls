#pragma once
#include "collide.hpp"
#include "pos.hpp"
#include "media_manager.hpp"
#include <SDL2/SDL.h>
#include <nlohmann/json.hpp>

class Game;

enum class SpriteSpawnType { NONE, PLAYER, CREEP, HEALTH_POTION, SPEED_POTION, TITLE_SCREEN };

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

using Tilemap = std::unordered_map<uint32_t, Tile *>;

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
  TileLayer(std::vector<std::vector<Tile *>> tiles);

  std::vector<Tile *> &operator[](size_t layer_id);
  const std::vector<Tile *> &operator[](size_t layer_id) const;
  size_t size() const;

private:
  std::vector<std::vector<Tile *>> tile_data;

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

struct LevelProperties {
  std::filesystem::path background_music;
};

class Level {
public:
  Level(Game &game);
  Level(Game &game, const std::filesystem::path &level_loc);
  Level &operator=(const Level &) = delete;
  Level(Level &&) = default;
  Level &operator=(Level &&) = default;

  void draw();
  TileLayer &operator[](size_t layer_id);
  const TileLayer &operator[](size_t layer_id) const;

  Tile &operator[](Pos pos);
  const Tile &operator[](Pos pos) const;

  size_t size() const;

  void add_colliders(std::vector<CollideLayer> &layers);
  void handle_reactions();

  Level copy_level() const;

  LevelProperties &get_props();

private:
  Level(const Level &) = default;

  void load_tileset(const std::filesystem::path &tileset_loc,
                    uint32_t first_tid, uint32_t end_tid);

  uint32_t width;
  uint32_t height;
  Tilemap tilemap;
  // Index order is Layer id y x
  std::vector<TileLayer> layers;
  LevelProperties props;

  Game *game;
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

struct LevelFactory: public MediaFactory<Level *> {
  using KeyType = std::filesystem::path;
  Level *construct(MediaManager &media, const std::filesystem::path& path) {
    return new Level(media.get_game(), path);
  }

  void unload(MediaManager &media, const std::filesystem::path& path, Level *level) {
    delete level;
  }
};

