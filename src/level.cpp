#include "level.hpp"
#include "game.hpp"
#include "media_manager.hpp"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <unordered_map>

#include <cstdio>

using json = nlohmann::json;

static void do_nothing_on_tile_react(int x, int y, TileLayer &layer) {}

const OnTileReactFn DO_NOTHING_ON_TILE_REACT = do_nothing_on_tile_react;

// Represents a tile and its 7 rotations and reflections
struct TileGroup {
  std::array<std::shared_ptr<Tile>, 8> tiles;

  TileGroup(Tilemap &tilemap, uint32_t tile_id);
  std::shared_ptr<Tile> transfer_flip(uint32_t id);
};

TileGroup::TileGroup(Tilemap &tilemap, uint32_t tile_id) {
  for (int flip_bits = 0; flip_bits < 0x8; flip_bits++) {
    uint32_t translated_id = tile_id | (flip_bits << 29);
    tiles[flip_bits] = tilemap[translated_id];
  }
}

std::shared_ptr<Tile> TileGroup::transfer_flip(uint32_t id) {
  int flip_bits = id >> 29;
  return tiles[flip_bits];
}

struct TileImpl {
  TileImpl() = delete;

  static void backup_texture(Tile &self);
  static OnTileReactFn gen_react_fn(Game &game, Tilemap &tilemap,
                                    uint32_t id_offset, const json &j);
};

void TileImpl::backup_texture(Tile &tile) {
  SDL_SetRenderTarget(tile.renderer, tile.texture);
  tile.texture_backup.resize(TILE_SIZE * TILE_SIZE);
  SDL_Rect backup_zone = {.x = 0, .y = 0, .w = TILE_SIZE, .h = TILE_SIZE};
  SDL_RenderReadPixels(tile.renderer, &backup_zone, SDL_PIXELFORMAT_RGBA8888,
                       &tile.texture_backup.front(), TILE_SIZE * 4);
}

// Implimentation of the type TileReactorBehavor in "doc/Tile Exdata.txt"
OnTileReactFn TileImpl::gen_react_fn(Game &game, Tilemap &tilemap,
                                     uint32_t id_offset, const json &j) {
  if (j.is_array()) {
    std::vector<json> function_defs = j;
    std::vector<OnTileReactFn> functions;
    for (auto &function_data : function_defs) {
      functions.push_back(
          gen_react_fn(game, tilemap, id_offset, function_data));
    }
    return [functions = std::move(functions)](int tile_x, int tile_y,
                                              TileLayer &current_layer) {
      for (auto &function : functions) {
        function(tile_x, tile_y, current_layer);
      }
    };
  }
  std::string fn_type = j["type"];
  if (fn_type == "do_nothing") {
    return DO_NOTHING_ON_TILE_REACT;
  } else if (fn_type == "change_id") {
    uint32_t new_id = static_cast<uint32_t>(j["new_id"]) + id_offset;
    TileGroup new_tile = TileGroup(tilemap, new_id);
    return
        [new_tile](int tile_x, int tile_y, TileLayer &current_layer) mutable {
          int this_id = current_layer[tile_y][tile_x]->get_id();
          current_layer[tile_y][tile_x] = new_tile.transfer_flip(this_id);
        };
  } else if (fn_type == "lever") {
    bool new_lever_state = j["new_lever_state"];
    uint32_t new_id = static_cast<uint32_t>(j["new_id"]) + id_offset;
    TileGroup new_tile = TileGroup(tilemap, new_id);
    return [=](int tile_x, int tile_y, TileLayer &current_layer) mutable {
      int this_id = current_layer[tile_y][tile_x]->get_id();
      current_layer[tile_y][tile_x] = new_tile.transfer_flip(this_id);

      for (size_t i = 0; i < current_layer.size(); i++) {
        auto &row = current_layer[i];
        for (size_t j = 0; j < row.size(); j++) {
          auto &tile = row[j];
          if (new_lever_state) {
            tile->props().lever_activate(j, i, current_layer);
          } else {
            tile->props().lever_deactivate(j, i, current_layer);
          }
        }
      }
    };
  } else if (fn_type == "play_sfx") {
    std::string sound_path = j["sound_file"];
    return [=, &game](int tile_x, int tile_y, TileLayer &current_layer) {
      Mix_Chunk *s = game.media.readWAV(sound_path);
      Mix_PlayChannel(-1, s, 0);
    };
  } else {
    std::cerr << "Unknown type on tile react funtion" << std::endl;
    abort();
  }
}

