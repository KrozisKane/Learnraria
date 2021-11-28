#include "game.h"

PhysicalObject::PhysicalObject(FloatRect physical_box_size, float acceleration_x)
	: velocity_x{ 0 }, velocity_y{ 0 }, acceleration_x{ acceleration_x }, acceleration_y{ 400 }, is_on_ground{ false },
	physical_box{ physical_box_size } {}

void PhysicalObject::checkCollisions(GameField& game_field, bool axis)
{
	//Algorithm for checking collisions of a physical object
	for (int i = physical_box.top / game_field.getTileSize(); i < (physical_box.top + physical_box.height) / game_field.getTileSize(); i++)
		for (int j = physical_box.left / game_field.getTileSize(); j < (physical_box.left + physical_box.width) / game_field.getTileSize(); j++)
		{
			//If this tile type of the game field has a collision
			if (game_field.getGameFieldTilesList().getTileType(game_field.getGameFieldTileType(i, j)).getTileTypeCollision() != 0)
			{
				//Checking conditions for axis X
				//The object will hit left side of the tile (because it moves to the right)
				if (velocity_x > 0 && axis == 0) { physical_box.left = j * game_field.getTileSize() - physical_box.width; velocity_x = 0; }

				//The object will hit right side of the tile (because it moves to the left)
				if (velocity_x < 0 && axis == 0) { physical_box.left = j * game_field.getTileSize() + game_field.getTileSize(); velocity_x = 0; }

				//Checking conditions for axis Y
				//The object will hit top side of the tile (because it moves down)
				if (velocity_y > 0 && axis == 1) { physical_box.top = i * game_field.getTileSize() - physical_box.height; velocity_y = 0; is_on_ground = true; }

				//The object will hit bottom side of the tile (because it moves up)
				if (velocity_y < 0 && axis == 1) { physical_box.top = i * game_field.getTileSize() + game_field.getTileSize(); velocity_y = 0; }
			}
		}
}

void PhysicalObject::applyGravity(GameField& game_field, float& dt)
{
	physical_box.top += velocity_y * dt;
	velocity_y += acceleration_y * dt;
	checkCollisions(game_field, 1);		//Check collisions on the Y axis
}