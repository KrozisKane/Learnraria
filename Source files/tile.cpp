#include "game.h"

TileType::TileType()
	: has_collision{ 1 } {}

TileTypesManager::TileTypesManager() {}

void TileTypesManager::createTileType(int tile_type, string tile_type_name, Texture& tile_types_texture, bool has_collision)
{
	TileType new_tile_type;
	new_tile_type.tile_type_name = tile_type_name;
	new_tile_type.tile_type_sprite.setTexture(tile_types_texture);
	new_tile_type.tile_type_sprite.setTextureRect(IntRect((tile_type -1)*16, 0, 16, 16));
	new_tile_type.has_collision = has_collision;

	tile_types_list[tile_type] = new_tile_type;
}

Tree::Tree()
	: tree_x{ 0 }, tree_y{ 0 }, tree_height{ 0 }, branches_amount{ 0 }
{
	tree_top_texture.loadFromFile("Resources\\images\\tree_top.png");
	tree_branches_texture.loadFromFile("Resources\\images\\tree_branches.png");
}

TreesManager::TreesManager() {}

void TreesManager::checkTreesCollisions(GameField& game_field, bool& is_check_ok, Tree& new_tree, int& tree_x, int& grass_layer_y)
{
	//Trees overlap check
	for (int j{ tree_x - 5 }; j < tree_x + 5; j++)
	{
		//The second condition is needed to take into account different values of tree_y for different trees
		if (game_field.getGameFieldTileType(grass_layer_y - 1, j) == 8 || game_field.getGameFieldTileType(grass_layer_y - 3, j) == 8)
		{
			if (tree_x < 120)
			{
				tree_x = tree_x++;

				//When changing the value of tree_x, the value of grass_layer_y also must be checked. If the tile at
				//[grass_layer_y; tree_x] is not a grass tile, the grass_layer_y value must be redefined
				if (game_field.getGameFieldTileType(grass_layer_y, tree_x) != 1)
					for (grass_layer_y = game_field.getGameFieldHeight() - 1; grass_layer_y > 0; grass_layer_y--)
					{
						if (game_field.getGameFieldTileType(grass_layer_y, tree_x) == 1) break;
					}
				return;
			}

			//If a tree shift has led to the overlap of two trees at the right end of the game field, tree_x
			//of this tree is changed to the minimum possible. Then the algorithm will be able to find
			//a place for this tree in any other free part of the game field
			else
			{
				tree_x = 5;
				if (game_field.getGameFieldTileType(grass_layer_y, tree_x) != 1)
					for (grass_layer_y = game_field.getGameFieldHeight() - 1; grass_layer_y > 0; grass_layer_y--)
					{
						if (game_field.getGameFieldTileType(grass_layer_y, tree_x) == 1) break;
					}
				return;
			}
		}
	}
	is_check_ok = true;
}

void TreesManager::createTree(GameField& game_field, int& tree, int& tree_x, int& tree_height)
{
	bool is_check_ok{ 0 };
	Tree new_tree;
	new_tree.tree_height = tree_height;

	new_tree.branches_amount = tree_height / 4;		//This expression sets the number of branches of a tree depending on its height
	new_tree.tree_branches_sprite.resize(new_tree.branches_amount);
	new_tree.tree_branches_sprite_position.resize(new_tree.branches_amount);

	for (int grass_layer_y{ game_field.getGameFieldHeight() - 1 }; grass_layer_y > 0; grass_layer_y--)
	{
		if (game_field.getGameFieldTileType(grass_layer_y, tree_x) == 1)
		{
			is_check_ok = false;
			while (!is_check_ok)
			{
				checkTreesCollisions(game_field, is_check_ok, new_tree, tree_x, grass_layer_y);
			}
			new_tree.tree_x = tree_x;
			new_tree.tree_y = grass_layer_y - 1;

			for (int tree_tile{ 1 }; tree_tile <= tree_height; tree_tile++)
			{
				game_field.setGameFieldTileType(grass_layer_y - tree_tile, tree_x) = 8;
				if (tree_tile == tree_height)
				{
					new_tree.tree_top_sprite.setPosition(tree_x * game_field.getTileSize() - 32, (grass_layer_y - tree_height) * game_field.getTileSize() - 80);
					new_tree.tree_top_sprite_position = new_tree.tree_top_sprite.getPosition();
					for (int branches{ 0 }; branches < new_tree.branches_amount; branches++)
					{
						new_tree.tree_branches_sprite[branches].setPosition(tree_x * game_field.getTileSize() - 30, (grass_layer_y - (tree_height / new_tree.branches_amount) * branches - 2) * game_field.getTileSize() - 28);
						new_tree.tree_branches_sprite_position[branches] = new_tree.tree_branches_sprite[branches].getPosition();
					}
				}
			}
		}
	}
	trees_list[tree] = new_tree;
}

void TreesManager::updateTrees(RenderWindow& game_window, int& offset_x, int& offset_y)
{
	for (int tree_id{ 0 }; tree_id < trees_list.size(); tree_id++)
	{
		trees_list[tree_id].tree_top_sprite.setTexture(trees_list[tree_id].tree_top_texture);
		trees_list[tree_id].tree_top_sprite.setPosition(trees_list[tree_id].tree_top_sprite_position.x - offset_x, trees_list[tree_id].tree_top_sprite_position.y - offset_y);
		game_window.draw(trees_list[tree_id].tree_top_sprite);
		for (int branches{ 0 }; branches < trees_list[tree_id].branches_amount; branches++)
		{
			trees_list[tree_id].tree_branches_sprite[branches].setPosition(trees_list[tree_id].tree_branches_sprite_position[branches].x - offset_x, trees_list[tree_id].tree_branches_sprite_position[branches].y - offset_y);
			trees_list[tree_id].tree_branches_sprite[branches].setTexture(trees_list[tree_id].tree_branches_texture);
			game_window.draw(trees_list[tree_id].tree_branches_sprite[branches]);
		}
	}
}