Tile::Tile(Game &game, Tilemap &tilemap, uint32_t id_offset,
           const std::filesystem::path &tileset_loc, uint32_t id,
           const nlohmann::json &j) {
  std::string texture_path = j["image"];
  std::string texture_fullpath =
      (tileset_loc.parent_path() / texture_path).u8string();

  SDL_Texture *tex = game.media.readTexture(texture_fullpath.c_str());

  SDL_SetTextureBlendMode(tex, SDL_BLENDMODE_BLEND);
  SDL_Texture *big_tex =
      SDL_CreateTexture(game.renderer, SDL_PIXELFORMAT_RGBA8888,
                        SDL_TEXTUREACCESS_TARGET, TILE_SIZE, TILE_SIZE);
  SDL_SetTextureBlendMode(big_tex, SDL_BLENDMODE_BLEND);
  SDL_SetRenderTarget(game.renderer, big_tex);
  SDL_SetTextureBlendMode(tex, SDL_BLENDMODE_NONE);
  SDL_RenderCopy(game.renderer, tex, nullptr, nullptr);
  SDL_DestroyTexture(tex);
  tex = big_tex;

  auto properties_iter = j.find("properties");
  std::vector<json> props;
  if (properties_iter != j.end()) {
    props = *properties_iter;
  }
  for (auto &prop : props) {
    const std::string &name = prop["name"];
    json value = prop["value"];
    if (name == "invisible") {
      this->properties.invisible = value == true;
    } else if (name == "spawn_tile") {
      if (value == "player") {
        this->properties.spawn_type = SpriteSpawnType::PLAYER;
      } else if (value == "creep") {
        this->properties.spawn_type = SpriteSpawnType::CREEP;
      } else if (value == "health_potion") {
        this->properties.spawn_type = SpriteSpawnType::HEALTH_POTION;
      } else if (value == "speed_potion") {
        this->properties.spawn_type = SpriteSpawnType::SPEED_POTION;
      }
    } else if (name == "activators") {
      std::vector<json> hitboxes = json::parse(std::string(value));
      for (auto &hitbox_json : hitboxes) {
        int activator = 0x1;
        int start_x = 0;
        int width = TILE_SIZE;
        int start_y = 0;
        int height = TILE_SIZE;
        CollideDamageProps damage;
        damage.hp_delt = 0;

        auto activator_iter = hitbox_json.find("type");
        if (activator_iter != hitbox_json.end()) {
          activator = *activator_iter;
        }
        auto start_x_iter = hitbox_json.find("start_x");
        if (start_x_iter != hitbox_json.end()) {
          start_x = *start_x_iter;
        }
        auto width_iter = hitbox_json.find("width");
        if (width_iter != hitbox_json.end()) {
          width = *width_iter;
        }
        auto start_y_iter = hitbox_json.find("start_y");
        if (start_y_iter != hitbox_json.end()) {
          start_y = *start_y_iter;
        }
        auto height_iter = hitbox_json.find("height");
        if (height_iter != hitbox_json.end()) {
          height = *height_iter;
        }
        auto damage_iter = hitbox_json.find("damage");
        if (damage_iter != hitbox_json.end()) {
          json damage_object = *damage_iter;
          damage.hp_delt = damage_object["hp_delt"];
        }

        auto activator_type = static_cast<ActivatorCollideType>(activator);

        start_x *= SUBPIXELS_IN_PIXEL;
        width *= SUBPIXELS_IN_PIXEL;
        start_y *= SUBPIXELS_IN_PIXEL;
        height *= SUBPIXELS_IN_PIXEL;
        auto new_activator = ActivatorCollideBox(activator_type, start_x, width,
                                                 start_y, height);
        new_activator.damage = damage;

        TileReactorData tile_react;

        this->properties.colliders.activators.push_back(new_activator);
      }
    } else if (name == "reactors") {
      std::vector<json> hitboxes = json::parse(std::string(value));
      for (auto &hitbox_json : hitboxes) {
        int type = hitbox_json["type"];
        int start_x = 0;
        int width = TILE_SIZE;
        int start_y = 0;
        int height = TILE_SIZE;

        auto start_x_iter = hitbox_json.find("start_x");
        if (start_x_iter != hitbox_json.end()) {
          start_x = *start_x_iter;
        }
        auto width_iter = hitbox_json.find("width");
        if (width_iter != hitbox_json.end()) {
          width = *width_iter;
        }
        auto start_y_iter = hitbox_json.find("start_y");
        if (start_y_iter != hitbox_json.end()) {
          start_y = *start_y_iter;
        }
        auto height_iter = hitbox_json.find("height");
        if (height_iter != hitbox_json.end()) {
          height = *height_iter;
        }

        auto reactor_type = static_cast<ReactorCollideType>(type);

        start_x *= SUBPIXELS_IN_PIXEL;
        width *= SUBPIXELS_IN_PIXEL;
        start_y *= SUBPIXELS_IN_PIXEL;
        height *= SUBPIXELS_IN_PIXEL;

        TileReactorData tile_reactor;
        tile_reactor.react_box =
            ReactorCollideBox(reactor_type, start_x, width, start_y, height);
        tile_reactor.on_react = TileImpl::gen_react_fn(game, tilemap, id_offset,
                                                       hitbox_json["on_react"]);

        this->properties.colliders.reactors.push_back(tile_reactor);
      }
    } else if (name == "on_lever_activate" || name == "on_lever_deactivate") {
      OnTileReactFn *lever_change_state;
      if (name == "on_lever_activate") {
        lever_change_state = &properties.lever_activate;
      } else if (name == "on_lever_deactivate") {
        lever_change_state = &properties.lever_deactivate;
      }
      json lever_change_data = json::parse(std::string(value));

      *lever_change_state =
          TileImpl::gen_react_fn(game, tilemap, id_offset, lever_change_data);
    }
  }

  this->renderer = game.renderer;
  this->texture = tex;
  this->id = id;
  TileImpl::backup_texture(*this);
}
Tile::Tile(Game &game, SDL_Texture *texture, uint32_t id,
           TileProperties properties) {
  this->texture = texture;
  this->renderer = game.renderer;
  this->id = id;
  this->properties = properties;
  TileImpl::backup_texture(*this);
}
Tile Tile::horizontal_flip(Game &game) {
  SDL_Texture *fliped =
      SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888,
                        SDL_TEXTUREACCESS_TARGET, TILE_SIZE, TILE_SIZE);
  SDL_SetTextureBlendMode(fliped, SDL_BLENDMODE_BLEND);
  SDL_SetRenderTarget(renderer, fliped);
  SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_NONE);
  SDL_RenderCopyEx(renderer, texture, nullptr, nullptr, 0, nullptr,
                   SDL_FLIP_HORIZONTAL);
  SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);

  TileProperties props = properties;
  for (auto &activator : props.colliders.activators) {
    int end_x = activator.offset_x + activator.width;
    activator.offset_x = (SUBPIXELS_IN_PIXEL * 32) - end_x;
  }

  return Tile(game, fliped, id | 0x80000000, std::move(props));
}
Tile Tile::vertical_flip(Game &game) {
  SDL_Texture *fliped =
      SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888,
                        SDL_TEXTUREACCESS_TARGET, TILE_SIZE, TILE_SIZE);
  SDL_SetTextureBlendMode(fliped, SDL_BLENDMODE_BLEND);
  SDL_SetRenderTarget(renderer, fliped);
  SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_NONE);
  SDL_RenderCopyEx(renderer, texture, nullptr, nullptr, 0, nullptr,
                   SDL_FLIP_VERTICAL);
  SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);

  TileProperties props = properties;
  for (auto &activator : props.colliders.activators) {
    int end_y = activator.offset_y + activator.height;
    activator.offset_y = (SUBPIXELS_IN_PIXEL * TILE_SIZE) - end_y;
  }

  return Tile(game, fliped, id | 0x40000000, std::move(props));
}
Tile Tile::diagonal_flip(Game &game) {
  SDL_Texture *fliped =
      SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888,
                        SDL_TEXTUREACCESS_TARGET, TILE_SIZE, TILE_SIZE);
  SDL_SetTextureBlendMode(fliped, SDL_BLENDMODE_BLEND);
  SDL_SetRenderTarget(renderer, fliped);
  SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_NONE);
  SDL_RenderCopyEx(renderer, texture, nullptr, nullptr, 90, nullptr,
                   SDL_FLIP_VERTICAL);
  SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);

  TileProperties props = properties;
  for (auto &activator : props.colliders.activators) {
    // Flip x and y axis
    activator = ActivatorCollideBox(activator.type, activator.offset_y,
                                    activator.height, activator.offset_x,
                                    activator.width);
  }

  return Tile(game, fliped, id | 0x20000000, std::move(props));
}

