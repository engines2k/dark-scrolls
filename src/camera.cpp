#include "camera.hpp"
#include <cstdlib>

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
			o.x = o.x + p.x / SUBPIXELS_IN_PIXEL;
			o.y = o.y + p.y / SUBPIXELS_IN_PIXEL;
		}
		o.x = (o.x / fp_size) - (WIDTH / 2 / zoom_factor);
		o.y = (o.y / fp_size) - (HEIGHT / 2 / zoom_factor);
	}
	offset = o;
}

void Camera::calc_zoom() {
	float z;
	if(focus_points.size() <= 1)
		z = zoom_default;
	else {
		// Find smallest and largest x and y positions among all focus points
		int	x_min = focus_points[0]->get_pos().x / SUBPIXELS_IN_PIXEL, x_max = focus_points[0]->get_pos().x / SUBPIXELS_IN_PIXEL;

		for(auto sprite: focus_points) {
			Pos p = sprite->get_pos();
			p.x /= SUBPIXELS_IN_PIXEL;

			if(&sprite == &focus_points.front()){
				x_min = p.x;
				x_max = p.x;
			}

			if(p.x < x_min) x_min = p.x;
			else if (p.x < x_max) x_max = p.x;
			
		}

		int delt = abs(x_max - x_min);

		// Choose greatest delt for zoom and axis

		// if(delt < WIDTH) delt = WIDTH;	// Failsafe for small delts

		z = zoom_default; static_cast<float>(WIDTH) / delt;

	}

	// zoom_factor = z; // Commented out for testing
	zoom_factor = z;
}

SDL_Rect Camera::rect_offsetted(SDL_Rect r) {
	SDL_Rect result = r;

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

SDL_Rect Camera::rect_processed(SDL_Rect r) {
	SDL_Rect result = r;
	result.x /= SUBPIXELS_IN_PIXEL;
	result.y /= SUBPIXELS_IN_PIXEL;
	result.x -= 16 * zoom_factor;
	result.y -= 16 * zoom_factor;
	result = rect_offsetted(result);
	result = rect_scaled(result);

	return result;
}

int Camera::render(	SDL_Renderer *renderer,
					SDL_Texture *texture,
					const SDL_Rect *srcrect,
					const SDL_Rect *dstrect) {

	SDL_Rect processed_rect = rect_processed(*dstrect);

	return SDL_RenderCopy(renderer, texture, srcrect, &processed_rect);
}

int Camera::render_ex(	SDL_Renderer * renderer,
		               	SDL_Texture * texture,
		               	const SDL_Rect * srcrect,
		               	SDL_Rect * dstrect,
		               	const double angle,
		               	const SDL_Point *center,
		               	const SDL_RendererFlip flip) {

	SDL_Rect processed_rect = rect_processed(*dstrect);

	return SDL_RenderCopyEx(renderer, texture, srcrect, &processed_rect, angle, center, flip);

}

int Camera::fill_rect( 	SDL_Renderer * renderer,
						SDL_Rect * dstrect) {

	SDL_Rect processed_rect = rect_processed(*dstrect);

	return SDL_RenderFillRect(renderer, &processed_rect);
}