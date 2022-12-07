#include "item.hpp"
#include "game.hpp"

Item::Item(Game &game, Pos p) : Sprite(game, p) {
  ReactorCollideBox temp( // what can hit/ effect it
      ReactorCollideType::INTERACTABLE, 0 * SUBPIXELS_IN_PIXEL,
      64 * SUBPIXELS_IN_PIXEL, 0 * SUBPIXELS_IN_PIXEL, 64 * SUBPIXELS_IN_PIXEL);

  reactbox = temp;

  ActivatorCollideBox temp2( // What it can hit/ effect
      ActivatorCollideType::HIT_GOOD | ActivatorCollideType::INTERACT,
      1 * SUBPIXELS_IN_PIXEL, 16 * SUBPIXELS_IN_PIXEL, 0 * SUBPIXELS_IN_PIXEL,
      16 * SUBPIXELS_IN_PIXEL);

  hitbox = temp2;

  activators.push_back(hitbox);
  reactors.push_back(reactbox);
}

void Item::draw() {
  SDL_Rect my_rect = SHAPE;
  my_rect.x = pos.x;
  my_rect.y = pos.y;

  SDL_Texture *texture = game.media.readTexture(filename.c_str());
  game.camera->render(game.renderer, texture, NULL, &my_rect);
}

void Item::tick() {
  // Check for player coliding with Item
  for (auto &reactor : reactors) {
    if (hitbox.collides_with(pos, reactor, game.player->get_pos())) {
      game.inventory->add_item(*this);
      despawn();
      player_collide = true;
    }
  }
}

void Item::use() {}

std::string Item::get_filename() { return filename; }

Item &Item::operator=(const Item &other) {
  this->filename = other.filename;
  this->reactbox = other.reactbox;
  this->hitbox = other.hitbox;
  this->player_collide = other.player_collide;
  return *this;
}