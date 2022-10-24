#pragma once

constexpr int SUBPIXELS_IN_PIXEL = 1 << 15;
constexpr int TILE_SIZE = 32;
constexpr int TILE_SUBPIXEL_SIZE = TILE_SIZE * SUBPIXELS_IN_PIXEL; 

struct Translation;

struct Pos {
  int layer;
  int x;
  int y;

  int tile_scaled_x() const {
    return tile_x() * TILE_SUBPIXEL_SIZE;
  }

  int tile_scaled_y() const {
    return tile_y() * TILE_SUBPIXEL_SIZE;
  }

  int pixel_scaled_x() const {
    return pixel_x() * SUBPIXELS_IN_PIXEL;
  }

  int pixel_scaled_y() const {
    return pixel_y() * SUBPIXELS_IN_PIXEL; 
  }

  int tile_x() const {
    return x / TILE_SUBPIXEL_SIZE;
  }

  int tile_y() const {
    return y / TILE_SUBPIXEL_SIZE;
  }

  int pixel_x() const {
    return x / SUBPIXELS_IN_PIXEL;
  }

  int pixel_y() const {
    return y / SUBPIXELS_IN_PIXEL;
  }

  Pos operator+(const Translation& trans) const {
    Pos new_pos = *this;
    new_pos += trans;
    return new_pos;
  }
  Pos& operator+=(const Translation& trans);

  bool operator==(const Pos& other) {
    return this->layer == other.layer &&
      this->x == other.x &&
      this->y == other.y;
  }
  bool operator!=(const Pos& other) {
    return !(*this == other);
  }
};

struct Translation {
  int x;
  int y;

  Translation operator+(Translation other) const {
    Translation new_trans = *this;
    new_trans += other;
    return new_trans;
  }

  Translation& operator+=(Translation other) {
    this->x += other.x;
    this->y += other.y;
    return *this;
  }
};

inline Pos& Pos::operator+=(const Translation& trans) {
  x += trans.x;
  y += trans.y;
  return *this;
}
