#include "game.h"

GameField::GameField(bool new_or_load)
	: game_field_width{ 0 }, game_field_height{ 0 }, 
	tile_size{ 16 }, mouse_x{ 0 }, mouse_y{ 0 }
{
	background_texture.loadFromFile("Resources\\images\\background.png");
	tile_types_texture.loadFromFile("Resources\\images\\tile_types.png");
	back_tile_types_texture.loadFromFile("Resources\\images\\back_tile_types.png");
	background_sprite.setTexture(background_texture);
	game_field_tiles.createTileType(0, "air", tile_types_texture, 0);
	game_field_tiles.createTileType(1, "grass", tile_types_texture, 1);
	game_field_tiles.createTileType(2, "dirt", tile_types_texture, 1);
	game_field_tiles.createTileType(3, "stone", tile_types_texture, 1);
	game_field_tiles.createTileType(4, "iron", tile_types_texture, 1);
	game_field_tiles.createTileType(5, "copper", tile_types_texture, 1);
	game_field_tiles.createTileType(6, "gold", tile_types_texture, 1);
	game_field_tiles.createTileType(7, "wooden_planks", tile_types_texture, 1);
	game_field_tiles.createTileType(8, "wood", tile_types_texture, 0);
	back_game_field_tiles.createTileType(0, "air", back_tile_types_texture, 0);
	back_game_field_tiles.createTileType(1, "dirt_wall", back_tile_types_texture, 0);

	place_tile_sound_buffer_1.loadFromFile("Resources\\sounds\\mine_dirt_1.wav");
	place_tile_sound_buffer_2.loadFromFile("Resources\\sounds\\mine_dirt_2.wav");
	place_tile_sound_buffer_3.loadFromFile("Resources\\sounds\\mine_dirt_3.wav");
	mine_dirt_tile_sound_buffer_1 = place_tile_sound_buffer_1;
	mine_dirt_tile_sound_buffer_2 = place_tile_sound_buffer_2;
	mine_dirt_tile_sound_buffer_3 = place_tile_sound_buffer_3;
	mine_stone_tile_sound_buffer_1.loadFromFile("Resources\\sounds\\mine_stone_1.wav");
	mine_stone_tile_sound_buffer_2.loadFromFile("Resources\\sounds\\mine_stone_2.wav");
	mine_stone_tile_sound_buffer_3.loadFromFile("Resources\\sounds\\mine_stone_3.wav");


	if (!new_or_load) generateGameField();
	else loadGameField();
}

void GameField::generateDirtBase(mt19937& random_generator)
{
	//First, a number is randomly generated that is responsible for height of the column of tiles (dirt_layer_height). 
	//Then a number is randomly generated that is responsible for width of the game field area with such column
	//height (dirt_layer_width). The value of dirt_layer_width decreases with each generated column of tiles.
	//Once dirt_layer_width value drops to 0, both dirt_layer_width and dirt_layer_height values will be redefined 
	//with new random numbers

	uniform_int_distribution<int> randomize_dirt_layer_height(15, 17);
	uniform_int_distribution<int> randomize_dirt_layer_width(3, 20);
	int dirt_layer_height{ randomize_dirt_layer_height(random_generator) };
	int dirt_layer_width{ randomize_dirt_layer_width(random_generator) };

	for (int i{ 0 }; i < game_field_width; i++)
	{
		dirt_layer_width--;
		if (dirt_layer_width == 0)
		{
			dirt_layer_height = randomize_dirt_layer_height(random_generator);
			dirt_layer_width = randomize_dirt_layer_width(random_generator);
		}
		for (int j{ game_field_height - 1 }; j > game_field_height - 1 - dirt_layer_height; j--)
		{
			game_field_array[j][i] = 2;
		}
	}

	//Loop for generating borders on left and right game field sides 
	for (int i{ 0 }; i < game_field_height; i++)
	{
		game_field_array[i][0] = 2;
		game_field_array[i][game_field_width - 1] = 2;
	}
}

void GameField::generateWalls()
{
	for (int i{ 0 }; i < game_field_height; i++)
	{
		for (int j{ 0 }; j < game_field_width; j++)
		{
			if (game_field_array[i][j] == 2) back_game_field_array[i][j] = 1;
		}
	}
}

