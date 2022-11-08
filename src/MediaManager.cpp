#include "MediaManager.hpp"
#include <iostream>

SDL_Texture *MediaManager::readIMG(SDL_Renderer *renderer, std::string filename) {
    
    if (images.find(filename) == images.end()) {
        
        SDL_Surface *surface = IMG_Load(filename.c_str());
        //surface = 

        if (surface == NULL) {
            printf("Image error: %s\n", SDL_GetError());
            abort();
        }

        SDL_Texture *image = SDL_CreateTextureFromSurface(renderer, surface);

        SDL_FreeSurface(surface);
        images[filename] = image;

        return image;	
    }
    
    SDL_Texture* texture = images[filename];

    return texture;
}

SDL_Texture *MediaManager::readSurface(SDL_Renderer *renderer, std::string filename) {
    
    if (images.find(filename) == images.end()) { 
        SDL_Surface *surface = IMG_Load(filename.c_str());

        if (surface == NULL) {
            printf("Image error: %s\n", SDL_GetError());
            abort();
        }

        SDL_Texture *image = SDL_CreateTextureFromSurface(renderer, surface);
        
        surfaces[filename] = surface;

        SDL_FreeSurface(surface);

        return image;	
    }

    SDL_Surface *surface = surfaces[filename];
    SDL_Texture *image = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    return image;
}

Mix_Chunk *MediaManager::readWAV(std::string filename) { 
    if (sounds.find(filename) == sounds.end()) {  
        Mix_Chunk *waves = Mix_LoadWAV(filename.c_str());
        
        if(!waves) {
            printf("Sound error: %s\n", SDL_GetError());
            abort();
        }

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

SDL_Texture *MediaManager::showFont(SDL_Renderer *renderer, TTF_Font *font, char *text, SDL_Color color) {   
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


SDL_Texture *MediaManager::readTile(SDL_Renderer *renderer, std::string filename) {
    if (tiles.find(filename) == tiles.end()) {
        SDL_Texture *tile = IMG_LoadTexture(renderer, filename.c_str());

        if (!tile) {
            printf("Tile load failed: %s\n", IMG_GetError());
            abort();
        }

        return tile;
    }

    SDL_Texture *tile = tiles[filename];
    return tile;
}

MediaManager mediaManager;