void TreesManager::chopTree(GameField& game_field, int& mouse_x, int& mouse_y)
{
	//Loop that find the tree ID that was clicked with LMB
	for (int tree_id{ 0 }; tree_id < trees_list.size(); tree_id++)
	{
		//If mouse_x matches one of the trees's tree_x, the tree we need is found
		if (mouse_x == trees_list[tree_id].tree_x)
		{
			//Loop that chops all tiles of the tree from the tile where LMB was clicked to the top of the tree
			for (int tree_tile{ trees_list[tree_id].tree_y - mouse_y }; tree_tile < trees_list[tree_id].tree_height; tree_tile++)
			{
				//The tree_tile value is the coordinate of the tree tile to be destroyed in the tree trunk coordinate system.
				//But to create an item and replace the type of tile in the game field array, its need the coordinates of this tile in the game field coordinate system.
				//For this used the expression "tree_tile - (tree_list[tree_id].tree_y - mouse_y)".
				//Due to the collisions feature, 1 is added to the item creation coordinate on the Y axis, so that the item spawn a little higher
				game_field.getGameFieldItemsList().createItem(game_field.getGameFieldTileType(mouse_y - (tree_tile - (trees_list[tree_id].tree_y - mouse_y)), mouse_x), game_field.getTileSize(), mouse_x, mouse_y - 1 - (tree_tile - (trees_list[tree_id].tree_y - mouse_y)));
				game_field.setGameFieldTileType(mouse_y - (tree_tile - (trees_list[tree_id].tree_y - mouse_y)), mouse_x) = 0;

				trees_list[tree_id].tree_top_sprite.setColor(Color::Transparent);
				for (int branches{ 0 }; branches < trees_list[tree_id].branches_amount; branches++)
				{
					if (trees_list[tree_id].tree_branches_sprite_position[branches].y < mouse_y * game_field.getTileSize())
						trees_list[tree_id].tree_branches_sprite[branches].setColor(Color::Transparent);
				}
			}

			//Updating height of the tree after chopping its tiles
			trees_list[tree_id].tree_height = trees_list[tree_id].tree_y - mouse_y;
		}
	}
}

void TreesManager::saveTrees(ofstream& write_to_file)
{
	write_to_file << trees_list.size() << " ";
	for (int tree_id{ 0 }; tree_id < trees_list.size(); tree_id++)
	{
		write_to_file << trees_list[tree_id].tree_x << " ";
		write_to_file << trees_list[tree_id].tree_y << " ";
		write_to_file << trees_list[tree_id].tree_height << " ";
		write_to_file << trees_list[tree_id].tree_top_sprite_position.x << " ";
		write_to_file << trees_list[tree_id].tree_top_sprite_position.y << " ";
		if ((trees_list[tree_id].tree_top_sprite.getColor()) != Color::Transparent) write_to_file << 1 << " ";
		else write_to_file << 0 << " ";
		write_to_file << trees_list[tree_id].branches_amount << " ";
		for (int branches_id{ 0 }; branches_id < trees_list[tree_id].tree_branches_sprite_position.size(); branches_id++)
		{
			if ((trees_list[tree_id].tree_branches_sprite[branches_id].getColor()) != Color::Transparent) write_to_file << 1 << " ";
			else write_to_file << 0 << " ";
			write_to_file << trees_list[tree_id].tree_branches_sprite_position[branches_id].x << " ";
			write_to_file << trees_list[tree_id].tree_branches_sprite_position[branches_id].y << " ";
		}
	}
}

void TreesManager::loadTrees(ifstream& read_from_file, vector<int>& buffer, int& buffer_counter)
{
	int trees_amount{ buffer[buffer_counter++] };

	for (int tree_id{ 0 }; tree_id < trees_amount; tree_id++)
	{
		Tree new_tree;
		trees_list[tree_id] = new_tree;
	}

	for (int tree_id{ 0 }; tree_id < trees_amount; tree_id++)
	{
		trees_list[tree_id].tree_x = buffer[buffer_counter++];
		trees_list[tree_id].tree_y = buffer[buffer_counter++];
		trees_list[tree_id].tree_height = buffer[buffer_counter++];
		trees_list[tree_id].tree_top_sprite_position.x = buffer[buffer_counter++];
		trees_list[tree_id].tree_top_sprite_position.y = buffer[buffer_counter++];
		if (buffer[buffer_counter++] == 0) trees_list[tree_id].tree_top_sprite.setColor(Color::Transparent);
		trees_list[tree_id].branches_amount = buffer[buffer_counter++];
		trees_list[tree_id].tree_branches_sprite.resize(trees_list[tree_id].branches_amount);
		trees_list[tree_id].tree_branches_sprite_position.resize(trees_list[tree_id].branches_amount);
		for (int branches_id{ 0 }; branches_id < trees_list[tree_id].branches_amount; branches_id++)
		{
			if (buffer[buffer_counter++] == 0) trees_list[tree_id].tree_branches_sprite[branches_id].setColor(Color::Transparent);
			trees_list[tree_id].tree_branches_sprite_position[branches_id].x = buffer[buffer_counter++];
			trees_list[tree_id].tree_branches_sprite_position[branches_id].y = buffer[buffer_counter++];
		}
	}
}