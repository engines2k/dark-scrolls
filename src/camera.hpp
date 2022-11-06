#pragma once
#include <SDL2/SDL.h>
#include "pos.hpp"
/*
class Game;

class FocusPoint : public Translation {

	
};

class Camera {
private:
	Game &game;
	std::vector<FocusPoint> focus_points[];

	float get_zoom() {
		return game.get_camera_zoom;
	}

	SDL_Rect *rect_scaled(SDL_Rect *r) {
		r->x *= get_zoom();
		r->y *= get_zoom();
		r->w *= get_zoom();
		r->h *= get_zoom();

		return r;
	}

public:
	int render(	SDL_Renderer *renderer,
				SDL_Texture *texture,
				const SDL_Rect *srcrect,
				SDL_Rect *dstrect) {

		return SDL_RenderCopy(renderer, texture, srcrect, rect_scaled(dstrect));
	}

	int render_ex(	SDL_Renderer * renderer,
                   	SDL_Texture * texture,
                   	const SDL_Rect * srcrect,
                   	SDL_Rect * dstrect,
                   	const double angle,
                   	const SDL_Point *center,
                   	const SDL_RendererFlip flip) {

		return SDL_RenderCopyEx(renderer, texture, srcrect, dstrect, angle, center, flip);

	}
};*/