Tile::Tile(const Tile &other) noexcept {
  this->properties = other.properties;
  this->renderer = other.renderer;
  this->id = other.id;
  this->texture_backup = other.texture_backup;

  this->texture =
      SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888,
                        SDL_TEXTUREACCESS_TARGET, TILE_SIZE, TILE_SIZE);
  SDL_SetTextureBlendMode(this->texture, SDL_BLENDMODE_BLEND);
  SDL_Texture *old_render_target = SDL_GetRenderTarget(renderer);
  SDL_SetRenderTarget(renderer, this->texture);
  SDL_SetTextureBlendMode(other.texture, SDL_BLENDMODE_NONE);
  SDL_RenderCopy(renderer, other.texture, nullptr, nullptr);
  SDL_SetTextureBlendMode(other.texture, SDL_BLENDMODE_BLEND);
  SDL_SetRenderTarget(renderer, old_render_target);
}

void Tile::reload_texture() {
  SDL_DestroyTexture(texture);
  texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888,
                              SDL_TEXTUREACCESS_STATIC, TILE_SIZE, TILE_SIZE);
  SDL_UpdateTexture(texture, nullptr, &texture_backup.front(), TILE_SIZE * 4);
}

TileLayer::TileLayer(std::vector<std::vector<std::shared_ptr<Tile>>> tiles) {
  tile_data = std::move(tiles);
}

