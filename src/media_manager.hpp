#pragma once

#include "util.hpp"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>
#include <filesystem>
#include <map>
#include <memory>
#include <string>
#include <type_traits>
#include <typeinfo>
#include <typeindex>

// This type must be default constructible
template <typename MT>
struct MediaFactory {
  // The type used to lookup the piece of media
  // KeyType must implement operator<
  // using KeyType = 0
  //
  // The type this piece of media will produce
  using MediaType = MT;
  //
  // Creates an instance of the media
  // MediaType construct(MediaManager &media, const KeyType &key) = 0
  //
  // Unloades the piece of media
  // void unload(MediaManager &media, const KeyType &key, MediaType media) = 0
  //
  // If true flush the media cache on texture invalidation
  bool flush_on_texture_invalid() {
    return false;
  }

  // If true call texture_reload on each MediaType 
  // when textures are invalidated
  bool reload_on_texture_invalid() {
    return false;
  }
  void texture_reload(MediaType media) {}
};

template <typename KeyType> KeyType normallize_media_key(KeyType key) {
  return key;
}

template <>
std::filesystem::path normallize_media_key(std::filesystem::path path);

class MediaManager;

struct SubMediaManagerBase {
  virtual void on_texture_invalid(MediaManager &media) = 0;
  virtual void unload_all(MediaManager &media) = 0;

  virtual ~SubMediaManagerBase() {}
};

template <typename Factory> struct SubMediaManager : public SubMediaManagerBase {
  std::map<typename Factory::KeyType, typename Factory::MediaType> media;
  Factory factory;

  virtual void on_texture_invalid(MediaManager &media_man) override {
    if (factory.flush_on_texture_invalid()) {
      unload_all(media_man);
    }
    if (factory.reload_on_texture_invalid()) {
      for (auto &item: media) {
        factory.texture_reload(item.second);
      }
    }
  }

  virtual void unload_all(MediaManager &media_man) override {
    for (auto &item: media) {
      factory.unload(media_man, item.first, item.second);
    }
    media.clear();
  }
};

class Game;

class MediaManager {
  std::map<std::type_index, std::unique_ptr<SubMediaManagerBase>> sub_managers;
  Game &game;

  template <typename Factory> static void check_factory() {
    static_assert(std::is_base_of_v<MediaFactory<typename Factory::MediaType>, Factory>,
                  "Type Factory must be a MediaFactory");
  }

  template <typename Factory>
  SubMediaManager<std::decay_t<Factory>> &get_sub_manager() {
    using F = std::decay_t<Factory>;
    check_factory<F>();
    using SM = SubMediaManager<F>;
    std::type_index sub_man_id = typeid(SM);
    auto sub_man_iter = sub_managers.find(sub_man_id);
    SM *sub_man;
    if (sub_man_iter == sub_managers.end()) {
      auto new_sub_man = std::make_unique<SM>();
      sub_man = new_sub_man.get();
      sub_managers[sub_man_id] = std::move(new_sub_man);
    } else {
      sub_man = static_cast<SM *>(sub_man_iter->second.get());
    }
    return *sub_man;
  }

public:
  template <typename Factory>
  typename std::decay_t<Factory>::MediaType
  read(const typename std::decay_t<Factory>::KeyType key) {
    typename std::decay_t<Factory>::KeyType normal_key =
        normallize_media_key(key);
    auto &sub_man = get_sub_manager<Factory>();
    auto media_iter = sub_man.media.find(normal_key);
    if (media_iter == sub_man.media.end()) {
      auto media = sub_man.factory.construct(*this, normal_key);
      sub_man.media[normal_key] = media;
      return media;
    } else {
      return media_iter->second;
    }
  }

  MediaManager(Game &game);
  ~MediaManager();

  SDL_Renderer *get_renderer();
  Game &get_game();
  SDL_Texture *readTexture(const std::filesystem::path &path);
  SDL_Surface *readSurface(const std::filesystem::path &path);
  Mix_Chunk *readWAV(const std::filesystem::path &path);
  Mix_Music *readMusic(const std::filesystem::path &path);
  TTF_Font *readFont(const std::filesystem::path &path, int size);
  SDL_Texture *showFont(TTF_Font *font, char *text, SDL_Color color);
  void flushTextureCache();
  void unloadAll();
};
