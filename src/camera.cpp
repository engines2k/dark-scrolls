#include "camera.hpp"
#include <cstdio>

const int WIDTH = 800, HEIGHT = 600;

FocusPoint::FocusPoint(Pos &npos, float nweight): pos(npos) {
	weight = nweight;
}

Camera::Camera(Game &game): game(game){
	zoom_default = 1;
}

void Camera::add_focus(std::shared_ptr<Sprite> sprite) {
	focus_points.push_back(sprite);
}

void Camera::calc_offset() {
	// Calculate offset translation (camera center)
	// to be the average of all focus points
	Translation t = {0, 0};
	int fp_size = focus_points.size();
	if(fp_size > 0) {
		for(auto sprite: focus_points) {
			Pos p = sprite->get_pos();
			t.x += p.x;
			t.y += p.y;
		}
		t.x /= fp_size;
		t.y /= fp_size;
		t.x /= SUBPIXELS_IN_PIXEL;
		t.y /= SUBPIXELS_IN_PIXEL;
		t.x -= WIDTH / 2;
		t.y -= HEIGHT / 2;
	}

	offset = t;
	// printf("fp_size: %i x: %i y: %i\n", fp_size, t.x, t.y);
}

float Camera::get_zoom() {
	float z;
	if(focus_points.size() <= 1)
		z = zoom_default;
	else {
		int x_center = WIDTH / 2;
		int y_center = HEIGHT / 2;
		int x_min = x_center;
		int x_max = x_center;
		int y_min = y_center;
		int y_max = y_center;
		for(auto sprite: focus_points) {
			Pos p = sprite->get_pos();
			if(p.x < x_min)
				x_min = p.x;
			else if (p.x < x_max)
				x_max = p.x;

			if(p.y < y_min)
				y_min = p.y;
			else if (p.x < x_max)
				y_max = p.y;
		}

		int delt = x_max - x_min;
		if((y_max - y_min) * WIDTH / HEIGHT > delt)
			delt = y_max - y_min;

		int target_view = delt * 1.2; // Give padding on the sides
		z = target_view / WIDTH;
	}

	return z;
}

SDL_Rect Camera::rect_scaled(SDL_Rect r) {
	SDL_Rect result = r;
	int z = get_zoom();
	result.w = static_cast<int>(result.w * z);
	result.h = static_cast<int>(result.h * z);

	return result;
}

