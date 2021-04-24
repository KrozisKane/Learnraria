#include "game.h"

Character::Character(GameField& game_field)
{
	char_coord_field_x = 0;
	char_coord_field_y = 0;
	char_vel_x = 0;
	char_vel_y = 0;
	char_accel_x = 1000;
	char_accel_y = 9.81 * 40;
	on_ground = false;
	space_state = false;
	move_dir = 0;
	view_dir = 2;
	char_state = IDLE;
	charAnimations.createAnim("idle", texture_character, 6, 10, 26, 44, 1, 0, 0, 1);
	charAnimations.createAnim("use", texture_character, 6, 66, 28, 44, 4, 8, 56, 1);
	charAnimations.createAnim("jump", texture_character, 6, 290, 30, 49, 1, 0, 0, 1);
	charAnimations.createAnim("move", texture_character, 4, 344, 32, 46, 14, 15, 56, 1);
	charAnimations.createAnim("jump_and_use", texture_character, 6, 1130, 30, 49, 4, 8, 56, 1);
	charAnimations.createAnim("move_and_use", texture_character, 4, 1354, 32, 46, 14, 15, 56, 1);
	charCollisionRect = FloatRect(0, 0, 26, 44);
	texture_character.loadFromFile("D:\\Programming\\Learnraria\\Resources\\images\\char_anim.png");

	char_offset_x = 0;
	char_offset_y = 0;

	charSetSpawnPosion(game_field);
}

void Character::charUpdate(RenderWindow& game_window, float& dt, GameField& game_field, int& game_window_width, int& game_window_height)
{
	keyCheck();														//Обработка нажатых клавиш
	setCharFieldCoord(game_field);									//Вычисление координаты персонажа в системе игрового поля (в блоках)
	gravity(dt, game_field);										//Применение гравитации к персонажу (если он прыгает/падает)
	charMove(game_field, dt);										//Перемещение персонажа (если нажата кнопка)
	charOffset(game_field, game_window_width, game_window_height);	//Сдвига игровой камеры, чтобы персонаж находился в центре окна игры
	charDraw(game_window, dt);										//Отрисовка персонажа (его анимаций)
}

void Character::charMove(GameField& game_field, float& dt)
{
	//Функция движения персонажа. От нажатой кнопки зависит знак скорости/ускорения
	if (move_dir == 2)
	{
		if (char_vel_x < 300) char_vel_x += char_accel_x * dt;
		else char_vel_x = 300;
	}
	else if (move_dir == 1)
	{
		if (char_vel_x > -300) char_vel_x -= char_accel_x * dt;
		else char_vel_x = -300;
	}
	//Данный код отвечает за плавную остановку персонажа при отжатии клавиши движения
	else if (move_dir == 0)
	{
		if (char_vel_x > 0)
		{
			char_vel_x -= char_accel_x * dt;
			if (char_vel_x < 0) char_vel_x = 0;
		}

		if (char_vel_x < 0)
		{
			char_vel_x += char_accel_x * dt;
			if (char_vel_x > 0) char_vel_x = 0;
		}
	}
	charCollisionRect.left += char_vel_x * dt;	//Вычисление координаты персонажа в зависимости от его скорости
	charCollision(game_field, 0);				//Вычисление коллизий по оси икс
}

void Character::charJump()
{
	if (on_ground) char_vel_y -= 200;	//Если персонаж стоит на земле, ему придается импульс вверх (персонаж делает прыжок)
	on_ground = false;
}

void Character::gravity(float& dt, GameField& game_field)
{
	charCollisionRect.top += char_vel_y * dt;
	char_vel_y += char_accel_y * dt;
	charCollision(game_field, 1);	//Вычисление коллизий по оси игрек
}

void Character::setCharFieldCoord(GameField& game_field)
{
	char_coord_field_x = charCollisionRect.left / game_field.block_size;
	char_coord_field_y = charCollisionRect.top / game_field.block_size + 1;		//Сдвиг на 1 блок вниз, т.к. координата спрайта считается от левого верхнего угла, а не от нижнего
}

