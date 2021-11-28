#include "game.h"

GameWindow::GameWindow(int& set_game_window_width, int& set_game_window_height)
	: game_window_width{ set_game_window_width }, game_window_height{ set_game_window_height }, 
	fps_limit{ 60 }, dt{ 0 }, selected_menu_item{ 0 }, sound_played{ false }
{
	menu_background_texture.loadFromFile("Resources\\images\\background.png");
	logo_texture.loadFromFile("Resources\\images\\logo.png");
	save_before_exit_texture.loadFromFile("Resources\\images\\save_before_exit.png");
	menu_font.loadFromFile("Resources\\fonts\\comic_sans.ttf");
	menu_background_sprite.setTexture(menu_background_texture);
	logo_sprite.setTexture(logo_texture, true);
	save_before_exit_sprite.setTexture(save_before_exit_texture, true);
	logo_sprite.setPosition(game_window_width / 2 - logo_sprite.getTextureRect().width / 2, game_window_height / 2 - logo_sprite.getTextureRect().height / 2 - 230);
	save_before_exit_sprite.setPosition(game_window_width / 2 - save_before_exit_sprite.getTextureRect().width / 2, game_window_height / 2 - save_before_exit_sprite.getTextureRect().height / 2);

	//Configuring menu items
	menu_item_text[0].setString("New Game");
	menu_item_text[1].setString("Load Game");
	menu_item_text[2].setString("About");
	menu_item_text[3].setString("Exit");
	for (int string_id{ 0 }; string_id < 4; string_id++)
	{
		menu_item_text[string_id].setFont(menu_font);
		menu_item_text[string_id].setStyle(Text::Bold);
		menu_item_text[string_id].setCharacterSize(40);
		menu_item_text[string_id].setOutlineThickness(1);
		menu_item_text[string_id].setOutlineColor(Color::Black);
		menu_item_text[string_id].setFillColor(Color(146, 146, 146, 255));
	}

	//Configuring the content of the third menu item
	about_text[0].setString("This game is developing by Konstantin \"Krozis Kane\" Solomon for C++ learning and practice.");
	about_text[1].setString("Source code of this game is distributed under the GNU General Public License v3.0.");
	about_text[2].setString("It could be found here: github.com/KrozisKane/Learnraria");
	about_text[3].setString("Used materials are trademarks and copyrights of Re-Logic.");
	about_text[4].setString("Learnraria v0.5");
	for (int string_id{ 0 }; string_id < 5; string_id++)
	{
		about_text[string_id].setFont(menu_font);
		about_text[string_id].setCharacterSize(28);
		about_text[string_id].setOutlineThickness(1);
		about_text[string_id].setOutlineColor(Color::Black);
		about_text[string_id].setFillColor(Color(255, 255, 255, 255));
		about_text[string_id].setPosition(game_window_width / 2 - about_text[string_id].getGlobalBounds().width / 2, game_window_height / 2 - about_text[string_id].getGlobalBounds().height / 2 - 65 + string_id * 40);
	}
	about_text[4].setPosition(game_window_width - about_text[4].getGlobalBounds().width - 20, game_window_height - about_text[4].getGlobalBounds().height - 20);	//For the last line, the coordinates are overridden

	//Configuring the save and exit window
	save_before_exit_text[0].setString("Save before exit?");
	save_before_exit_text[1].setString("Yes");
	save_before_exit_text[2].setString("No");
	for (int string_id{ 0 }; string_id < 3; string_id++)
	{
		save_before_exit_text[string_id].setFont(menu_font);
		save_before_exit_text[string_id].setStyle(Text::Bold);
		save_before_exit_text[string_id].setCharacterSize(40);
		save_before_exit_text[string_id].setOutlineThickness(1);
		save_before_exit_text[string_id].setOutlineColor(Color::Black);
		save_before_exit_text[string_id].setFillColor(Color(146, 146, 146, 255));
	}
	save_before_exit_text[0].setPosition(game_window_width / 2 - save_before_exit_text[0].getGlobalBounds().width / 2, game_window_height / 2 - save_before_exit_text[0].getGlobalBounds().height / 2 - 50);

	//Configuring menu sounds and music
	menu_item_sound_buffer.loadFromFile("Resources\\sounds\\menu_tick.wav");
	save_before_exit_open_sound_buffer.loadFromFile("Resources\\sounds\\save_menu_open.wav");
	save_before_exit_close_sound_buffer.loadFromFile("Resources\\sounds\\save_menu_close.wav");
	menu_item_sound.setBuffer(menu_item_sound_buffer);
	save_before_exit_open_sound.setBuffer(save_before_exit_open_sound_buffer);
	save_before_exit_close_sound.setBuffer(save_before_exit_close_sound_buffer);

	menu_music.openFromFile("Resources\\sounds\\menu_music.wav");
	game_music.openFromFile("Resources\\sounds\\game_music.wav");
	menu_music.setLoop(true);
	game_music.setLoop(true);
}

