#pragma once

constexpr int SUBPIXELS_IN_PIXEL = 1 << 15;
constexpr int TILE_SIZE = 32;
constexpr int TILE_SUBPIXEL_SIZE = TILE_SIZE * SUBPIXELS_IN_PIXEL;

struct Translation;

struct Pos {
  int layer;
  int x;
  int y;

  int tile_scaled_x() const;
  int tile_scaled_y() const;
  int pixel_scaled_x() const;
  int pixel_scaled_y() const;

  int tile_x() const;
  int tile_y() const;
  int pixel_x() const;
  int pixel_y() const;

  Pos operator+(const Translation &trans) const;
  Pos &operator+=(const Translation &trans);
  bool operator==(const Pos &other);
  bool operator!=(const Pos &other);
};

struct Translation {
  int x;
  int y;

  Translation operator+(Translation other) const;
  Translation &operator+=(Translation other);
};
