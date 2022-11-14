#pragma once

#include "Sprite.hpp"
#include "Player.hpp"
#include "MediaManager.hpp"
#include "Text.hpp"
#include <iostream>

class Item: public Sprite{
    public:
        Item(Game &game, Pos p) : Sprite(game, p) {
            ReactorCollideBox temp( // what can hit/ effect it
                ReactorCollideType::INTERACTABLE,
                0 * SUBPIXELS_IN_PIXEL,
                64 * SUBPIXELS_IN_PIXEL,
                0 * SUBPIXELS_IN_PIXEL,
                64 * SUBPIXELS_IN_PIXEL
            );

            reactbox = temp;

            ActivatorCollideBox temp2( // What it can hit/ effect
                ActivatorCollideType::HIT_GOOD | ActivatorCollideType::INTERACT,
                0 * SUBPIXELS_IN_PIXEL,
                64 * SUBPIXELS_IN_PIXEL,
                0 * SUBPIXELS_IN_PIXEL,
                64 * SUBPIXELS_IN_PIXEL
            );

            hitbox = temp2;

            hitbox.damage.hp_delt = 0; // This is needed to not launch the player off the screen

            activators.push_back(hitbox);
            reactors.push_back(reactbox); 
               
        }

        virtual void draw() {
            SDL_Rect my_rect = SHAPE;
            my_rect.x = pos.x;
            my_rect.y = pos.y;
            
            SDL_Texture *texture = game.media.readTexture("data/sprite/health_potion.png");
            game.camera->render(game.renderer, texture, NULL, &my_rect);
        }

        virtual void tick() { 
            // Check for player coliding with Item
            for (auto &reactor: reactors) { 
                if (hitbox.collides_with(pos, reactor, game.player->get_pos())) {
                    int heal_amount = 15;
                    if (game.player->get_health() <= 100 - heal_amount) // Makes sure player hp doesn't go over 100
                        game.player->heal(heal_amount); // Increases players hp by heal_amount
                    else // players' health was too high to heal him heal_amount
                        game.player->set_health(100); // heal him to 100
                    despawn();

                    Text t((char*)"+15 hp!", game, pos);
                    t.draw();
                }
            }

        }
    
    protected:
        std::string filename;
        ReactorCollideBox reactbox;
        ActivatorCollideBox hitbox;
        bool player_collide = false;
        static constexpr SDL_Rect SHAPE = {.x = 0, .y = 0, .w = 64, .h = 64};
};

