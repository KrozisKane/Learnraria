#include "game.h"

Character::Character(GameField& game_field, bool new_or_load)
	: PhysicalObject(FloatRect(0, 0, 26, 44), 1000), character_game_field_x{ 0 }, character_game_field_y{ 0 }, space_state{ false },
	move_direction{ 0 }, view_direction{ 2 }, character_state{ IDLE }, character_offset_x{ 0 }, character_offset_y{ 0 }
{
	character_texture.loadFromFile("Resources\\images\\character_animations.png");
	character_animations.createAnimation("idle", character_texture, 6, 10, 26, 44, 1, 0, 0, 1);
	character_animations.createAnimation("use", character_texture, 6, 66, 28, 44, 4, 8, 56, 1);
	character_animations.createAnimation("jump", character_texture, 6, 290, 30, 49, 1, 0, 0, 1);
	character_animations.createAnimation("move", character_texture, 4, 344, 32, 46, 14, 15, 56, 1);
	character_animations.createAnimation("jump_and_use", character_texture, 6, 1130, 30, 49, 4, 8, 56, 1);
	character_animations.createAnimation("move_and_use", character_texture, 4, 1354, 32, 46, 14, 15, 56, 1);

	get_item_sound_buffer.loadFromFile("Resources\\sounds\\get_item.wav");
	get_item_sound.setBuffer(get_item_sound_buffer);

	if (!new_or_load) setCharacterSpawnPosition(game_field);
	else loadCharacterSpawnPosition(game_field);
}

void Character::updateCharacter(RenderWindow& game_window, float& dt, GameField& game_field, int& game_window_width, int& game_window_height, Interface& hotbar)
{
	keyCheck();
	setCharacterGameFieldCoordinates(game_field);
	applyGravity(game_field, dt);
	characterMove(game_field, dt);
	characterOffset(game_field, game_window_width, game_window_height);
	setAndPlayCharacterAnimation(game_window, dt);
	getItem(game_field, hotbar);
}

void Character::characterMove(GameField& game_field, float& dt)
{
	//The sign of speed/acceleration depends on which button is pressed
	if (move_direction == 2)
	{
		if (velocity_x < 300) velocity_x += acceleration_x * dt;
		else velocity_x = 300;
	}
	else if (move_direction == 1)
	{
		if (velocity_x > -300) velocity_x -= acceleration_x * dt;
		else velocity_x = -300;
	}

	//This code is for smooth stop of the character when the movement key is released
	else if (move_direction == 0)
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
	}
	physical_box.left += velocity_x * dt;
	checkCollisions(game_field, 0);		//Check collisions on X axis
}

void Character::characterJump()
{
	//If the character is standing on surface, an upward impulse is given to him (the character jumps)
	if (is_on_ground) velocity_y -= 200;
	is_on_ground = false;
}

void Character::setCharacterGameFieldCoordinates(GameField& game_field)
{
	//Shift to 2 tiles down as the sprite Y coordinate counts from the top left corner, not from the bottom left corner
	character_game_field_y = physical_box.top / game_field.getTileSize() + 2;
	character_game_field_x = physical_box.left / game_field.getTileSize();
}

void Character::setCharacterSpawnPosition(GameField& game_field)
{
	physical_box.left = (game_field.getGameFieldWidth() / 2) * game_field.getTileSize();
	for (int tile{ 0 }; tile < game_field.getGameFieldHeight(); tile++)
	{
		//Shift to 2 tiles down as the Y sprite coordinate counts from the top left corner, 
		//not from the bottom left corner (so the character does not spawn in dirt and grass tiles)
		if (game_field.getGameFieldTileType(tile, game_field.getGameFieldWidth() / 2) == 1) physical_box.top = (tile - 2) * game_field.getTileSize();
	}
}

void Character::loadCharacterSpawnPosition(GameField& game_field)
{
	ifstream read_from_file("gamedata.lrnrr");
	string data_from_file;
	vector<int> buffer;
	int buffer_counter{ 0 };

	while (data_from_file.compare("?"))
	{
		read_from_file >> data_from_file;
		if (!data_from_file.compare("!"))
		{
			while (data_from_file.compare("?"))
			{
				read_from_file >> data_from_file;
				if (data_from_file.compare("?")) buffer.push_back(stoi(data_from_file));
			}
		}
	}

	character_game_field_x = buffer[0];
	character_game_field_y = buffer[1];
	physical_box.left = character_game_field_x * game_field.getTileSize();
	physical_box.top = (character_game_field_y - 2) * game_field.getTileSize();
}

