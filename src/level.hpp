#pragma once
#include <nlohmann/json.hpp>
#include <SDL2/SDL.h>
#include "pos.hpp"
#include "collide.hpp"

class Game;

enum class SpriteSpawnType { NONE, PLAYER, CREEP };

struct TileProperties {
  bool invisible = false;
  SpriteSpawnType spawn_type = SpriteSpawnType::NONE;
  TileCollideData colliders;
};

class Tile {
  public:
    Tile(Game& game, const std::filesystem::path& tileset_loc, uint32_t id, const nlohmann::json& j);
    Tile(Game& game, SDL_Texture* texture, uint32_t id, TileProperties properties);
    uint32_t get_id() const {
      return id;
    }
    SDL_Texture* get_texture() {
      return texture;
    }
    TileProperties& props() {
      return properties;
    }
    const TileProperties& props() const {
      return properties;
    }
    Tile horizontal_flip(Game& game);
    Tile vertical_flip(Game& game);
    Tile diagonal_flip(Game& game);

    Tile(const Tile& other) noexcept;

    Tile& operator=(const Tile& other) noexcept {
      if (this == &other) {
        return *this;
      }
      return *this = Tile(other);
    }

    Tile(Tile&& other) noexcept {
      this->renderer = nullptr;
      this->texture = nullptr;
      this->id = 0;

      *this = other;
    }

    Tile& operator=(Tile&& other) noexcept {
      if (this == &other) {
        return *this;
      }
      if (this->texture) {
        SDL_DestroyTexture(this->texture);
      }

      this->renderer = other.renderer;
      this->texture = other.texture;
      this->texture_backup = std::move(other.texture_backup);
      this->id = other.id;
      this->properties = other.properties;

      other.texture = nullptr;
      return *this;
    }

    void reload_texture();

    ~Tile() {
      if (texture) {
        SDL_DestroyTexture(texture);
      }
    }
  private:
    void backup_texture();

    SDL_Renderer* renderer;
    SDL_Texture* texture;
    std::vector<uint32_t> texture_backup;
    uint32_t id;
    TileProperties properties;
};

// TODO: Rename to TileLayer
class Layer {
  public:
    Layer(std::vector<std::vector<std::shared_ptr<Tile>>> tiles);

    std::vector<std::shared_ptr<Tile>>& operator[](size_t layer_id) {
      return tile_data[layer_id];
    }

    const std::vector<std::shared_ptr<Tile>>& operator[](size_t layer_id) const {
      return tile_data[layer_id];
    }

    size_t size() const {
      return tile_data.size();
    }
  private:
    std::vector<std::vector<std::shared_ptr<Tile>>> tile_data;

  public:
    decltype(std::declval<Layer>().tile_data.begin()) begin() {
      return tile_data.begin();
    }

    decltype(std::declval<const Layer>().tile_data.begin()) begin() const {
      return tile_data.begin();
    }

    decltype(std::declval<Layer>().tile_data.end()) end() {
      return tile_data.end();
    }

    decltype(std::declval<const Layer>().tile_data.end()) end() const {
      return tile_data.end();
    }
};

class Level {
  public:
    Level(Game &game);
    Level(Game &game, const std::filesystem::path& level_loc);

    void draw();
    Layer& operator[](size_t layer_id) {
      return layers[layer_id];
    }

    const Layer& operator[](size_t layer_id) const {
      return layers[layer_id];
    }

    Tile& operator[](Pos pos) {
      unsigned layer = pos.layer;
      unsigned tile_y = pos.tile_y();
      unsigned tile_x = pos.tile_x();
      bool in_bounds = layer < layers.size() &&
        tile_y < layers[layer].size() &&
        tile_x < layers[layer][tile_y].size();

      if (in_bounds) {
        return *layers[layer][tile_y][tile_x];
      } else {
        return *tilemap.find(0)->second;
      }
    }

    const Tile& operator[](Pos pos) const {
      unsigned layer = pos.layer;
      unsigned tile_y = pos.tile_y();
      unsigned tile_x = pos.tile_x();
      bool in_bounds = layer < layers.size() &&
        tile_y < layers[layer].size() &&
        tile_x < layers[layer][tile_y].size();

      if (in_bounds) {
        return *layers[layer][tile_y][tile_x];
      } else {
        return *tilemap.find(0)->second;
      }    
    }

    size_t size() const {
      return layers.size();
    }

    float get_camera_zoom() {
      return camera_zoom;
    }

    void set_camera_zoom(float scalar) {
      camera_zoom = scalar;
    }

    void add_colliders(std::vector<CollideLayer>& layers);

    void reload_texture();

  private:
    void load_tileset(const nlohmann::json& tileset, const std::filesystem::path& tileset_loc, uint32_t first_tid, uint32_t end_tid);

    uint32_t width;
    uint32_t height;
    std::unordered_map<uint32_t, std::shared_ptr<Tile>> tilemap;
    // Index order is Layer id y x
    std::vector<Layer> layers;

    Game *game;

    Translation camera_offset = {.x = 32 * SUBPIXELS_IN_PIXEL, .y = 32 * SUBPIXELS_IN_PIXEL};
    
    // This needs to be fixed.
    // A draw function needs to be created that handles
    // zoom for everything drawn (and probably transpositions as well.)
    float camera_zoom = 1;

  public:
    decltype(std::declval<Level&>().layers.begin()) begin() {
      return layers.begin();
    }

    decltype(std::declval<const Level&>().layers.begin()) begin() const {
      return layers.begin();
    }

    decltype(std::declval<Level>().layers.end()) end() {
      return layers.end();
    }

    decltype(std::declval<const Level>().layers.end()) end() const {
      return layers.end();
    }
};
