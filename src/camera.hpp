#pragma once
#include <SDL2/SDL.h>
#include <vector>
#include <memory>
#include "pos.hpp"
#include "sprite.hpp"
#include <cstdio>

class Game;

struct FocusPoint {
	Pos &pos;
	float weight;
	FocusPoint(Pos &npos, float nweight);
};

class Camera {
private:
	Game &game;
 	std::vector<std::shared_ptr<Sprite>> focus_points;
 	Translation offset = {0, 0};
	float zoom_default;
	
	float get_zoom();
	SDL_Rect rect_scaled(SDL_Rect r);
	SDL_Rect rect_offsetted(SDL_Rect r);

public:
	void calc_offset();
	Camera(Game &game);
	void add_focus(std::shared_ptr<Sprite> sprite);

	/* 
	Wrappers for SDL render functions that apply zoom and offset.

	NOTE: These functions expect dstrect to have been scaled with
	SUBPIXELS_IN_PIXEL. Only render sprites with these functions.
	*/

	int render(	SDL_Renderer *renderer,
				SDL_Texture *texture,
				const SDL_Rect *srcrect,
				const SDL_Rect *dstrect) {
		// Apply camera offset, de-subpixelize
		SDL_Rect offset_dst = { dstrect->x, dstrect->y, dstrect->w, dstrect->h };
		offset_dst.x /= SUBPIXELS_IN_PIXEL;
		offset_dst.y /= SUBPIXELS_IN_PIXEL;
		offset_dst.x -= offset.x;
		offset_dst.y -= offset.y;
		offset_dst = rect_scaled(offset_dst);

		return SDL_RenderCopy(renderer, texture, srcrect, &offset_dst);
	}

	int render_ex(	SDL_Renderer * renderer,
                   	SDL_Texture * texture,
                   	const SDL_Rect * srcrect,
                   	SDL_Rect * dstrect,
                   	const double angle,
                   	const SDL_Point *center,
                   	const SDL_RendererFlip flip) {
		// Apply camera offset, de-subpixelize
		SDL_Rect offset_dst = { dstrect->x, dstrect->y, dstrect->w, dstrect->h };
		offset_dst.x /= SUBPIXELS_IN_PIXEL;
		offset_dst.y /= SUBPIXELS_IN_PIXEL;
		offset_dst.x -= offset.x;
		offset_dst.y -= offset.y;
		offset_dst = rect_scaled(offset_dst);
		
		return SDL_RenderCopyEx(renderer, texture, srcrect, &offset_dst, angle, center, flip);

	}
};