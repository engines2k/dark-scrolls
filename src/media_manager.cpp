#include "media_manager.hpp"
#include <iostream>

MediaManager::MediaManager(SDL_Renderer *renderer) {
  this->renderer = renderer;
}

SDL_Surface *MediaManager::readSurface(std::string filename) {
  auto surface_iter = surfaces.find(filename);
  if (surface_iter == surfaces.end()) {

    SDL_Surface *surface = IMG_Load(filename.c_str());
    // surface =

    if (surface == NULL) {
      printf("Image error: %s\n", SDL_GetError());
      abort();
    }

    surfaces[filename] = surface;

    return surface;
  }

  return surface_iter->second;
}

SDL_Texture *MediaManager::readTexture(std::string filename) {
  auto texture_iter = textures.find(filename);
  if (texture_iter == textures.end()) {
    SDL_Surface *surface = readSurface(filename.c_str());

    if (surface == NULL) {
      printf("Texture error: %s\n", SDL_GetError());
      abort();
    }

    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);

    textures[filename] = texture;

    return texture;
  }

  return texture_iter->second;
}

Mix_Chunk *MediaManager::readWAV(std::string filename) {
  if (sounds.find(filename) == sounds.end()) {
    Mix_Chunk *waves = Mix_LoadWAV(filename.c_str());

    if (!waves) {
      printf("Sound error: %s\n", SDL_GetError());
      abort();
    }

    sounds[filename] = waves;

    return waves;
  }

  Mix_Chunk *waves = sounds[filename];
  return waves;
}

TTF_Font *MediaManager::readFont(std::string filename, int size) {
  if (fonts.find(filename) == fonts.end()) {
    TTF_Font *font = TTF_OpenFont(filename.c_str(), size);

    if (!font) {
      printf("Font error: %s\n", SDL_GetError());
      abort();
    }

    return font;
  }

  TTF_Font *font = fonts[filename];
  return font;
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

  texture = SDL_CreateTextureFromSurface(renderer, surface);

  SDL_FreeSurface(surface);

  return texture;
}

void MediaManager::flushTextureCache() {
  for (auto &texture : textures) {
    SDL_DestroyTexture(texture.second);
  }
  textures.clear();
}