void Character::charCollision(GameField& game_field, int axis)
{
	//Алгоритм просчета коллизии персонажа
	for (int i = charCollisionRect.top / game_field.block_size; i < (charCollisionRect.top + charCollisionRect.height) / game_field.block_size; i++)		//От координаты верхней точки персонажа (в блоках) до координаты нижней точки персонажа (в блоках)
		for (int j = charCollisionRect.left / game_field.block_size; j < (charCollisionRect.left + charCollisionRect.width) / game_field.block_size; j++)	//От координаты левой точки персонажа (в блоках) до координаты правой точки персонажа (в блоках)
		{
			if (game_field.game_field_massive[i][j] != 0)	//Если проверяемый блок игрового поля - воздух
			{
				//Условия проверки для оси икс
				if (char_vel_x > 0 && axis == 0) { charCollisionRect.left = j * game_field.block_size - charCollisionRect.width; char_vel_x = 0; }	//Персонаж упрется в левую часть блока (т.к. двигается вправо)
				if (char_vel_x < 0 && axis == 0) { charCollisionRect.left = j * game_field.block_size + game_field.block_size; char_vel_x = 0; }	//Персонаж упрется в правую часть блока (т.к. двигается влево)
				//Условия проверки для оси игрек
				if (char_vel_y > 0 && axis == 1) { charCollisionRect.top = i * game_field.block_size - charCollisionRect.height; char_vel_y = 0; on_ground = true;}	//Персонаж упрется в верхнюю часть блока (т.к. падает вниз)
				if (char_vel_y < 0 && axis == 1) { charCollisionRect.top = i * game_field.block_size + game_field.block_size; char_vel_y = 0; }						//Персонаж упрется в нижнюю часть блока (т.к. прыгает вверх)
			}
		}
}

void Character::charSetSpawnPosion(GameField& game_field)
{
	charCollisionRect.left = (game_field.game_field_width / 2) * game_field.block_size;
	for (int line_of_blocks = 1; line_of_blocks < game_field.game_field_height; line_of_blocks++)
	{
		if (game_field.game_field_massive[line_of_blocks][game_field.game_field_width / 2] == game_field.block_grass) 
			charCollisionRect.top = (line_of_blocks - 1) * game_field.block_size;	//Сдвиг на 1 блок вверх, т.к. координата спрайта считается от верхнего угла, а не от нижнего (чтобы персонаж не спавнился в блоках земли и травы)
	}
}

void Character::keyCheck()
{
	// Действия при нажатии клавиш
	if (control_key["A"])
	{
		move_dir = 1;
		view_dir = 1;
		if (char_state == IDLE) char_state = MOVE;
		if (char_state == USE) char_state = MOVE_AND_USE;
	}

	if (control_key["D"])
	{
		move_dir = 2;
		view_dir = 2;
		if (char_state == IDLE) char_state = MOVE;
		if (char_state == USE) char_state = MOVE_AND_USE;
	}

	if (control_key["Space"])
	{
		if (!space_state)
		{
			charJump();
			space_state = true;
			char_state = JUMP;
		}
		if (on_ground && char_state == JUMP) char_state = IDLE;
		if (on_ground && char_state == JUMP_AND_USE) char_state = USE;
	}

	if (control_key["Use"])
	{
		if (char_state == IDLE) char_state = USE;
		if (char_state == JUMP) char_state = JUMP_AND_USE;
		if (char_state == MOVE) char_state = MOVE_AND_USE;
	}

	// Действия при отжатии клавиш
	if (!(control_key["A"] || control_key["D"]))
	{
		move_dir = 0;	//Сброс переменной-индикатора заданного персонажу направления движения 
		if (char_state == MOVE) char_state = IDLE;
		if (char_state == MOVE_AND_USE) char_state = USE;
	}

	if (!control_key["Space"])
	{
		if (on_ground && char_state == JUMP) char_state = IDLE;
		if (on_ground && char_state == JUMP_AND_USE) char_state = USE;
	}

	if (!control_key["Use"])
	{
		if (char_state == USE) char_state = IDLE;
		if (!on_ground && char_state == JUMP_AND_USE) char_state = JUMP;
		if (char_state == MOVE_AND_USE) char_state = MOVE;
	}

	control_key["A"] = control_key["D"] = control_key["Space"] = control_key["Use"] = false;
}

