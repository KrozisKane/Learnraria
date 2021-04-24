#pragma once
#include <vector>
#include <iostream>
#include <SFML/Graphics.hpp>

using namespace sf;
using namespace std;

class GameField;

//Класс интефрейса инвентаря пперсонажа
class Interface
{
private:
	int inventory_coord_x;			//Координата интерфейса инвентаря по оси икс
	int inventory_coord_y;			//Координата интерфейса инвентаря по оси игрек
	int blocks_counter[2] = { 0 }; 	//Переменная-счетчик блоков в инвентаре персонажа (для 2-х типов блоков)
	int selected_block;				//Переменная-индикатор выбранного блока
	Texture texture_inventory, texture_selected_block;
	Sprite sprite_inventory, sprite_selected_block;
	Font font_block_counter;
	Text text_blocks_counter[2];

public:
	Interface();

	void drawBlocksSelection(RenderWindow& game_window, GameField& game_field);	//Метод отрисовки выбора блока
	void setBlocksCounterPlus(int block_id);									//Метод увеличения количества блоков в инвентаре персонажа
	void setSelectedBlock(char delta);											//Метод выбора блока
	int& getSelectedBlock() { return selected_block; };							//Метод-индикатор выбранного блока
	int& getBlocksCounter(int block_id) { return blocks_counter[block_id]; };	//Метод получения количества блоков одного типа в инвентаре
	void setBlocksCounterMinus(int block_id) { blocks_counter[block_id]--; };	//Метод уменьшения количества блоков в инвентаре персонажа
};

//Класс анимации
class Animation
{
public:
	std::vector<IntRect> anim_frames, anim_frames_flipped;	//Массивы для хранения кадров и их отзеркаленной версии
	float current_frame;									//Переменная для хранения значения текущего кадра
	float anim_speed;										//Переменная для хранения значения текущего кадра и скорости анимации
	bool is_looped;											//Переменная-индикатор зацикленности анимации
	bool is_flipped;										//Переменная-индикатор отзеркаливания анимации
	Sprite anim_sprite;

	Animation();

	void animPlayer(float time);	//Метод проигрывания анимации
};

//Класс менеджера анимаций
class AnimationManager
{
public:
	string current_anim;				//Переменная для хранения имени текущей анимации
	map<string, Animation> anim_list;	//Ассоциативный массив "имя анимации - анимация"

	AnimationManager();

	//Методы для управления анимациями. Предназначение каждого метода ясно из его названия
	void createAnim(string anim_name, Texture& anim_texture, int first_frame_x, int first_frame_y, int first_frame_width, int first_frame_height, int frames_count, float anim_speed, int anim_step, bool is_looped);
	void setAnim(string name);
	void drawAnim(RenderWindow& window, int x, int y);
	void flipAnim(bool flip) { anim_list[current_anim].is_flipped = flip; }
	void playAnim(float time) { anim_list[current_anim].animPlayer(time); }
};

//Класс персонажа
class Character
{
private:
	int char_coord_field_x;		//Координата персонажа в системе координат блоков игрового поля (по оси икс)
	int char_coord_field_y;		//Координата персонажа в системе координат блоков игрового поля (по оси игрек)
	float char_vel_x;			//Скорость персонажа по оси икс
	float char_vel_y;			//Скорость персонажа по оси игрек
	float char_accel_x;			//Ускорение персонажа по оси икс
	float char_accel_y;			//Ускорение персонажа по оси игрек
	bool on_ground;				//Переменная-индикатор состояния персонажа (в воздухе он или нет)
	bool space_state;			//Переменная-индикатор состояния клавиши пробел (нажата или нет). Используется для предотвращения бесконечных прыжков при зажатом пробеле
	char move_dir;				//Переменная-индикатор заданного персонажу направления движения
	char view_dir;				//Переменная-индикатор заданного персонажу направления взгляда
	enum						//Перечисление для хранения состояний персонажа
	{ 
		IDLE, 
		MOVE, 
		JUMP, 
		USE, 
		MOVE_AND_USE, 
		JUMP_AND_USE 
	}	char_state;
	AnimationManager charAnimations;	//Объект для управления анимациями персонажа
	FloatRect charCollisionRect;		//Объект, отвечающий за коллизию персонажа и его координаты
	Texture texture_character;

public:
	float char_offset_x;				//Переменная для сдвига экарана относительно персонажа (чтобы он был в центре) по оси икс
	float char_offset_y;				//Переменная для сдвига экарана относительно персонажа (чтобы он был в центре) по оси игрек
	map<string, bool> control_key;		//Ассоциативный массив для кнопок управления

