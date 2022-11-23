#include "camera.hpp"
#include <cstdlib>
#include <iostream>

const int WIDTH = 800, HEIGHT = 600;

Camera::Camera(Game &game) : game(game) {
  zoom_default = 1.5;
  zoom_factor = zoom_default;
}

void Camera::add_focus(std::shared_ptr<Sprite> sprite) {
  focus_points.push_back(sprite);
}

/*
Calculate offset translation (camera center) as the average of all focus points
NOTE: Caclulated as pos value (pixels * SUBPIXELS_IN_PIXEL)
*/

void Camera::calc_eased_offset() {
  int ease_factor = 6;
  eased_offset.x += (offset.x - eased_offset.x) / ease_factor;
  eased_offset.y += (offset.y - eased_offset.y) / ease_factor;
}

void Camera::calc_offset() {
  std::vector<std::shared_ptr<Sprite>> next_focus_points;
  Translation o = {0, 0};
  int active_fps = focus_points.size();
  for (auto sprite : focus_points) {
    if (sprite->is_spawned()) {
      Pos p = sprite->get_pos();
      o.x = o.x + p.x / SUBPIXELS_IN_PIXEL;
      o.y = o.y + p.y / SUBPIXELS_IN_PIXEL;
    } else
      active_fps--;
  }
  if (active_fps > 0) {
    o.x = (WIDTH / 2 / zoom_factor) - (o.x / active_fps);
    o.y = (HEIGHT / 2 / zoom_factor) - (o.y / active_fps);
    offset = o;
  }
  calc_eased_offset();
}

void Camera::calc_zoom() {
  float z;
  if (!zoom_enabled || focus_points.size() <= 1)
    z = zoom_default;
  else {
    // Find smallest and largest x and y positions among all focus points
    int x_min = focus_points[0]->get_pos().x / SUBPIXELS_IN_PIXEL,
        x_max = focus_points[0]->get_pos().x / SUBPIXELS_IN_PIXEL;

    for (auto sprite : focus_points) {
      Pos p = sprite->get_pos();
      if (sprite->is_spawned()) {
        p.x /= SUBPIXELS_IN_PIXEL;

        if (p.x < x_min)
          x_min = p.x;
        else if (p.x > x_max)
          x_max = p.x;
      }
    }

    int delt = x_max - x_min;
    if (delt < 0)
      delt = -delt;
    delt *= 2; // Gives padding on the sides.
    if (delt == 0)
      delt = 1; // Failsafe for small delts

    z = static_cast<float>(WIDTH) / delt;
    z = int(z * 32) /
        32.0; // Precision limit trick, avoids black lines but causes choppiness

    if (z < zoom_default)
      z = zoom_default;
    else if (z > max_zoom)
      z = max_zoom;
  }

  zoom_factor = z;
}

SDL_Rect Camera::rect_offsetted(SDL_Rect r) {
  SDL_Rect result = r;

  // Apply camera offset
  result.x = (result.x + eased_offset.x);
  result.y = (result.y + eased_offset.y);

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

int Camera::render(SDL_Renderer *renderer, SDL_Texture *texture,
                   const SDL_Rect *srcrect, const SDL_Rect *dstrect) {

  SDL_Rect processed_rect = rect_processed(*dstrect);

  return SDL_RenderCopy(renderer, texture, srcrect, &processed_rect);
}

int Camera::render_ex(SDL_Renderer *renderer, SDL_Texture *texture,
                      const SDL_Rect *srcrect, SDL_Rect *dstrect,
                      const double angle, const SDL_Point *center,
                      const SDL_RendererFlip flip) {

  SDL_Rect processed_rect = rect_processed(*dstrect);

  return SDL_RenderCopyEx(renderer, texture, srcrect, &processed_rect, angle,
                          center, flip);
}

int Camera::fill_rect(SDL_Renderer *renderer, SDL_Rect *dstrect) {

  SDL_Rect processed_rect = rect_processed(*dstrect);

  return SDL_RenderFillRect(renderer, &processed_rect);
}