void GameWindow::saveGame(GameField& game_field, Character& player, Interface& hotbar)
{
	ofstream write_to_file("gamedata.lrnrr");

	//Writing the size of the game field
	write_to_file << game_field.getGameFieldHeight() << " " << game_field.getGameFieldWidth() << " ";

	//Writing the array of game field tiles
	for (int i{ 0 }; i < game_field.getGameFieldHeight(); i++)
	{
		for (int j{ 0 }; j < game_field.getGameFieldWidth(); j++)
		{
			write_to_file << game_field.getGameFieldTileType(i, j) << " ";
		}
	}

	//Writing the array of game field walls
	for (int i{ 0 }; i < game_field.getGameFieldHeight(); i++)
	{
		for (int j{ 0 }; j < game_field.getGameFieldWidth(); j++)
		{
			write_to_file << game_field.getBackGameFieldTileType(i, j) << " ";
		}
	}

	//Writing data about trees
	game_field.getGameFieldTreesList().saveTrees(write_to_file);

	//Writing player coordinates
	write_to_file << "!" << " " << player.getCharacterGameFieldX() << " " << player.getCharacterGameFieldY() << " ";

	//Writing data about tiles in hotbar
	hotbar.saveHotbarTiles(write_to_file);
}

void GameWindow::about(RenderWindow& game_window)
{
	while (game_window.isOpen())
	{
		Event about_event;
		while (game_window.pollEvent(about_event))
		{
			if (about_event.type == Event::Closed) game_window.close();

			if (about_event.type == Event::KeyPressed)
			{
				if (about_event.key.code == Keyboard::Escape)
				{
					return;
				}
			}
		}

		game_window.clear();

		game_window.draw(menu_background_sprite);
		for (int string_id{ 0 }; string_id < 5; string_id++)
		{
			game_window.draw(about_text[string_id]);
		}

		game_window.display();
	}
}

void GameWindow::mainMenu()
{
	RenderWindow game_window(VideoMode(game_window_width, game_window_height), "Learnraria v0.5 by Krozis Kane");
	game_window.setFramerateLimit(fps_limit);

	menu_music.play();

	while (game_window.isOpen())
	{
		Event menu_event;
		while (game_window.pollEvent(menu_event))
		{
			if (menu_event.type == Event::Closed) game_window.close();

			if (menu_event.type == Event::KeyPressed)
			{
				if (menu_event.key.code == Keyboard::Escape)
				{
					game_window.close();
				}
			}
		}

		//Loop that is responsible for menu interactivity
		for (int item{ 0 }; item < 4; item++)
		{
			if (menu_item_text[item].getGlobalBounds().contains(static_cast<Vector2f>(Mouse::getPosition(game_window))))
			{
				if (!sound_played[item]) menu_item_sound.play();
				sound_played[item] = true;
				menu_item_text[item].setFillColor(Color(255, 215, 0, 255));
				menu_item_text[item].setScale(1.2, 1.2);
				menu_item_text[item].setPosition(game_window_width / 2 - menu_item_text[item].getGlobalBounds().width / 2, game_window_height / 2 - menu_item_text[item].getGlobalBounds().height / 2 - 100 + item * 63);
				if (Mouse::isButtonPressed(Mouse::Left)) selected_menu_item = item + 1;		//Adding "+ 1" is necessary because a value of 0 for selected_menu_item means that no menu item is selected
			}
			else
			{
				if (sound_played[item]) sound_played[item] = false;
				menu_item_text[item].setFillColor(Color(146, 146, 146, 200));
				menu_item_text[item].setScale(1, 1);
				menu_item_text[item].setPosition(game_window_width / 2 - menu_item_text[item].getGlobalBounds().width / 2, game_window_height / 2 - menu_item_text[item].getGlobalBounds().height / 2 - 100 + item * 63);
			}
		}

		switch(selected_menu_item)
		{
			case 1:
				menu_music.stop();
				startGame(game_window, 0);
				selected_menu_item = 0;
				break;
			case 2:
				menu_music.stop();
				startGame(game_window, 1);
				selected_menu_item = 0;
				break;
			case 3:
				about(game_window);
				selected_menu_item = 0;
				break;
			case 4:
				game_window.close();
				break;
		}
		if (game_music.getStatus() == game_music.Playing) game_music.stop();
		if (menu_music.getStatus() == menu_music.Stopped) menu_music.play();

		game_window.clear();

		game_window.draw(menu_background_sprite);
		game_window.draw(logo_sprite);
		for (int item{ 0 }; item < 4; item++)
		{
			game_window.draw(menu_item_text[item]);
		}

		game_window.display();
	}
}

