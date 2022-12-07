#include "inventory.hpp"
#include "game.hpp"

// ********* InventoryBlock **********
void InventoryBlock::add_item(Item i) {
	if (counter == 0) {
		image = game.media.readTexture(i.get_filename().c_str());
		item_name = i.get_filename();
	}
	counter ++;
}

void InventoryBlock::use_item() { 
	if (item_name == "") return; // There is no item to use
	if (get_item_type() == "HealthPotion") {
		HealthPotion h(game, pos);
		h.use();
	} else if (get_item_type() == "SpeedPotion") {
		SpeedPotion s(game, pos);
		s.use();
	} else { // Default case
		Item i(game, pos);
		i.use();
	}
	counter --;
	if (counter == 0) reset_block(); // Last item in block is used
}

std::string InventoryBlock::get_item_type() { // Determine what type of item it is
	if (item_name == "data/sprite/healing_potion.png") return "HealthPotion";
	else if (item_name == "data/sprite/speed_potion.png") return "SpeedPotion";
	else return "Item";
}

void InventoryBlock::reset_block() {
	item_name = "";
	image = NULL;
	counter = 0;
}

void InventoryBlock::draw() {
	
	SDL_Rect my_rect = { .x = 0, .y = 0, .w = 32, .h = 32 };
	my_rect.x = pos.x;
	my_rect.y = pos.y;

	if (image != NULL) { // Displays Image of item in block
		SDL_RenderCopy(game.renderer, image, NULL, &my_rect);
	} 
	
	if (counter > 1) { // Displays Count of items in block
		int w = 0, h = 0;
		std::string s = "x" + std::to_string(counter);
		char * c = &s[0];
		TTF_Font *f = game.media.readFont("./data/font/horrendo.ttf", 15);
		SDL_Texture *tex = game.media.showFont(f, (char*)c, {255, 255, 255});
		SDL_QueryTexture(tex, NULL, NULL, &w, &h);
		SDL_Rect r = {pos.x+32, pos.y+16, w, h};
		SDL_RenderCopy(game.renderer, tex, NULL, &r);
		SDL_DestroyTexture(tex);
	}

	// The box around the block
	SDL_SetRenderDrawColor(game.renderer, 255, 255, 255, 225);
	SDL_RenderDrawRect(game.renderer, &my_rect);
}

void InventoryBlock::tick() {}

void InventoryBlock::set_position(Pos new_pos) { pos = new_pos; }

void InventoryBlock::set_button(SDL_Scancode key) { button = key; }

SDL_Scancode InventoryBlock::get_button() { return button; }

std::string InventoryBlock::get_item_name() { return item_name; }

InventoryBlock &InventoryBlock::operator=(const InventoryBlock &other) {
	this->item_name = other.item_name;
	this->counter = other.counter;
	this->image = other.image;
	this->button = other.button;
	return *this;
}


// ********* Inventory **********
Inventory::Inventory(Game &game, Pos pos) : InventoryBlock(game, pos) {
	int startX = WIDTH / 3.125;
	int moveX = 64;
	int y = HEIGHT - 50;

	for (int i = 0; i < 5; i++) {
		InventoryBlock block(game, Pos {.layer = 0, .x = (startX + moveX * i), .y = y});
		inventory.push_back(block);
	}
	
	inventory[0].set_button(SDL_SCANCODE_1);
	inventory[1].set_button(SDL_SCANCODE_2);
	inventory[2].set_button(SDL_SCANCODE_3);
	inventory[3].set_button(SDL_SCANCODE_4);
	inventory[4].set_button(SDL_SCANCODE_5);
}

void Inventory::draw() {
	for (auto &blocks : inventory) {
		blocks.draw();
	}
}

void Inventory::tick() {
	int i = 0; 	
	for (auto &block : inventory) {
		if (game.keyboard.is_pressed(block.get_button())) {
     		block.use_item();
    	}
		i ++;
	}
}

void Inventory::add_item(Item item) {
	int add_index = find_open_block(item);
	InventoryBlock temp = inventory.at(add_index);
	temp.add_item(item);
	inventory.at(add_index) = temp;
}

int Inventory::find_open_block(Item item) {
	int first_empty = 0;
	bool first = false;
	int i = 0;
	if (stored_items == 0) { // Inventory has no Items
			stored_items ++;
			return 0;
		} 
	for (auto &blocks : inventory) {	
		if (blocks.get_item_name() == "" && first == false) { // Finds the first empty slot
			first_empty = i;
			first = true;
		} 	
		if (item.get_filename() == blocks.get_item_name()) // There is already the same item in the inventory
			return i;
		i ++;
	}
	stored_items ++;
	return first_empty;
}