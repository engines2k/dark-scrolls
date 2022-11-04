#include <SDL2/SDL.h>
#include "camera.hpp"
#include "pos.hpp"
#include "sprite.hpp"

// FOCUS POINT

FocusPoint::FocusPoint(Sprite &s, float w=1) : subject(s)
{
    weight = w;
}

Translation FocusPoint::get_trans()
{
    Pos p = subject.get_pos();
    Translation t = {p.x, p.y};
    return t;
}

// CAMERA

float Camera::calc_center() // WIP
{
    return 0;
}

float Camera::calc_zoom() // WIP
{
    // int sig_x = 0;
    // int sig_y = 0;
    // for(auto fp: focus_points)
    // {
    //     Pos p = fp.subject->get_pos();
    // }

    return 0;
}

SDL_Rect* Camera::rect_zscale(SDL_Rect *r) // WIP
{
    // r->x *= calc_zoom();
    // r->y *= calc_zoom();
    // r->w *= calc_zoom();
    // r->h *= calc_zoom();
    return r;
}

int Camera::render( SDL_Renderer * renderer,
            SDL_Texture * texture,
            const SDL_Rect * srcrect,
            SDL_Rect * dstrect)
{
    SDL_Rect *scaled_dstrect = rect_zscale(dstrect);
    return SDL_RenderCopy(renderer, texture, srcrect, scaled_dstrect);
}

int Camera::render_ex(  SDL_Renderer * renderer,
                SDL_Texture * texture,
                const SDL_Rect * srcrect,
                SDL_Rect * dstrect,
                const double angle,
                const SDL_Point *center,
                const SDL_RendererFlip flip)
{
    SDL_Rect *scaled_dstrect = rect_zscale(dstrect);
    return SDL_RenderCopyEx(renderer, texture, srcrect, scaled_dstrect, angle, center, flip);
}