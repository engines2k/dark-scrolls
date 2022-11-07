#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <string>
#include <map>

const SDL_Rect EmptyRect = {0, 0, 0, 0};

class TextureInfo {
	public:
		SDL_Texture * texture;
		SDL_Rect srcRect;
		TextureInfo(SDL_Texture * newTexture = NULL, const SDL_Rect &newSrcRect = EmptyRect) {
			texture = newTexture;
			srcRect = newSrcRect;
		}
};

class MediaManager {
	std::map <std::string, TextureInfo> images;
    std::map <std::string, Mix_Chunk*> sounds;
    std::map <std::string, TTF_Font*> fonts;
	std::map <std::string, SDL_Texture*> tiles;

	public:
		SDL_Texture *readIMG(SDL_Renderer *renderer, std::string filename, SDL_Rect &SrcR);
		Mix_Chunk *readWAV(std::string filename);
        TTF_Font *readFont(std::string filename, int size);
		SDL_Texture *showFont(SDL_Renderer *renderer, TTF_Font *font, char *text, SDL_Color color);
		SDL_Texture *readTile(SDL_Renderer *renderer, std::string filename);
};

extern MediaManager mediaManager;