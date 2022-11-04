#pragma once
#include <SDL2/SDL.h>
#include <vector>
#include "pos.hpp"

class Game;
class Sprite;

class FocusPoint : public Translation
{
    Sprite &subject;
    float weight;

public:
    FocusPoint(Sprite &s, float w);
    Translation get_trans();
};

class Camera
{
    Game &game;
    std::vector<FocusPoint> focus_points;
    const float default_scale = 3;          // Defaults to zooming in 300%.

    float calc_center();
    float calc_zoom();

public:
    Camera(Game &game) : game(game) {}
    SDL_Rect* rect_zscale(SDL_Rect *r);

    // Wrappers for SDL render functions. Applies zoom scale.
    int render(     SDL_Renderer *  renderer,
                    SDL_Texture * texture,
                    const SDL_Rect * srcrect,
                    SDL_Rect * dstrect);

    int render_ex(  SDL_Renderer * renderer,
                    SDL_Texture * texture,
                    const SDL_Rect * srcrect,
                    SDL_Rect * dstrect,
                    const double angle,
                    const SDL_Point * center,
                    const SDL_RendererFlip flip);
};