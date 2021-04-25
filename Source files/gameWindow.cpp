#include "game.h"

GameWindow::GameWindow(int& set_game_window_width, int& set_game_window_height)
{
	game_window_width = set_game_window_width;
	game_window_height = set_game_window_height;
	FPS_limit = 60;
	dt = 0;
}

void GameWindow::startGame(string& map_size)
{
	GameField game_field(map_size);
	Character player(game_field);
	Interface block_selection;

	RenderWindow game_window(VideoMode(game_window_width, game_window_height), "Learnraria v0.4 by Krozis Kane");
	game_window.setFramerateLimit(FPS_limit);

	Clock delta_t_clock;

	while (game_window.isOpen())
	{
		Event game_main_event;

		dt = delta_t_clock.restart().asSeconds();

		while (game_window.pollEvent(game_main_event))
		{
			if (game_main_event.type == Event::Closed) game_window.close();

			if (game_main_event.type == Event::KeyPressed)
			{
				if (game_main_event.key.code == Keyboard::Escape)
				{
					game_window.close();
				}

				if (game_main_event.key.code == Keyboard::G)
				{
					game_field.setGCounterValuePlus();
					if (game_field.getGCounterValue() == 2) game_field.setGCounterValueZero();
				}
			}

			if (game_main_event.type == Event::KeyReleased)
			{
				if (game_main_event.key.code == Keyboard::Space)	//Отслеживание отжатия клавиши пробел
				{
					player.setSpaceState(false);
				}
			}
			
			if (game_main_event.type == Event::MouseWheelScrolled)
			{
				block_selection.setSelectedBlock(game_main_event.mouseWheelScroll.delta);
			}
		}

		if (Keyboard::isKeyPressed(Keyboard::Space))
		{
			player.control_key["Space"] = true;
		}

		if (Mouse::isButtonPressed(Mouse::Left))
		{
			game_field.blockHarvest(game_window, player, block_selection, game_window_width, game_window_height);
			player.control_key["Use"] = true;
		}

		if (Mouse::isButtonPressed(Mouse::Right))
		{
			game_field.blockPlace(game_window, player, block_selection, game_window_width, game_window_height);
			player.control_key["Use"] = true;
		}

		if (Keyboard::isKeyPressed(Keyboard::A))
		{
			player.control_key["A"] = true;
		}

		if (Keyboard::isKeyPressed(Keyboard::D))
		{
			player.control_key["D"] = true;
		}

		game_window.clear();

		game_field.drawBackground(game_window);
		game_field.drawGameField(game_window, player, game_window_width, game_window_height);
		game_field.drawGrid(game_window, player);
		game_field.drawSelection(game_window, player, game_window_width, game_window_height);
		player.charUpdate(game_window, dt, game_field, game_window_width, game_window_height);
		block_selection.drawBlocksSelection(game_window, game_field);

		game_window.display();
	}
}
