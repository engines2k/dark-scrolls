#include "fixture.hpp"

Fixture::Fixture(Game &game, Pos pos) : Sprite(game, pos) {
	frameData = new FrameData(game);
};

void Fixture::tick() {
	Sprite::tick();
}