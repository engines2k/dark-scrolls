#pragma once
#include <nlohmann/json.hpp>
#include <SDL2/SDL.h>

class Tile {
  public:
    Tile(SDL_Renderer* renderer, const std::filesystem::path& tileset_loc, uint32_t id, const nlohmann::json& j);
    Tile(SDL_Renderer* renderer, SDL_Texture* texture, uint32_t id);
    uint32_t get_id() const {
      return id;
    }
    SDL_Texture* get_texture() {
      return texture;
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
};

class Layer {
  public:
    Layer(std::vector<std::vector<uint32_t>> tiles);
    const std::vector<std::vector<uint32_t>>& get_tile_data() const {
      return tile_data;
    }
  private:
    std::vector<std::vector<uint32_t>> tile_data;
};

class Level {
  public:
    Level(SDL_Renderer* renderer);
    Level(SDL_Renderer* renderer, const std::filesystem::path& level_loc);

    void draw();
  private:
    void load_tileset(const nlohmann::json& tileset, const std::filesystem::path& tileset_loc, uint32_t first_tid, uint32_t end_tid);

    uint32_t width;
    uint32_t height;
    SDL_Renderer* renderer;
    std::unordered_map<uint32_t, Tile> tilemap;
    // Index order is Layer id y x
    std::vector<Layer> layers;
};