void GameField::generateGrass()
{
	for (int i{ 0 }; i < game_field_width; i++)
	{
		for (int j{ game_field_height - 1 }; j > 0; j--)
		{
			if (game_field_array[j][i] == 2 && game_field_array[j - 1][i] == 0) game_field_array[j - 1][i] = 1;
		}
	}
}

void GameField::checkOreClustersCollisions(int& ore_spawn_y, int& ore_spawn_x, int& ore_size_y, int& ore_size_x, bool& is_check_ok)
{
	//Ores overlap check
	for (int i{ ore_spawn_y - 2 }; i < ore_spawn_y + ore_size_y + 2; i++)
	{
		for (int j{ ore_spawn_x - 2}; j < ore_spawn_x + ore_size_x + 2; j++)
		{
			if (game_field_array[i][j] != 2)
			{
				//Here "+ 1" allows to check if the expression "ore_spawn_x + ore_size_x"
				//will go beyond width of the game field when increasing ore_spawn_x by 1
				if (ore_spawn_x + ore_size_x + 1 < 124)
				{
					ore_spawn_x++;	
					return;
				}

				//If an ore shift has led to the overlap of two ores at the right end of the game field, ore_spawn_x
				//of this ore is changed to the minimum possible. Then the algorithm will be able to find a place
				//for this ore in any other free part of the game field
				else
				{
					ore_spawn_x = 3;
					return;
				}
			}
		}
	}
	is_check_ok = true;
}

void GameField::generateOres(mt19937& random_generator)
{
	uniform_int_distribution<int> randomize_ore_amount(1, 6);

	//The maximum value of randomize_ore_spawn_y is set as "game_field_height - max randomize_ore_size_y - 2"
	//so that there are at least 2 dirt tiles between ore clusters on the Y axis
	uniform_int_distribution<int> randomize_ore_spawn_y(36, 43);

	//The maximum value of randomize_ore_spawn_x is set as "game_field_width - max randomize_ore_size_x - 2"
	//so that there are at least 2 dirt tiles between ore clusters on the X axis
	uniform_int_distribution<int> randomize_ore_spawn_x(3, 118);
	uniform_int_distribution<int> randomize_ore_size_y(2, 4);
	uniform_int_distribution<int> randomize_ore_size_x(2, 4);
	int ore_amount{ 0 };
	int ore_spawn_y{ 0 };
	int ore_spawn_x{ 0 };
	int ore_size_y{ 0 };
	int ore_size_x{ 0 };
	bool is_check_ok{ false };

	//Ores generation loop. Here sets types of ores to generate
	for (int ore_type{ 3 }; ore_type <= 6; ore_type++)
	{
		ore_amount = randomize_ore_amount(random_generator);

		//Ore clusters generation loop
		for (int ore{ 0 }; ore < ore_amount; ore++)
		{
			ore_spawn_y = randomize_ore_spawn_y(random_generator);
			ore_spawn_x = randomize_ore_spawn_x(random_generator);
			ore_size_y = randomize_ore_size_y(random_generator);
			ore_size_x = randomize_ore_size_x(random_generator);

			is_check_ok = false;
			while (!is_check_ok)			
			{
				checkOreClustersCollisions(ore_spawn_y, ore_spawn_x, ore_size_y, ore_size_x, is_check_ok);
			}

			//One cluster generation loops
			for (int i{ ore_spawn_y }; i < ore_spawn_y + ore_size_y; i++)
			{
				for (int j{ ore_spawn_x }; j < ore_spawn_x + ore_size_x; j++)
				{
					game_field_array[i][j] = ore_type;
				}
			}
		}
	}
}

void GameField::generateTrees(mt19937& random_generator)
{
	uniform_int_distribution<int> randomize_tree_x(5, 120);
	uniform_int_distribution<int> randomize_tree_height(4, 15);
	uniform_int_distribution<int> randomize_trees_amount(5, 10);
	int tree_x{ 0 };
	int tree_height{ 0 };
	int trees_amount{ randomize_trees_amount(random_generator) };

	for (int tree{ 0 }; tree < trees_amount; tree++)
	{
		tree_x = randomize_tree_x(random_generator);
		tree_height = randomize_tree_height(random_generator);
		game_field_trees.createTree(*this, tree, tree_x, tree_height);
	}
}

