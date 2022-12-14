#include "potions.hpp"
#include "game.hpp"

// *********** Health Potion *************
HealthPotion::HealthPotion(Game &game, Pos pos) : Item(game, pos) {
  filename = "data/sprite/healing_potion.png";
}

void HealthPotion::draw() {
  Item::draw();

  if (player_collide) {
    Mix_Chunk *s = game.media.readWAV("data/sound/heal.wav");
    Mix_PlayChannel(-1, s, 0);
    game.sprite_list.push_back(std::make_shared<AppearingText>(AppearingText((char*)"Health Potion!", game, pos, 2, true, {255, 255, 255})));
  }
}

void HealthPotion::tick() {
  Item::tick();
}

void HealthPotion::use() {
  if (game.player->get_health() <=
      100 - heal_amount) // Makes sure player hp doesn't go over 100
    game.player->heal(heal_amount); // Increases players hp by heal_amount
  else // players' health was too high to heal him heal_amount
    game.player->set_health(100); // heal him to 100

  Mix_Chunk *s = game.media.readWAV("data/sound/heal.wav");
  Mix_PlayChannel(-1, s, 0);
}

void HealthPotion::set_heal_amount(int num) { heal_amount = num; }

// *********** Speed Potion *************
SpeedPotion::SpeedPotion(Game &game, Pos pos) : Item(game, pos) {
  filename = "data/sprite/speed_potion.png";
}

void SpeedPotion::draw() {
  Item::draw();

  if (player_collide) {
    Mix_Chunk *s = game.media.readWAV("data/sound/heal.wav");
    Mix_PlayChannel(-1, s, 0);
    game.sprite_list.push_back(std::make_shared<AppearingText>(AppearingText((char*)"Speed Potion!", game, pos, 2, true, {255, 255, 255})));
  }
}

void SpeedPotion::tick() { Item::tick(); }

void SpeedPotion::use() {
  game.player->speed_mod += speed_mod;

  Mix_Chunk *s = game.media.readWAV("data/sound/heal.wav");
  Mix_PlayChannel(-1, s, 0);
}

void SpeedPotion::set_speed_mod(int num) { speed_mod = num; }
