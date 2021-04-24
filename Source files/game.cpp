#include "game.h"

int main()
{
	int set_game_window_width = 1280;	//Настройка ширины окна игры
	int set_game_window_height = 720;	//Настройка высоты окна игры
	string map_size = "small";			//Настройка размера карты игры (small, medium, large)

	GameWindow main_window(set_game_window_width, set_game_window_height);	//Создание объекта класса окна игры
	main_window.startGame(map_size);										//Запуск игры

	return 0;
}