void GameField::generateGameField()
{
	//Setting the size of the game field array
	game_field_width = 125; 
	game_field_height = 50;

	vector<vector<int>> set_game_field_size(game_field_height, vector<int>(game_field_width, 0));
	game_field_array = set_game_field_size;
	back_game_field_array = set_game_field_size;

	mt19937 random_generator(random_device());

	//Below is a random generation algorithm for "forest" biome:
	//Stage one. Generation of the base - dirt tile mass
	generateDirtBase(random_generator);

	//Stage two. Walls generation
	generateWalls();

	//Stage three. Covering dirt tiles with a layer of grass tiles one tile thick
	generateGrass();

	//Stage four. Trees generation
	generateTrees(random_generator);

	//Stage five. Stone and ore deposits generation
	generateOres(random_generator);
}

void GameField::loadGameField()
{
	ifstream read_from_file("gamedata.lrnrr");
	string data_from_file;
	vector<int> buffer;
	int buffer_counter{ 0 };

	while (data_from_file.compare("!"))
	{
		read_from_file >> data_from_file;
		if (data_from_file.compare("!")) buffer.push_back(stoi(data_from_file));
	}

	game_field_height = buffer[buffer_counter++];
	game_field_width = buffer[buffer_counter++];

	vector<vector<int>> set_game_field_size(game_field_height, vector<int>(game_field_width, 0));
	game_field_array = set_game_field_size;
	back_game_field_array = set_game_field_size;

	for (int i{ 0 }; i < game_field_height; i++)
	{
		for (int j{ 0 }; j < game_field_width; j++)
		{
			game_field_array[i][j] = buffer[buffer_counter++];
		}
	}

	for (int i{ 0 }; i < game_field_height; i++)
	{
		for (int j{ 0 }; j < game_field_width; j++)
		{
			back_game_field_array[i][j] = buffer[buffer_counter++];
		}
	}

	game_field_trees.loadTrees(read_from_file, buffer, buffer_counter);
}

void GameField::updateGameField(RenderWindow& game_window, Character& player, float& dt, int& game_window_width, int& game_window_height)
{
	for (int i{ 0 }; i < game_field_height; i++)
	{
		for (int j{ 0 }; j < game_field_width; j++)
		{
			//tile_type starts at 1 because 0 - air tile and it is not rendering
			for (int tile_type{ 1 }; tile_type < back_game_field_tiles.getTileTypesAmount(); tile_type++)
			{
				//Conformity check of the value in the game field array and the tile type in the list of tile types
				if (back_game_field_array[i][j] == tile_type)
				{
					//The sprite of the type of tile that matches the condition is set at specific coordinates and displayed on the screen
					back_game_field_tiles.getTileType(tile_type).getTileTypeSprite().setPosition(j * tile_size - player.getCharacterOffsetX(), i * tile_size - player.getCharacterOffsetY());
					game_window.draw(back_game_field_tiles.getTileType(tile_type).getTileTypeSprite());
				}
			}
			for (int tile_type{ 1 }; tile_type < game_field_tiles.getTileTypesAmount(); tile_type++)
			{
				if (game_field_array[i][j] == tile_type)
				{
					game_field_tiles.getTileType(tile_type).getTileTypeSprite().setPosition(j * tile_size - player.getCharacterOffsetX(), i * tile_size - player.getCharacterOffsetY());
					game_window.draw(game_field_tiles.getTileType(tile_type).getTileTypeSprite());																					
				}
			}
		}
	}
	game_field_items.updateItems(game_window, dt, *this, player.getCharacterOffsetX(), player.getCharacterOffsetY());
	game_field_trees.updateTrees(game_window, player.getCharacterOffsetX(), player.getCharacterOffsetY());
}

