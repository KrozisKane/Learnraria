#include "game.h"

Item::Item()
	: PhysicalObject(FloatRect(0, 0, 16, 16), 16), item_type{ 0 } {}

void Item::moveItem(GameField& game_field, float& dt)
{
	if (velocity_x > 0)
	{
		velocity_x -= acceleration_x * dt;
		if (velocity_x < 0) velocity_x = 0;
	}

	if (velocity_x < 0)
	{
		velocity_x += acceleration_x * dt;
		if (velocity_x > 0) velocity_x = 0;
	}
	physical_box.left += velocity_x * dt;
	checkCollisions(game_field, 0);		//Check collisions on the X axis
}

ItemsManager::ItemsManager() {}

void ItemsManager::createItem(const int& tile_type, int& tile_size, int& item_create_x, const int& item_create_y)
{
	Item new_item;

	//A small kludge, thanks to which mined tile of grass turns into a dirt item
	if (tile_type == 1)
	{
		new_item.item_type = tile_type + 1;
		new_item.item_texture.loadFromFile("Resources\\images\\items\\" + to_string(tile_type + 1) + ".png");
	}

	//A small kludge, thanks to which mined tile of wood turns into a wooden planks item
	else if (tile_type == 8)
	{
		new_item.item_type = tile_type - 1;
		new_item.item_texture.loadFromFile("Resources\\images\\items\\" + to_string(tile_type - 1) + ".png");
	}
	else
	{
		new_item.item_texture.loadFromFile("Resources\\images\\items\\" + to_string(tile_type) + ".png");
		new_item.item_type = tile_type;
	}

	//Because textures of different items differ in size, the physical box of each item
	//must be changed by setting new dimensions to it based on the size of its texture
	new_item.physical_box.width = new_item.item_texture.getSize().x;
	new_item.physical_box.height = new_item.item_texture.getSize().y;
	new_item.physical_box.left = item_create_x * tile_size;
	new_item.physical_box.top = item_create_y * tile_size;

	random_device random_device;
	mt19937 random_generator(random_device());
	uniform_int_distribution<int> randomize_item_speed_x(0, 1);

	if (randomize_item_speed_x(random_generator)) new_item.velocity_x = 20;
	else new_item.velocity_x = -20;
	new_item.velocity_y = -20;

	items_list.push_back(new_item);
}

void ItemsManager::updateItems(RenderWindow& game_window, float& dt, GameField& game_field, int& offset_x, int& offset_y)
{
	for (int item_id{ 0 }; item_id < items_list.size(); item_id++)
	{
		items_list[item_id].applyGravity(game_field, dt);
		items_list[item_id].moveItem(game_field, dt);
		items_list[item_id].item_sprite.setTexture(items_list[item_id].item_texture);
		items_list[item_id].item_sprite.setPosition(items_list[item_id].physical_box.left - offset_x, items_list[item_id].physical_box.top - offset_y);
		game_window.draw(items_list[item_id].item_sprite);
	}
}