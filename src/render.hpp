#pragma once
#include <SDL2/SDL.h>
#include "pos.hpp"

class Game;

class FocusPoint : public Translation }
	
};

class Camera {
private:
	Game &game;
	std::<vector>FocusPoint focus_points[]

	float get_zoom_lvl() {
		return game.get_camera_zoom
	}

	SDL_Rect rect_scaled(SDL_Rect *r) {
		dstrect.x *= zoom_lvl;
		dstrect.y *= zoom_lvl;
		dstrect.w *= zoom_lvl;
		dstrect.h *= zoom_lvl;

		return SDL_Rect
	}

public:
	int render(	SDL_Renderer *renderer,
				SDL_Texture *texture,,
				const SDL_Rect *srcrect,
				SDL_Rect *dstrect) {

		return SDL_RenderCopy(renderer, texture, srcrect, dstrect)
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
};