#include "camera.hpp"
#include <cstdlib>

// Included for debugging. Not neccesary.
#include <cstdio>
#include "game.hpp"

const int WIDTH = 800, HEIGHT = 600;

Camera::Camera(Game &game): game(game){
	zoom_default = 1;
	zoom_factor = zoom_default;
}

void Camera::add_focus(std::shared_ptr<Sprite> sprite) {
	focus_points.push_back(sprite);
}

/*
Calculate offset translation (camera center) as the average of all focus points
NOTE: Caclulated as pos value (pixels * SUBPIXELS_IN_PIXEL)
*/
void Camera::calc_offset() {

	Translation o = {0, 0};
	int fp_size = focus_points.size();
	if(fp_size > 0)
	{
		for(auto sprite: focus_points) {
			Pos p = sprite->get_pos();
			o.x = o.x + p.x;
			o.y = o.y + p.y;
		}

		o.x = (o.x / fp_size) - (WIDTH * SUBPIXELS_IN_PIXEL / 2);
		o.y = (o.y / fp_size) - (HEIGHT * SUBPIXELS_IN_PIXEL / 2);

		
	}


	offset = o;
	// if(game.frame_counter.rendered_frames % 10 == 0)
	// printf("offset: %i %i\n", offset.x, offset.y);
}

void Camera::calc_zoom() {
	float z;
	if(focus_points.size() <= 1)
		z = zoom_default;
	else {
		// Find smallest and largest x and y positions among all focus points
		int	x_min, x_max, y_min, y_max;

		for(auto sprite: focus_points) {
			
			Pos p = sprite->get_pos();

			if(&sprite == &focus_points.front()){
				x_min = p.x;
				x_max = p.x;
				y_min = p.y;
				y_max = p.y;
			}

			if(p.x < x_min) x_min = p.x;
			else if (p.x < x_max) x_max = p.x;
			
			if(p.y < y_min) y_min = p.y;
			else if (p.y < y_max) y_max = p.y;
		}

		int delt,
			delt_x = abs(x_max - x_min),
			delt_y = abs(y_max - y_min);

		// Choose greatest delt for zoom and axis
		int subpixeled_axis;
		if(delt_x > delt_y)
		{
			subpixeled_axis = WIDTH * SUBPIXELS_IN_PIXEL;
			delt = delt_x;
		}
		else {
			subpixeled_axis = HEIGHT * SUBPIXELS_IN_PIXEL;
			delt = delt_y;
		} 

		if(delt < subpixeled_axis) delt = subpixeled_axis;	// Failsafe for small delts

		z = static_cast<float>(WIDTH) / delt;
		// printf("delt(*1.2): %i\nx_min: %i\nx_max: %i\ny_min: %i\ny_max: %i\nz: %f\n", delt, x_min, x_max, y_min, y_max, z);
	}

	// zoom_factor = z; // Commented out for testing
	zoom_factor = 1;
}

SDL_Rect Camera::rect_offsetted(const SDL_Rect *r) {
	SDL_Rect result = { r->x, r->y, r->w, r->h };

	// Apply camera offset
	result.x = (result.x - offset.x);
	result.y = (result.y - offset.y);

	return result;
}

SDL_Rect Camera::rect_scaled(SDL_Rect r) {
	SDL_Rect result = r;
	result.x = static_cast<int>(result.x * zoom_factor);
	result.y = static_cast<int>(result.y * zoom_factor);
	result.w = static_cast<int>(result.w * zoom_factor);
	result.h = static_cast<int>(result.h * zoom_factor);

	return result;
}
























int Camera::render(	SDL_Renderer *renderer,
					SDL_Texture *texture,
					const SDL_Rect *srcrect,
					const SDL_Rect *dstrect) {

	SDL_Rect offset_dst = rect_offsetted(dstrect);
	offset_dst = rect_scaled(offset_dst);
	offset_dst.x /= SUBPIXELS_IN_PIXEL;
	offset_dst.y /= SUBPIXELS_IN_PIXEL;

	return SDL_RenderCopy(renderer, texture, srcrect, &offset_dst);
}

int Camera::render_ex(	SDL_Renderer * renderer,
		               	SDL_Texture * texture,
		               	const SDL_Rect * srcrect,
		               	SDL_Rect * dstrect,
		               	const double angle,
		               	const SDL_Point *center,
		               	const SDL_RendererFlip flip) {

	SDL_Rect offset_dst = rect_offsetted(dstrect);
	offset_dst = rect_scaled(offset_dst);
	// .w and .h shouldn't be desubbed
	offset_dst.x /= SUBPIXELS_IN_PIXEL;
	offset_dst.y /= SUBPIXELS_IN_PIXEL;

	// if(game.frame_counter.rendered_frames % 10 == 0)
	// printf("offset: %i %i\n", offset.x, offset.y);

	return SDL_RenderCopyEx(renderer, texture, srcrect, &offset_dst, angle, center, flip);

}