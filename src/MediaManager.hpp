#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <string>
#include <map>

class MediaManager {
	std::map <std::string, SDL_Texture*> textures;
	std::map <std::string, SDL_Surface*> surfaces;
  std::map <std::string, Mix_Chunk*> sounds;
  std::map <std::string, TTF_Font*> fonts;
  SDL_Renderer* renderer;

	public:
    MediaManager(SDL_Renderer* renderer);
		SDL_Texture *readTexture(std::string filename);
		SDL_Surface *readSurface(std::string filename);
		Mix_Chunk *readWAV(std::string filename);
    TTF_Font *readFont(std::string filename, int size);
		SDL_Texture *showFont(TTF_Font *font, char *text, SDL_Color color);
    void flushTextureCache();
};
