#pragma once
#include "sprite.hpp"
#include "animation.hpp"

class Game;
class SDL_Rect;


class Fixture : public Sprite {
public:
	Fixture(Game &game, Pos pos);
	virtual void tick();
	virtual void draw() = 0;
protected:
	const char *spritename = "SPRITE_NAME_NOT_SET";
	FrameData *frameData;
	virtual ~Fixture() {}
};