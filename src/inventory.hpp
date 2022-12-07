#pragma once

#include "item.hpp"
#include "potions.hpp"
#include <SDL2/SDL.h>
#include <vector>

class Game;

class InventoryBlock : public Sprite{
	SDL_Texture* image = NULL;

	public:
		InventoryBlock(Game &game, Pos pos) : Sprite(game, pos), game(game) {}

		void add_item(Item item);
		void use_item();
		void reset_block();
		
		virtual void draw();
		virtual void tick();
	
		void set_position(Pos new_pos);
		void set_button(SDL_Scancode key);
		SDL_Scancode get_button();
		std::string get_item_name();
		std::string get_item_type();

		InventoryBlock &operator=(const InventoryBlock &other);

	protected:
		Game &game;
		SDL_Scancode button;
		std::string item_name = "";
		std::string item_type = "";
		int counter = 0;
};

class Inventory : public InventoryBlock { 
	int stored_items = 0;
		public:
			std::vector<InventoryBlock> inventory; // Vector of inventory blocks
			
			Inventory(Game &game, Pos pos);

			virtual void draw();
			virtual void tick();

			int find_open_block(Item item);
			void add_item(Item item);
};


 