Level::Level(Game &game) {
  this->game = &game;
  this->width = 0;
  this->height = 0;
}

Level::Level(Game &game, const std::filesystem::path &level_loc) {
  this->game = &game;
  std::ifstream level_file(level_loc);
  json level_data = json::parse(level_file);
  this->width = level_data["width"].get<uint32_t>();
  this->height = level_data["height"].get<uint32_t>();
  std::shared_ptr<Tile> empty =
      std::make_shared<Tile>(Tile(game, nullptr, 0, TileProperties()));
  empty->props().invisible = true;
  tilemap.insert(std::pair(0, std::move(empty)));

  std::vector<json> tilesets = level_data["tilesets"];
  std::sort(tilesets.begin(), tilesets.end(),
            [](const json &lhs, const json &rhs) {
              return lhs["firstgid"].get<uint32_t>() <
                     rhs["firstgid"].get<uint32_t>();
            });
  for (size_t i = 0; i < tilesets.size(); i++) {
    uint32_t first_tid = tilesets[i]["firstgid"].get<uint32_t>();
    uint32_t last_tid = UINT32_MAX;
    if (i < tilesets.size() - 1) {
      last_tid = tilesets[i + 1]["firstgid"].get<uint32_t>();
    }
    std::filesystem::path tileset_loc =
        level_loc.parent_path() / tilesets[i]["source"].get<std::string>();
    std::ifstream tileset_file(tileset_loc);
    json tileset_json = json::parse(tileset_file);
    load_tileset(tileset_json, tileset_loc, first_tid, last_tid);
  }

  std::vector<json> layers = level_data["layers"];
  for (auto &layer_data : layers) {
    std::vector<std::vector<std::shared_ptr<Tile>>> tiles;
    std::vector<std::shared_ptr<Tile>> current_row;
    std::vector<uint32_t> tile_data =
        layer_data["data"].get<std::vector<uint32_t>>();
    for (uint32_t tile_id : tile_data) {
      tile_id &= ~0x10000000; // This flip bit is only used for hex maps and
                              // should be masked for non-hex maps.
      auto tile_iter = tilemap.find(tile_id);
      if (tile_iter == tilemap.end()) {
        std::cerr << "Tile " << tile_id << " could not be found." << std::endl;
        abort();
      }
      std::shared_ptr<Tile> tile = tile_iter->second;
      current_row.push_back(tile);
      if (current_row.size() >= this->width) {
        tiles.push_back(std::move(current_row));
        current_row.clear();
      }
    }
    this->layers.push_back(TileLayer(std::move(tiles)));
  }
}