void Character::keyCheck()
{
	//Key press actions
	if (control_key["A"])
	{
		move_direction = 1;
		view_direction = 1;
		if (character_state == IDLE) character_state = MOVE;
		if (character_state == USE) character_state = MOVE_AND_USE;
	}

	if (control_key["D"])
	{
		move_direction = 2;
		view_direction = 2;
		if (character_state == IDLE) character_state = MOVE;
		if (character_state == USE) character_state = MOVE_AND_USE;
	}

	if (control_key["Space"])
	{
		if (!space_state)
		{
			characterJump();
			space_state = true;
			character_state = JUMP;
		}
		if (is_on_ground && character_state == JUMP) character_state = IDLE;
		if (is_on_ground && character_state == JUMP_AND_USE) character_state = USE;
	}

	if (control_key["Use"])
	{
		if (character_state == IDLE) character_state = USE;
		if (character_state == JUMP) character_state = JUMP_AND_USE;
		if (character_state == MOVE) character_state = MOVE_AND_USE;
	}

	//Key release actions
	if (!(control_key["A"] || control_key["D"]))
	{
		move_direction = 0;
		if (character_state == MOVE) character_state = IDLE;
		if (character_state == MOVE_AND_USE) character_state = USE;
	}

	if (!control_key["Space"])
	{
		if (is_on_ground && character_state == JUMP) character_state = IDLE;
		if (is_on_ground && character_state == JUMP_AND_USE) character_state = USE;
	}

	if (!control_key["Use"])
	{
		if (character_state == USE) character_state = IDLE;
		if (!is_on_ground && character_state == JUMP_AND_USE) character_state = JUMP;
		if (character_state == MOVE_AND_USE) character_state = MOVE;
	}

	control_key["A"] = control_key["D"] = control_key["Space"] = control_key["Use"] = false;
}

void Character::characterOffset(GameField& game_field, int& game_window_width, int& game_window_height)
{
	//Here are the conditions for the game camera offset so the character is in the center of the game window.
	//When approaching the edge of the game window, the offset relative to this edge stops.
	//Note: the game field has borders one tile wide on three sides of the game field (left, bottom, right),
	//necessary to prevent the character from moving outside of the game field. Adding/subtracting game_field.getTileSize()
	//in the conditions below prevents the camera from hitting edge tiles and showing them to the player (which also prevents him from mining them)

	//Offset in width of the game window
	//Offset condition when distance to the edge of the game field is more than half of the game window width
	if (physical_box.left > game_window_width / 2 + game_field.getTileSize()
		&& physical_box.left < (game_field.getGameFieldWidth() * game_field.getTileSize() - game_window_width / 2) - game_field.getTileSize())
		character_offset_x = physical_box.left - game_window_width / 2;

	//Offset condition when distance to the edge of the game field is less than half of the game window width (by condition for each side)
	else
	{
		if (physical_box.left < game_window_width / 2) character_offset_x = 0 + game_field.getTileSize();
		if (physical_box.left > (game_field.getGameFieldWidth() * game_field.getTileSize() - game_window_width / 2) - game_field.getTileSize())
			character_offset_x = game_field.getGameFieldWidth() * game_field.getTileSize() - game_window_width - game_field.getTileSize();
	}

	//Offset in height of the game window
	//Offset condition when distance to the edge of the game field is more than half of the game window height
	if (physical_box.top > game_window_height / 2
		&& physical_box.top < (game_field.getGameFieldHeight() * game_field.getTileSize() - game_window_height / 2) - game_field.getTileSize())
		character_offset_y = physical_box.top - game_window_height / 2;

	//Offset condition when distance to the edge of the game field is less than half of the game window height (by condition for each side)
	else
	{
		if (physical_box.top < game_window_height / 2) character_offset_y = 0;
		if (physical_box.top > (game_field.getGameFieldHeight() * game_field.getTileSize() - game_window_height / 2) - game_field.getTileSize())
			character_offset_y = game_field.getGameFieldHeight() * game_field.getTileSize() - game_window_height - game_field.getTileSize();
	}
}

void Character::setAndPlayCharacterAnimation(RenderWindow& game_window, float& dt)
{
	if (character_state == IDLE) character_animations.setAnimation("idle");
	if (character_state == MOVE) character_animations.setAnimation("move");
	if (character_state == JUMP) character_animations.setAnimation("jump");
	if (character_state == USE) character_animations.setAnimation("use");
	if (character_state == JUMP_AND_USE) character_animations.setAnimation("jump_and_use");
	if (character_state == MOVE_AND_USE) character_animations.setAnimation("move_and_use");

	//Conditions for animation mirroring depending on the direction of view.
	//They are needed to set and keep the direction of character view (left or right)
	if (view_direction == 1) character_animations.flipAnimation(1);
	else character_animations.flipAnimation(0);
	character_animations.playAnimation(dt);
	character_animations.updateAnimation(game_window, physical_box.left - character_offset_x, physical_box.top - character_offset_y);
}

void Character::getItem(GameField& game_field, Interface& hotbar)
{
	for (int item_id{ 0 }; item_id < game_field.getGameFieldItemsList().getItemsAmount(); item_id++)
	{
		if (physical_box.intersects(game_field.getGameFieldItemsList().getItem(item_id).getItemPhysicalBox()))
		{
			get_item_sound.play();
			hotbar.setTilesCounterPlus(game_field.getGameFieldItemsList().getItem(item_id).getItemType());
			game_field.getGameFieldItemsList().deleteItem(item_id);
		}
	}
}