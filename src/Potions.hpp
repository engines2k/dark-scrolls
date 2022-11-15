#pragma once

#include "Item.hpp"

class HealthPotion: public Item {
    public:
        HealthPotion(Game &game, Pos pos) : Item(game, pos) {
            filename = "data/sprite/healing_potion.png";
        }

        virtual void draw() {
            Item::draw();

            if (player_collide) {
                Text t((char*)"+15 hp!", game, pos);
                game.sprite_list.push_back(std::make_shared<Text>(t));

                Mix_Chunk *s = game.media.readWAV("data/sound/heal.wav");
                Mix_PlayChannel(-1, s, 0);
            }
        }

        virtual void tick() {
            Item::tick();
            if (player_collide) {
                if (game.player->get_health() <= 100 - heal_amount) // Makes sure player hp doesn't go over 100
                    game.player->heal(heal_amount); // Increases players hp by heal_amount
                else // players' health was too high to heal him heal_amount
                    game.player->set_health(100); // heal him to 100
            }
        }

        void set_heal_amount(int num) {
            heal_amount = num;
        }

        protected:
            int heal_amount = 15;
};