void Level::draw() {
  if (!game) {
    printf("Level game not initialized!");
    abort();
  }
  for (auto &layer : layers) {
    SDL_Rect dest_loc = {0, 0, TILE_SIZE, TILE_SIZE};
    for (auto &row : layer) {
      // printf("x: %i y: %i\n", dest_loc.x, dest_loc.y);
      for (auto &tile : row) {
        if (!tile->props().invisible) {
          game->camera->render(game->renderer, tile->get_texture(), nullptr,
                               &dest_loc);
        }
        dest_loc.x += TILE_SUBPIXEL_SIZE;
      }
      dest_loc.x = 0;
      dest_loc.y += TILE_SUBPIXEL_SIZE;
    }
  }
}

void Level::load_tileset(const json &tileset,
                         const std::filesystem::path &tileset_loc,
                         uint32_t first_tid, uint32_t end_tid) {
  std::vector<json> tiles = tileset["tiles"];
  for (auto &tile_data : tiles) {
    uint32_t global_tile_id = tile_data["id"].get<uint32_t>() + first_tid;
    if (global_tile_id >= end_tid) {
      continue;
    }
    Tile placeholder_tile(*game, nullptr, global_tile_id, TileProperties());

    for (int flip_bits = 0; flip_bits < 0x8; flip_bits++) {
      int placeholder_id = global_tile_id | (flip_bits << 29);
      Tile placeholder_tile(*game, nullptr, placeholder_id, TileProperties());
      tilemap.insert(std::pair(
          placeholder_id, std::make_shared<Tile>(std::move(placeholder_tile))));
    }
  }
  for (auto &tile_data : tiles) {
    uint32_t global_tile_id = tile_data["id"].get<uint32_t>() + first_tid;
    if (global_tile_id >= end_tid) {
      continue;
    }
    Tile base_tile(*game, tilemap, first_tid, tileset_loc, global_tile_id,
                   tile_data);

    for (int flip_bits = 0; flip_bits < 0x8; flip_bits++) {
      Tile new_tile = base_tile;
      if ((flip_bits & 0x1) == 0x1) {
        new_tile = new_tile.diagonal_flip(*game);
      }
      if ((flip_bits & 0x2) == 0x2) {
        new_tile = new_tile.horizontal_flip(*game);
      }
      if ((flip_bits & 0x4) == 0x4) {
        new_tile = new_tile.vertical_flip(*game);
      }
      *tilemap[new_tile.get_id()] = std::move(new_tile);
    }
  }
}

void Level::reload_texture() {
  for (auto &tile_pair : tilemap) {
    tile_pair.second->reload_texture();
  }
}

void Level::add_colliders(std::vector<CollideLayer> &collide_layers) {
  for (auto &tile_layer : *this) {
    for (size_t y = 0; y < tile_layer.size(); y++) {
      auto &tile_row = tile_layer[y];
      for (size_t x = 0; x < tile_row.size(); x++) {
        Tile &tile = *tile_row[x];
        for (auto &activator : tile.props().colliders.activators) {
          Pos pos = {.layer = 0,
                     .x = static_cast<int>(x * TILE_SUBPIXEL_SIZE),
                     .y = static_cast<int>(y * TILE_SUBPIXEL_SIZE)};

          // TODO: Assumes collide layer is always zero
          collide_layers[0].add_activator(activator, pos);
        }
      }
    }
  }
}

void Level::handle_reactions() {
  for (auto &tile_layer : *this) {
    for (size_t y = 0; y < tile_layer.size(); y++) {
      auto &tile_row = tile_layer[y];
      for (size_t x = 0; x < tile_row.size(); x++) {
        Tile &tile = *tile_row[x];
        for (auto &reactor : tile.props().colliders.reactors) {
          Pos pos = {.layer = 0,
                     .x = static_cast<int>(x * TILE_SUBPIXEL_SIZE),
                     .y = static_cast<int>(y * TILE_SUBPIXEL_SIZE)};
          // FIXME: Collide layers still is not done
          if (game->collide_layers[0].overlaps_activator(reactor.react_box, pos,
                                                         nullptr, nullptr)) {
            reactor.on_react(x, y, tile_layer);
          }
        }
      }
    }
  }
}
