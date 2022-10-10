#pragma once

constexpr int SUBPIXELS_IN_PIXEL = 1 << 15;
constexpr int TILE_SIZE = 32;
constexpr int TILE_SUBPIXEL_SIZE = TILE_SIZE * SUBPIXELS_IN_PIXEL; 

struct Translation;

struct Pos {
  int layer;
  int x;
  int y;

  int tile_x() const {
    return x / SUBPIXELS_IN_PIXEL / TILE_SIZE;
  }

  int tile_y() const {
    return y / SUBPIXELS_IN_PIXEL / TILE_SIZE;
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

class BoundingBox {
  public:
    BoundingBox(Pos corner, int width, int height) {
      this->left_corner = corner;
      this->width = width;
      this->height = height;
    }

    bool collides_with(const BoundingBox& other) {
      int this_x = this->left_corner.x;
      int this_y = this->left_corner.y;
      int other_x = other.left_corner.x;
      int other_y = other.left_corner.y;

      return this->left_corner.layer == other.left_corner.layer &&
        this_x < other_x + other.width &&
        this_x + this->width > other_x &&
        this_y < other_y + other.height &&
        this_y + this->height > other_y;
    }
    Pos left_corner;
    int width;
    int height;
};