void GameField::setTileActionSound(string action_type)
{
	mt19937 random_generator(random_device());
	uniform_int_distribution<int> randomize_tile_action_sound(0, 2);
	int tile_action_sound{ 0 };
	tile_action_sound = randomize_tile_action_sound(random_generator);

	if (action_type == "mine")
	{
		if (game_field_array[mouse_y][mouse_x] == 1 || game_field_array[mouse_y][mouse_x] == 2
			|| game_field_array[mouse_y][mouse_x] == 7 || game_field_array[mouse_y][mouse_x] == 8)
		{
			if (tile_action_sound == 0) mine_tile_sound.setBuffer(mine_dirt_tile_sound_buffer_1);
			else if (tile_action_sound == 1) mine_tile_sound.setBuffer(mine_dirt_tile_sound_buffer_2);
			else mine_tile_sound.setBuffer(mine_dirt_tile_sound_buffer_3);
		}
		else if (game_field_array[mouse_y][mouse_x] == 3 || game_field_array[mouse_y][mouse_x] == 4
				 || game_field_array[mouse_y][mouse_x] == 5 || game_field_array[mouse_y][mouse_x] == 6)
			 {
				 if (tile_action_sound == 0) mine_tile_sound.setBuffer(mine_stone_tile_sound_buffer_1);
				 else if (tile_action_sound == 1) mine_tile_sound.setBuffer(mine_stone_tile_sound_buffer_2);
				 else mine_tile_sound.setBuffer(mine_stone_tile_sound_buffer_3);
			 }
	}
	else if(action_type == "place")
	{
		if (tile_action_sound == 0) place_tile_sound.setBuffer(place_tile_sound_buffer_1);
		else if (tile_action_sound == 1) place_tile_sound.setBuffer(place_tile_sound_buffer_2);
		else place_tile_sound.setBuffer(place_tile_sound_buffer_3);
	}
}

void GameField::mineTile(RenderWindow& game_window, Character& player, Interface& hotbar, int& game_window_width, int& game_window_height)
{
	if (Mouse::getPosition(game_window).x < game_window_width && Mouse::getPosition(game_window).x > 0 && Mouse::getPosition(game_window).y < game_window_height && Mouse::getPosition(game_window).y > 0)
	{
		mouse_x = (Mouse::getPosition(game_window).x + player.getCharacterOffsetX()) / tile_size;
		mouse_y = (Mouse::getPosition(game_window).y + player.getCharacterOffsetY()) / tile_size;

		setTileActionSound("mine");

		//Conditions limiting tiles mining range
		if (mouse_x < player.getCharacterGameFieldX() + 4
			&& mouse_x > player.getCharacterGameFieldX() - 4
			&& mouse_y < player.getCharacterGameFieldY() + 4
			&& mouse_y > player.getCharacterGameFieldY() - 4)
		{
			//If selected tile type is not an air tile or a grass tile, above which there is a wood tile
			if (game_field_array[mouse_y][mouse_x] != 0 && !(game_field_array[mouse_y][mouse_x] == 1 && game_field_array[mouse_y - 1][mouse_x] == 8))
			{
				mine_tile_sound.play();

				//If selected tile type is a tree tile, then method for chopping is called
				if (game_field_array[mouse_y][mouse_x] == 8) game_field_trees.chopTree(*this, mouse_x, mouse_y);

				//Otherwise, an item of conformiting type is created at the place of selected tile, and this tile's type is assigned "air" tile type
				else
				{
					game_field_items.createItem(game_field_array[mouse_y][mouse_x], tile_size, mouse_x, mouse_y);
					game_field_array[mouse_y][mouse_x] = 0;
				}
			}

		}
	}
}

void GameField::placeTile(RenderWindow& game_window, Character& player, Interface& hotbar, int& game_window_width, int& game_window_height)
{
	if (Mouse::getPosition(game_window).x < game_window_width && Mouse::getPosition(game_window).x > 0 && Mouse::getPosition(game_window).y < game_window_height && Mouse::getPosition(game_window).y > 0)
	{
		mouse_x = (Mouse::getPosition(game_window).x + player.getCharacterOffsetX()) / tile_size;
		mouse_y = (Mouse::getPosition(game_window).y + player.getCharacterOffsetY()) / tile_size;

		setTileActionSound("place");

		//Conditions that limiting tile place range and also checks for ability to place tiles and availability of selected tile type in hotbar
		if (mouse_x < player.getCharacterGameFieldX() + 4
			&& mouse_x > player.getCharacterGameFieldX() - 4
			&& mouse_y < player.getCharacterGameFieldY() + 4
			&& mouse_y > player.getCharacterGameFieldY() - 4
			&& game_field_array[mouse_y][mouse_x] == 0
			&& !(player.getCharacterPhysicalBox().intersects(FloatRect(mouse_x * 16, mouse_y * 16, 16, 16)))
			&& hotbar.getTilesAmount(hotbar.getSelectedTileType()) != 0)
		{
			place_tile_sound.play();
			hotbar.setTilesCounterMinus(hotbar.getSelectedTileType());

			//Adding tile to the game field array, taking into account its type. Adding "+ 2" to tile_type is necessary to take into account cutted off air and grass tile types
			game_field_array[mouse_y][mouse_x] = hotbar.getSelectedTileType() + 2;
		}
	}
}