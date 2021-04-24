#include "game.h"

GameField::GameField(string& map_size)
{
	gamefield_generated = false;
	g_pressed_counter = 0;
	game_field_dirt_height = 6;
	game_field_grass_height = 1;
	mouse_coord_x = 0;
	mouse_coord_y = 0;
	texture_background.loadFromFile("D:\\Programming\\Learnraria\\Resources\\images\\background.jpg");
	texture_blocks.loadFromFile("D:\\Programming\\Learnraria\\Resources\\images\\blocks.png");
	sprite_background.setTexture(texture_background);
	sprite_block_dirt.setTexture(texture_blocks);
	sprite_block_dirt.setTextureRect(IntRect(0, 0, 40, 40));
	sprite_block_grass.setTexture(texture_blocks);
	sprite_block_grass.setTextureRect(IntRect(40, 0, 40, 40));
	sprite_grid.setTexture(texture_blocks);
	sprite_grid.setTextureRect(IntRect(80, 0, 40, 40));
	sprite_selection.setTexture(texture_blocks);
	sprite_selection.setTextureRect(IntRect(120, 0, 40, 40));

	generateGameField(map_size);
}

void GameField::generateGameField(string& map_size)
{
	//Задание размера массиву игрового поля
	if (!map_size.compare("small")) {game_field_width = 50;	game_field_height = 20;	}
	else if (!map_size.compare("medium")) { game_field_width = 100;	game_field_height = 40; }
	else if (!map_size.compare("large")) { game_field_width = 200;	game_field_height = 80; }

	vector<vector<int>> set_game_field_size(game_field_height, vector<int>(game_field_width, 0));
	game_field_massive = set_game_field_size;

	//Заполнение массива игрового поля
	for (int i = 0; i < game_field_height; i++)
	{
		for (int j = 0; j < game_field_width; j++)
		{
			//Примитивный алгоритм генерации игрового поля, создающий несколько слоев блоков земли и один верхний слой блоков земли с травой.
			//Один слой земли "лишний" и является нижней границей игрового поля. Здесь также создаются левая и правая границы игрового поля
			if (i >= (game_field_height - game_field_dirt_height) || j == 0 || j == game_field_width - 1)
			{
				game_field_massive[i][j] = block_dirt;
			}
			else if (i == (game_field_height - game_field_grass_height - game_field_dirt_height))
			{
				game_field_massive[i][j] = block_grass;
			}
			else game_field_massive[i][j] = block_air;
		}
	}
}

void GameField::drawBackground(RenderWindow& game_window)
{
	game_window.draw(sprite_background);
}

void GameField::drawGameField(RenderWindow& game_window, Character& player, int& game_window_width, int& game_window_height)
{
	//Отрисовка в окне игры массива игрового поля
	for (int i = 0; i < game_field_height; i++)	
	{
		for (int j = 0; j < game_field_width; j++)
		{
			sprite_block_dirt.setPosition(j * block_size - player.char_offset_x, i * block_size - player.char_offset_y);
			sprite_block_grass.setPosition(j * block_size - player.char_offset_x, i * block_size - player.char_offset_y);
			if (game_field_massive[i][j] == 1) game_window.draw(sprite_block_dirt);
			if (game_field_massive[i][j] == 2) game_window.draw(sprite_block_grass);
		}
	}
}

void GameField::drawGrid(RenderWindow& game_window, Character& player)
{
	if (g_pressed_counter == 1)
	{
		for (int i = 0; i < game_field_height; i++)
		{
			for (int j = 0; j < game_field_width; j++)
			{
				sprite_grid.setPosition(j * block_size - player.char_offset_x, i * block_size - player.char_offset_y);
				game_window.draw(sprite_grid);
			}
		}
	}
}

