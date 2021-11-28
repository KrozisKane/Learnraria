#include "game.h"

Interface::Interface(GameField& game_field, bool new_or_load)
	: hotbar_x{ 32 }, hotbar_y{ 32 }, selected_tile_type{ 0 }, tile_types{ 0 }
{
	tile_types = game_field.getGameFieldTilesList().getTileTypesAmount() - 3;	 //Subtracting air, grass and wood tile types
	tiles_counter.resize(tile_types);
	tiles_counter_text.resize(tile_types);
	hotbar_texture.loadFromFile("Resources\\images\\hotbar.png");
	selected_tile_type_texture.loadFromFile("Resources\\images\\selected_tile.png");
	selected_tile_type_sprite.setTexture(selected_tile_type_texture);
	hotbar_sprite.setTexture(hotbar_texture);
	selected_tile_type_sprite.setPosition(hotbar_x, hotbar_y);
	hotbar_sprite.setPosition(hotbar_x, hotbar_y);
	tiles_counter_font.loadFromFile("Resources\\fonts\\comic_sans.ttf");

	for (int tile_type{ 0 }; tile_type < tile_types; tile_type++)
	{
		tiles_counter_text[tile_type].setFont(tiles_counter_font);
		tiles_counter_text[tile_type].setCharacterSize(16);
		tiles_counter_text[tile_type].setStyle(Text::Bold);
		tiles_counter_text[tile_type].setOutlineThickness(1);
		tiles_counter_text[tile_type].setOutlineColor(Color::Black);
		tiles_counter_text[tile_type].setString("0");
		tiles_counter_text[tile_type].setPosition(hotbar_x + 10 + 58 * tile_type, hotbar_y + 25);
	}

	if (new_or_load) loadHotbarTiles();
}

void Interface::loadHotbarTiles()
{
	ifstream read_from_file("gamedata.lrnrr");
	string data_from_file;
	vector<int> buffer;
	int buffer_counter{ 0 };

	while (read_from_file)
	{
		read_from_file >> data_from_file;
		if (!data_from_file.compare("?"))
		{
			while (read_from_file)
			{
				read_from_file >> data_from_file;
				buffer.push_back(stoi(data_from_file));
			}
		}
	}

	for (int i{ 0 }; i < tiles_counter.size(); i++)
	{
		tiles_counter[i] = buffer[buffer_counter++];
	}
}

void Interface::saveHotbarTiles(ofstream& write_to_file)
{
	write_to_file << "?" << " ";
	for (int tile_type{ 0 }; tile_type < tiles_counter.size(); tile_type++)
	{
		write_to_file << tiles_counter[tile_type] << " ";
	}
}

void Interface::updateHotbar(RenderWindow& game_window, GameField& game_field)
{
	game_window.draw(hotbar_sprite);
	game_window.draw(selected_tile_type_sprite);

	for (int tile_type{ 0 }; tile_type < tile_types; tile_type++)
	{
		//Adding "+ 2" to tile_type is necessary to cut off air and grass tile types
		tile_type_texture.loadFromFile("Resources\\images\\items\\" + to_string(tile_type + 2) + ".png");
		tile_type_sprite.setTexture(tile_type_texture);
		tile_type_sprite.setTextureRect(IntRect(0, 0, tile_type_texture.getSize().x, tile_type_texture.getSize().y));
		tile_type_sprite.setPosition(hotbar_x + 26 - tile_type_texture.getSize().x / 2 + 58 * tile_type, hotbar_y + 26 - tile_type_texture.getSize().y / 2);
		game_window.draw(tile_type_sprite);

		tiles_counter_text[tile_type].setString(to_string(tiles_counter[tile_type]));
		game_window.draw(tiles_counter_text[tile_type]);
	}
}

void Interface::setSelectedTileType(float& delta)
{
	if (delta == 1)
	{
		//Subtracting 1 from tile_types is necessary because tile_types is 6 and selected_tile_type starts counting from 0
		if (selected_tile_type == tile_types - 1)
		{
			selected_tile_type = 0;
			selected_tile_type_sprite.setPosition(hotbar_x + 58 * selected_tile_type, hotbar_y);
		}
		else
		{
			selected_tile_type++;
			selected_tile_type_sprite.setPosition(hotbar_x + 58 * selected_tile_type, hotbar_y);
		}
	}
	if (delta == -1)
	{
		if (selected_tile_type == 0)
		{
			selected_tile_type = tile_types - 1;
			selected_tile_type_sprite.setPosition(hotbar_x + 58 * selected_tile_type, hotbar_y);
		}
		else
		{
			selected_tile_type--;
			selected_tile_type_sprite.setPosition(hotbar_x + 58 * selected_tile_type, hotbar_y);
		}
	}
}