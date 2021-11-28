#include "game.h"

int main()
{
	int set_game_window_width{ 1280 };
	int set_game_window_height{ 720 };

	GameWindow main_window(set_game_window_width, set_game_window_height);
	main_window.mainMenu();

	return 0;
}