void GameField::drawSelection(RenderWindow& game_window, Character& player, int& game_window_width, int& game_window_height)
{
	auto mouse_coord = Mouse::getPosition(game_window);

	if (mouse_coord.x < game_window_width && mouse_coord.x > 0 && mouse_coord.y < game_window_height && mouse_coord.y > 0)	//Проверка, находится ли курсор мыши в окне игры
	{
		//Вычисление координаты курсора в окне игры в блоках
		mouse_coord_x = (mouse_coord.x + player.char_offset_x) / block_size;
		mouse_coord_y = (mouse_coord.y + player.char_offset_y) / block_size;
		sprite_selection.setPosition(mouse_coord_x * block_size - player.char_offset_x, mouse_coord_y * block_size - player.char_offset_y);	//Установка позиции спрайта выделения блока
		game_window.draw(sprite_selection);
	}
}

void GameField::blockHarvest(RenderWindow& game_window, Character& player, Interface& block_selection, int& game_window_width, int& game_window_height)
{
	auto mouse_coord = Mouse::getPosition(game_window);

	if (mouse_coord.x < game_window_width && mouse_coord.x > 0 && mouse_coord.y < game_window_height && mouse_coord.y > 0)
	{
		mouse_coord_x = (mouse_coord.x + player.char_offset_x) / block_size;
		mouse_coord_y = (mouse_coord.y + player.char_offset_y) / block_size;
		//Условия, ограничивающие дальность сбора блоков
		if (mouse_coord_x < player.getCharFieldCoordX() + 4
			&& mouse_coord_x > player.getCharFieldCoordX() - 4
			&& mouse_coord_y < player.getCharFieldCoordY() + 4
			&& mouse_coord_y > player.getCharFieldCoordY() - 4)
		{
			block_selection.setBlocksCounterPlus(game_field_massive[mouse_coord_y][mouse_coord_x]);	//Увеличение значения счетчика блоков в инвентаре с учетом типа блока
			game_field_massive[mouse_coord_y][mouse_coord_x] = block_air;							//Удаление блока из массива игрового поля
		}
	}
}

void GameField::blockPlace(RenderWindow& game_window, Character& player, Interface& block_selection, int& game_window_width, int& game_window_height)
{
	auto mouse_coord = Mouse::getPosition(game_window);

	if (mouse_coord.x < game_window_width && mouse_coord.x > 0 && mouse_coord.y < game_window_height && mouse_coord.y > 0)
	{
		mouse_coord_x = (mouse_coord.x + player.char_offset_x) / block_size;
		mouse_coord_y = (mouse_coord.y + player.char_offset_y) / block_size;
		//Условия, ограничивающие дальность установки блоков, а также проверки на возможность установки блока и наличие выбранного типа блока в инвентаре
		if (mouse_coord_x < player.getCharFieldCoordX() + 4
			&& mouse_coord_x > player.getCharFieldCoordX() - 4
			&& mouse_coord_y < player.getCharFieldCoordY() + 4
			&& mouse_coord_y > player.getCharFieldCoordY() - 4
			&& game_field_massive[mouse_coord_y][mouse_coord_x] == block_air
			&& block_selection.getBlocksCounter(block_selection.getSelectedBlock()) != 0)
		{
			if (block_selection.getSelectedBlock() == 0)
			{
				block_selection.setBlocksCounterMinus(block_selection.getSelectedBlock());	//Уменьшение значения счетчика блоков в инвентаре с учетом типа блока
				game_field_massive[mouse_coord_y][mouse_coord_x] = block_dirt;				//Добавление блока в массив игрового поля с учетом типа блока
			}
			else if (block_selection.getSelectedBlock() == 1)
			{
				block_selection.setBlocksCounterMinus(block_selection.getSelectedBlock());
				game_field_massive[mouse_coord_y][mouse_coord_x] = block_grass;
			}
		}
	}
}

Sprite& GameField::getBlockSprite(char block_type)
{
	if (block_type == block_dirt) return sprite_block_dirt;
	if (block_type == block_grass) return sprite_block_grass;
}