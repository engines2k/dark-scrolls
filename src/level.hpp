#pragma once
#include <nlohmann/json.hpp>
#include <SDL2/SDL.h>
#include "pos.hpp"

enum class SpriteSpawnType { NONE, PLAYER, CREEP };
enum class TileCollideType { FLOOR, WALL };

struct TileProperties {
  bool invisible = false;
  SpriteSpawnType spawn_type = SpriteSpawnType::NONE;
  TileCollideType collide_type = TileCollideType::FLOOR;
};

class Tile {
  public:
    Tile(SDL_Renderer* renderer, const std::filesystem::path& tileset_loc, uint32_t id, const nlohmann::json& j);
    Tile(SDL_Renderer* renderer, SDL_Texture* texture, uint32_t id, TileProperties properties);
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
    Tile horizontal_flip();
    Tile vertical_flip();
    Tile diagonal_flip();

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
      this->id = other.id;
      this->properties = other.properties;

      other.texture = nullptr;
      return *this;
    }

    ~Tile() {
      if (texture) {
        SDL_DestroyTexture(texture);
      }
    }
  private:
    SDL_Renderer* renderer;
    SDL_Texture* texture;
    uint32_t id;
    TileProperties properties;
};

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
    Level(SDL_Renderer* renderer);
    Level(SDL_Renderer* renderer, const std::filesystem::path& level_loc);

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

    Translation get_camera_offset() {
      return camera_offset;
    }

  private:
    void load_tileset(const nlohmann::json& tileset, const std::filesystem::path& tileset_loc, uint32_t first_tid, uint32_t end_tid);

    uint32_t width;
    uint32_t height;
    SDL_Renderer* renderer;
    std::unordered_map<uint32_t, std::shared_ptr<Tile>> tilemap;
    // Index order is Layer id y x
    std::vector<Layer> layers;

    Translation camera_offset = {.x = 32 * SUBPIXELS_IN_PIXEL, .y = 32 * SUBPIXELS_IN_PIXEL};
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
