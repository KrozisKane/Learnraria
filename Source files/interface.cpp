#include "game.h"

Interface::Interface()
{
	inventory_coord_x = 40;
	inventory_coord_y = 40;
	selected_block = 0;
	texture_inventory.loadFromFile("D:\\Programming\\Learnraria\\Resources\\images\\inventory.png");
	texture_selected_block.loadFromFile("D:\\Programming\\Learnraria\\Resources\\images\\selected_block.png");
	sprite_selected_block.setTexture(texture_selected_block);
	sprite_inventory.setTexture(texture_inventory);
	sprite_selected_block.setPosition(inventory_coord_x + 16, inventory_coord_y + 16);
	sprite_inventory.setPosition(inventory_coord_x, inventory_coord_y);
	font_block_counter.loadFromFile("D:\\Programming\\Learnraria\\Resources\\fonts\\comic_sans.ttf");
	text_blocks_counter[0].setFont(font_block_counter);
	text_blocks_counter[0].setCharacterSize(20);
	text_blocks_counter[0].setStyle(Text::Bold);
	text_blocks_counter[0].setFillColor(Color::Black);
	text_blocks_counter[0].setString("0");
	text_blocks_counter[0].setPosition(inventory_coord_x + 60, inventory_coord_y + 55);
	text_blocks_counter[1] = text_blocks_counter[0];
	text_blocks_counter[1].setPosition(inventory_coord_x + 145, inventory_coord_y + 55);
}

void Interface::drawBlocksSelection(RenderWindow& game_window, GameField& game_field)
{
	game_field.getBlockSprite(1).setPosition(inventory_coord_x + 30, inventory_coord_y + 30);
	game_field.getBlockSprite(2).setPosition(inventory_coord_x + 114, inventory_coord_y + 30);
	game_window.draw(sprite_inventory);
	game_window.draw(game_field.getBlockSprite(1));
	game_window.draw(game_field.getBlockSprite(2));
	game_window.draw(sprite_selected_block);
	text_blocks_counter[0].setString(to_string(blocks_counter[0]));
	text_blocks_counter[1].setString(to_string(blocks_counter[1]));
	game_window.draw(text_blocks_counter[0]);
	game_window.draw(text_blocks_counter[1]);
}

void Interface::setBlocksCounterPlus(int block_id)
{
	if (block_id == 1) blocks_counter[0]++;
	if (block_id == 2) blocks_counter[1]++;
}

void Interface::setSelectedBlock(char delta)
{
	if (delta == 1) 
	{ 
		if (selected_block == 0)
		{
			selected_block++;
			sprite_selected_block.setPosition(inventory_coord_x + 100, inventory_coord_y + 16);
		}
		else
		{
			selected_block = 0;
			sprite_selected_block.setPosition(inventory_coord_x + 16, inventory_coord_y + 16);
		}
	
	}
	if (delta == -1) 
	{ 
		if (selected_block == 1)
		{
			selected_block--;
			sprite_selected_block.setPosition(inventory_coord_x + 16, inventory_coord_y + 16);
		}
		else
		{
			selected_block = 1;
			sprite_selected_block.setPosition(inventory_coord_x + 100, inventory_coord_y + 16);
		}
	}
}