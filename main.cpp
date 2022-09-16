#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL_ttf.h>
//could be <SDL.h>

const int WIDTH = 800, HEIGHT = 600;

class Text {
  SDL_Renderer *renderer = NULL;
  SDL_Surface *message = NULL;
  SDL_Surface *surface = NULL;
  TTF_Font *font = TTF_OpenFont("arial.ttf", 25);
  SDL_Texture *texture;
  int texW,texH;
  SDL_Rect dstrect;
  SDL_Color color = { 255, 255, 255 };
  public:
  Text(SDL_Renderer *nRenderer) {
    renderer = nRenderer;
    surface = TTF_RenderText_Solid(font,
    "Welcome to Dark Scrolls", color);
    texture = SDL_CreateTextureFromSurface(renderer, surface);

    texW = 0;
    texH = 0;
    SDL_QueryTexture(texture, NULL, NULL, &texW, &texH);
    dstrect = { 0, 0, texW, texH };
  }

  void loop() {
    SDL_RenderCopy(renderer, texture, NULL, &dstrect);
    SDL_RenderPresent(renderer);
  }
};

int main(int argc, char *argv[]) {
  SDL_Window *window;
  SDL_Renderer *renderer;
  if(SDL_Init(SDL_INIT_EVERYTHING) < 0) {
    printf("SDL_Init failed: %s\n", SDL_GetError());
    return 1;
  }

  TTF_Init();

  window = SDL_CreateWindow("Dark Scrolls",
                                        SDL_WINDOWPOS_UNDEFINED,
                                        SDL_WINDOWPOS_UNDEFINED,
                                        WIDTH, HEIGHT,
                                        SDL_WINDOW_ALLOW_HIGHDPI);
  if(window == NULL) {
    printf("Could not create window: %s\n", SDL_GetError());
    return 1;
  }
  
  renderer = SDL_CreateRenderer(window, -1, 0);
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

  Text t(renderer);

  SDL_RenderClear(renderer);

  SDL_RenderPresent(renderer);
  
  SDL_Event event;
  while(1) {
    if(SDL_PollEvent(&event)) {
      if(event.type == SDL_QUIT) {
        break;
      }
    }
    t.loop();
  }

  SDL_DestroyWindow(window);

  SDL_Quit();
  return 0;
}