void Character::charOffset(GameField& game_field, int& game_window_width, int& game_window_height)
{
	//Здесь указаны условия сдвига игровой камеры, чтобы персонаж находился в центре окна игры. При приближении к краю окна игры сдвиг относительно этого края прекращается
	//Примечание: игровое поле имеет границы шириной в один блок земли с трех сторон игрового поля (слева, снизу, справа), необходимые для предотвращения перемещения персонажа за пределы игрового поля.
	//Прибавление/вычитание game_field.block_size в условиях ниже не позволяет камере попадать на блоки границ и показывать их игроку (что также не позволяет ему разрушить их)
	//Сдвиг по ширине окна игры
	//Условие сдвига при расстоянии до границы игрового поля более, чем половина ширины окна игры
	if (charCollisionRect.left > game_window_width / 2 + game_field.block_size
		&& charCollisionRect.left < (game_field.game_field_width * game_field.block_size - game_window_width / 2) - game_field.block_size)
		char_offset_x = charCollisionRect.left - game_window_width / 2;
	//Условия сдвига при расстоянии до границы игрового поля менее, чем половина ширины окна игры (по условию для каждой стороны)
	else
	{
		if (charCollisionRect.left < game_window_width / 2) char_offset_x = 0 + game_field.block_size;
		if (charCollisionRect.left > (game_field.game_field_width * game_field.block_size - game_window_width / 2) - game_field.block_size)
			char_offset_x = game_field.game_field_width * game_field.block_size - game_window_width - game_field.block_size;
	}
	//Сдвиг по высоте окна игры
	//Условие сдвига при расстоянии до границы игрового поля более, чем половина высоты окна игры
	if (charCollisionRect.top > game_window_height / 2
		&& charCollisionRect.top < (game_field.game_field_height * game_field.block_size - game_window_height / 2) - game_field.block_size)
		char_offset_y = charCollisionRect.top - game_window_height / 2;
	//Условия сдвига при расстоянии до границы игрового поля менее, чем половина высоты окна игры (по условию для каждой стороны)
	else
	{
		if (charCollisionRect.top < game_window_height / 2) char_offset_y = 0;
		if (charCollisionRect.top > (game_field.game_field_height * game_field.block_size - game_window_height / 2) - game_field.block_size)
			char_offset_y = game_field.game_field_height * game_field.block_size - game_window_height - game_field.block_size;
	}
}

void Character::charDraw(RenderWindow& game_window, float& dt)
{
	//Ниже написаны условия переключения анимаций персонажа в зависимости от его состояний
	if (char_state == IDLE) charAnimations.setAnim("idle");
	if (char_state == MOVE) charAnimations.setAnim("move");
	if (char_state == JUMP) charAnimations.setAnim("jump");
	if (char_state == USE) charAnimations.setAnim("use");
	if (char_state == JUMP_AND_USE) charAnimations.setAnim("jump_and_use");
	if (char_state == MOVE_AND_USE) charAnimations.setAnim("move_and_use");
	if (view_dir == 1) charAnimations.flipAnim(1);	//Условия отзеркаливания анимации в зависимости от направления взгляда. Необходимы для установки и сохранения направления "взгляда" персонажа (влево или вправо)
	else charAnimations.flipAnim(0);
	charAnimations.playAnim(dt);					//Включение таймера анимации
	charAnimations.drawAnim(game_window, charCollisionRect.left - char_offset_x, charCollisionRect.top - char_offset_y);	//Отрисовка и установка позиции персонажа с учетом сдвига камеры в центр экрана
}