	Character(GameField& game_field);

	void charUpdate(RenderWindow& game_window, float& dt, GameField& game_field, int& game_window_width, int& game_window_height);	//Метод обновления персонажа
	void charMove(GameField& game_field, float& dt);			//Метод движения персонажа
	void charJump();											//Метод прыжка персонажа
	void gravity(float& dt, GameField& game_field);				//Метод, отвечающая за гравитацию, действующую на персонажа
	void setCharFieldCoord(GameField& game_field);				//Метод вычисления координаты персонажа в системе координат блоков
	void charCollision(GameField& game_field, int dir);			//Метод, реализующая коллизию персонажа
	void charSetSpawnPosion(GameField& game_field);				//Метод задания персонажу координат при спавне на карте
	void keyCheck();											//Метод обработки нажатых клавиш
	void charOffset(GameField& game_field, int& game_window_width, int& game_window_height);	//Метод для сдвига игровой камеры, чтобы персонаж находился в центре окна игры
	void charDraw(RenderWindow& game_window, float& dt);		//Метод отрисовки персонажа
	int& getCharFieldCoordX() { return char_coord_field_x; }	//Метод получения координаты персонажа в системе координат блоков по оси икс
	int& getCharFieldCoordY() { return char_coord_field_y; }	//Метод получения координаты персонажа в системе координат блоков по оси игрек
	void setSpaceState(bool state) { space_state = state; }		//Метод для изменения состояния клавиши пробел
};

//Класс игрового поля
class GameField
{
private:
	bool gamefield_generated;		//Переменная для проверки, что игровое поле уже сгенерировано
	char g_pressed_counter;			//Переменна-счетчик нажатий кнопки "G" (отрисовка сетки)
	int game_field_dirt_height;		//Высота слоя земли в блоках
	int game_field_grass_height;	//Высота слоя травы в блоках
	int mouse_coord_x;				//Координата мыши по оси икс
	int mouse_coord_y;				//Координата мыши по оси игрек
	Texture texture_background, texture_blocks;
	Sprite sprite_background, sprite_block_dirt, sprite_block_grass, sprite_grid, sprite_selection;

public:
	int game_field_width;					//Ширина игрового поля в блоках
	int game_field_height;					//Высота игрового поля в блоках
	const int block_size = 40;				//Размер одного блока в пикселях
	vector<vector<int>> game_field_massive; //Двумерный массив, определяющий размер игрового поля в блоках
	enum block_type
	{
		block_air,
		block_dirt,
		block_grass
	};

	GameField(string& map_size);

	void generateGameField(string& map_size);				//Метод генерации игрового поля
	void drawBackground(RenderWindow& game_window);			//Метод отрисовки фона
	void drawGameField(RenderWindow& game_window, Character& player, int& game_window_width, int& game_window_height);	//Метод отрисовки игрового поля
	void drawGrid(RenderWindow& game_window, Character& player);														//Метод отрисовки сетки блоков игрвого поля. Данный метод отладочный и будет удален в будущих версиях
	void drawSelection(RenderWindow& game_window, Character& player, int& game_window_width, int& game_window_height);	//Метод отрисовки выбранного на игровом поле блока
	void blockHarvest(RenderWindow& game_window, Character& player, Interface& block_selection, int& game_window_width, int& game_window_height);	//Метод собирания блока
	void blockPlace(RenderWindow& game_window, Character& player, Interface& block_selection, int& game_window_width, int& game_window_height);		//Метод установки блока
	Sprite& getBlockSprite (char block_type);				//Метод получения спрайта блоков для интерфейса
	void setGCounterValuePlus() { g_pressed_counter++; }	//Метод-счетчик нажатий клавиши "G"
	void setGCounterValueZero() { g_pressed_counter = 0; }	//Метод-обнулитель счетчика нажатий клавиши "G"
	char& getGCounterValue() { return g_pressed_counter; }	//Метод для получения количества нажайти клавиши "G"
};

// Класс окна игры
class GameWindow
{
private:
	int game_window_width;	//Ширина окна игры
	int game_window_height;	//Высота окна игры
	int FPS_limit;			//Переменная-ограничитель фпс
	float dt;				//Переменная-дельта для интегратора (необходима для связи фпс и физики игры)

public:
	GameWindow(int& set_game_window_width, int& set_game_window_height);
	void startGame(string& map_size);		//Метод создания игрового окна
};