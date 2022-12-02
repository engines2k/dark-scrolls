#include "media_manager.hpp"
#include "game.hpp"
#include <iostream>

template <>
std::filesystem::path normallize_media_key(std::filesystem::path path) {
  return path.lexically_normal();
}

MediaManager::MediaManager(Game &game): game(game) {}

MediaManager::~MediaManager() {
  unloadAll();
}

Game &MediaManager::get_game() { return game; }
SDL_Renderer *MediaManager::get_renderer() { return game.renderer; }

struct SurfaceFactory : public MediaFactory<SDL_Surface *> {
  using KeyType = std::filesystem::path;
  SDL_Surface *construct(MediaManager &media,
                         const std::filesystem::path &path) {
    std::string path_str = path.u8string();
    SDL_Surface *surface = IMG_Load(path_str.c_str());

    if (surface == NULL) {
      printf("Image error: %s\n", SDL_GetError());
      abort();
    }

    return surface;
  }

  void unload(MediaManager &media, const std::filesystem::path &path,
              SDL_Surface *surface) {
    SDL_FreeSurface(surface);
  }
};

SDL_Surface *MediaManager::readSurface(const std::filesystem::path &path) {
  return read<SurfaceFactory>(path);
}

struct TextureFactory : public MediaFactory<SDL_Texture *> {
  using KeyType = std::filesystem::path;
  SDL_Texture *construct(MediaManager &media,
                         const std::filesystem::path &path) {
    SDL_Surface *surface = media.readSurface(path);

    SDL_Texture *texture =
        SDL_CreateTextureFromSurface(media.get_renderer(), surface);

    if (texture == NULL) {
      printf("Texture error: %s\n", SDL_GetError());
      abort();
    }

    return texture;
  }

  void unload(MediaManager &media, const std::filesystem::path &path,
              SDL_Texture *texture) {
    SDL_DestroyTexture(texture);
  }

  bool flush_on_texture_invalid() {
    return true;
  }
};

SDL_Texture *MediaManager::readTexture(const std::filesystem::path &path) {
  return read<TextureFactory>(path);
}

struct WavFactory : public MediaFactory<Mix_Chunk *> {
  using KeyType = std::filesystem::path;
  Mix_Chunk *construct(MediaManager &media, const std::filesystem::path &path) {
    std::string path_str = path.u8string();
    Mix_Chunk *waves = Mix_LoadWAV(path_str.c_str());

    if (!waves) {
      printf("Sound error: %s\n", SDL_GetError());
      abort();
    }

    return waves;
  }

  void unload(MediaManager &media, const std::filesystem::path &path,
              Mix_Chunk *sfx) {
    Mix_FreeChunk(sfx);
  }
};

Mix_Chunk *MediaManager::readWAV(const std::filesystem::path &path) {
  return read<WavFactory>(path);
}

struct MusicFactory : public MediaFactory<Mix_Music *> {
  using KeyType = std::filesystem::path;
  Mix_Music *construct(MediaManager &media, const std::filesystem::path &path) {
    std::string path_str = path.u8string();
    Mix_Music *song = Mix_LoadMUS(path_str.c_str());

    if (!song) {
      printf("Music error: %s\n", SDL_GetError());
      abort();
    }

    return song;
  }

  void unload(MediaManager &media, const std::filesystem::path &path,
              Mix_Music *song) {
    Mix_FreeMusic(song);
  }
};

Mix_Music *MediaManager::readMusic(const std::filesystem::path &path) {
  return read<MusicFactory>(path);
}

struct FontKey {
  FontKey(std::filesystem::path path, int size) {
    this->path = path;
    this->size = size;
  }

  std::filesystem::path path;
  int size;

  bool operator<(const FontKey &other) const {
    return path == other.path ? size < other.size : path < other.path;
  }

  bool operator<=(const FontKey &other) const {
    return path == other.path ? size <= other.size : path < other.path;
  }

  bool operator==(const FontKey &other) const {
    return path == other.path && size == other.size;
  }

  bool operator!=(const FontKey &other) const { return !(*this == other); }

  bool operator>(const FontKey &other) const {
    return path == other.path ? size > other.size : path > other.path;
  }

  bool operator>=(const FontKey &other) const {
    return path == other.path ? size >= other.size : path > other.path;
  }
};

template <> FontKey normallize_media_key(FontKey key) {
  return FontKey(normallize_media_key(key.path), key.size);
}

struct FontFactory : public MediaFactory<TTF_Font *> {
  using KeyType = FontKey;
  TTF_Font *construct(MediaManager &media, const FontKey &key) {
    std::string path_str = key.path.u8string();
    TTF_Font *font = TTF_OpenFont(path_str.c_str(), key.size);

    if (!font) {
      printf("Font error: %s\n", SDL_GetError());
      abort();
    }

    return font;
  }

  void unload(MediaManager &media, const FontKey &key, TTF_Font *font) {
    TTF_CloseFont(font);
  }
};

TTF_Font *MediaManager::readFont(const std::filesystem::path &path, int size) {
  auto key = FontKey(path, size);
  return read<FontFactory>(key);
}

SDL_Texture *MediaManager::showFont(TTF_Font *font, char *text,
                                    SDL_Color color) {
  SDL_Texture *texture = NULL;
  SDL_Surface *surface = NULL;

  surface = TTF_RenderText_Solid(font, text, color);

  if (surface == NULL) {
    printf("Font Render Error: %s\n", SDL_GetError());
    abort();
  }

  texture = SDL_CreateTextureFromSurface(get_renderer(), surface);

  SDL_FreeSurface(surface);

  return texture;
}

void MediaManager::flushTextureCache() {
  for (auto &sub_man: sub_managers) {
    sub_man.second->on_texture_invalid(*this);
  }
}

void MediaManager::unloadAll() {
  for (auto &sub_man: sub_managers) {
    sub_man.second->unload_all(*this);
  }
}
