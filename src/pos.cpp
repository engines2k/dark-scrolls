#include "pos.hpp"

// Pos
int Pos::tile_scaled_x() const { return tile_x() * TILE_SUBPIXEL_SIZE; }

int Pos::tile_scaled_y() const { return tile_y() * TILE_SUBPIXEL_SIZE; }

int Pos::pixel_scaled_x() const { return pixel_x() * SUBPIXELS_IN_PIXEL; }

int Pos::pixel_scaled_y() const { return pixel_y() * SUBPIXELS_IN_PIXEL; }

int Pos::tile_x() const { return x / TILE_SUBPIXEL_SIZE; }

int Pos::tile_y() const { return y / TILE_SUBPIXEL_SIZE; }

int Pos::pixel_x() const { return x / SUBPIXELS_IN_PIXEL; }

int Pos::pixel_y() const { return y / SUBPIXELS_IN_PIXEL; }

Pos Pos::operator+(const Translation &trans) const {
	Pos new_pos = *this;
	new_pos += trans;
	return new_pos;
}

bool Pos::operator==(const Pos &other) {
	return this->layer == other.layer && this->x == other.x &&
  			 this->y == other.y;
}

bool Pos::operator!=(const Pos &other) { return !(*this == other); }

// Translation
Translation Translation::operator+(Translation other) const {
	Translation new_trans = *this;
	new_trans += other;
	return new_trans;
}

Translation &Translation::operator+=(Translation other) {
	this->x += other.x;
	this->y += other.y;
	return *this;
}

inline Pos &Pos::operator+=(const Translation &trans) {
	x += trans.x;
	y += trans.y;
	return *this;
}