bool GameWindow::saveAndExitToMenu(RenderWindow& game_window, GameField& game_field, Character& player, Interface& hotbar)
{
	Texture game_state_screenshot;		//I have not yet added the ability to pause the game,
	Sprite game_state;					//so I have to use this kludge

	game_state_screenshot.create(game_window_width, game_window_height);
	game_state_screenshot.update(game_window);
	game_state.setTexture(game_state_screenshot);

	while (game_window.isOpen())
	{
		Event save_and_exit_event;
		while (game_window.pollEvent(save_and_exit_event))
		{
			if (save_and_exit_event.type == Event::Closed) game_window.close();

			if (save_and_exit_event.type == Event::KeyPressed)
			{
				if (save_and_exit_event.key.code == Keyboard::Escape)
				{
					return 0;
				}
			}
		}

		for (int string_id{ 1 }; string_id < 3; string_id++)
		{
			if (save_before_exit_text[string_id].getGlobalBounds().contains(static_cast<Vector2f>(Mouse::getPosition(game_window))))
			{
				if (!sound_played[string_id]) menu_item_sound.play();
				sound_played[string_id] = true;
				save_before_exit_text[string_id].setFillColor(Color(255, 215, 0, 255));
				save_before_exit_text[string_id].setScale(1.2, 1.2);
				if (string_id == 1)
				{
					save_before_exit_text[string_id].setPosition(game_window_width / 2 - save_before_exit_text[string_id].getGlobalBounds().width / 2 - 88, game_window_height / 2 - save_before_exit_text[string_id].getGlobalBounds().height / 2 + 25);
					if (Mouse::isButtonPressed(Mouse::Left))
					{
						saveGame(game_field, player, hotbar);	//Save and exit to the main menu
						return 1;
					}
				}
				else
				{
					save_before_exit_text[string_id].setPosition(game_window_width / 2 - save_before_exit_text[string_id].getGlobalBounds().width / 2 + 95, game_window_height / 2 - save_before_exit_text[string_id].getGlobalBounds().height / 2 + 25);
					if (Mouse::isButtonPressed(Mouse::Left)) return 1;	//Exit to the main menu without saving
				}
			}
			else
			{
				if (sound_played[string_id]) sound_played[string_id] = false;
				save_before_exit_text[string_id].setFillColor(Color(146, 146, 146, 255));
				save_before_exit_text[string_id].setScale(1, 1);
				if (string_id == 1) save_before_exit_text[string_id].setPosition(game_window_width / 2 - save_before_exit_text[string_id].getGlobalBounds().width / 2 - 88, game_window_height / 2 - save_before_exit_text[string_id].getGlobalBounds().height / 2 + 25);
				else save_before_exit_text[string_id].setPosition(game_window_width / 2 - save_before_exit_text[string_id].getGlobalBounds().width / 2 + 95, game_window_height / 2 - save_before_exit_text[string_id].getGlobalBounds().height / 2 + 25);
			}
		}
		game_window.clear();

		game_window.draw(game_state);
		game_window.draw(save_before_exit_sprite);
		for (int string_id{ 0 }; string_id < 3; string_id++)
		{
			game_window.draw(save_before_exit_text[string_id]);
		}

		game_window.display();
	}
}

void GameWindow::startGame(RenderWindow& game_window, bool new_or_load)
{
	GameField game_field(new_or_load);
	Character player(game_field, new_or_load);
	Interface hotbar(game_field, new_or_load);

	Clock delta_t_clock;

	game_music.play();

	while (game_window.isOpen())
	{
		Event game_event;

		dt = delta_t_clock.restart().asSeconds();

		while (game_window.pollEvent(game_event))
		{
			if (game_event.type == Event::Closed) game_window.close();

			if (game_event.type == Event::KeyPressed)
			{
				if (game_event.key.code == Keyboard::Escape)
				{
					save_before_exit_open_sound.play();
					if (saveAndExitToMenu(game_window, game_field, player, hotbar)) return;
					save_before_exit_close_sound.play();
				}
			}

			if (game_event.type == Event::KeyReleased)
			{
				if (game_event.key.code == Keyboard::Space)
				{
					player.setSpaceState(false);
				}
			}
			
			if (game_event.type == Event::MouseWheelScrolled)
			{
				hotbar.setSelectedTileType(game_event.mouseWheelScroll.delta);
			}
		}

		if (Keyboard::isKeyPressed(Keyboard::Space))
		{
			player.setControlKeyStateTrue("Space");
		}

		if (Mouse::isButtonPressed(Mouse::Left))
		{
			game_field.mineTile(game_window, player, hotbar, game_window_width, game_window_height);
			player.setControlKeyStateTrue("Use");
		}

		if (Mouse::isButtonPressed(Mouse::Right))
		{
			game_field.placeTile(game_window, player, hotbar, game_window_width, game_window_height);
			player.setControlKeyStateTrue("Use");
		}

		if (Keyboard::isKeyPressed(Keyboard::A))
		{
			player.setControlKeyStateTrue("A");
		}

		if (Keyboard::isKeyPressed(Keyboard::D))
		{
			player.setControlKeyStateTrue("D");
		}

		game_window.clear();

		game_field.updateBackground(game_window);
		game_field.updateGameField(game_window, player, dt, game_window_width, game_window_height);
		player.updateCharacter(game_window, dt, game_field, game_window_width, game_window_height, hotbar);
		hotbar.updateHotbar(game_window, game_field);

		game_window.display();
	}
}
