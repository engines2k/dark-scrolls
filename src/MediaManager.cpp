#include "MediaManager.hpp"
#include <iostream>

SDL_Texture *MediaManager::readIMG(SDL_Renderer *renderer, std::string filename, SDL_Rect &SrcR) {
    
    if (images.find(filename) == images.end()) {
        SDL_Texture *image = NULL;
        SDL_Surface *surface = NULL;
        surface = IMG_Load(filename.c_str());

        if (surface == NULL) {
            printf("Image error: %s\n", SDL_GetError());
            abort();
        }

        SrcR.x = 0;
        SrcR.y = 0;

        SrcR.w = surface->w;
        SrcR.h = surface->h;

        image = SDL_CreateTextureFromSurface(renderer, surface);

        SDL_FreeSurface(surface);
        images[filename] = TextureInfo(image);

        return image;	
    }
    
    TextureInfo texture = images[filename];
    SrcR = texture.srcRect;  

    return texture.texture;
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

// create an overloaded function for other text render thingy see text.cpp ln 64 & 65, 18 & 19, 76 & 77
    // rember to move the free surfaces into function and remove them from text.cpp

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