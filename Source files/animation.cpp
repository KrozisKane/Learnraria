#include "game.h"

//Класс анимации
Animation::Animation()
{
	current_frame = 0;
	anim_speed = 0;
	is_flipped = false;
	is_looped = true;
}

void Animation::animPlayer(float time)
{
	current_frame += anim_speed * time;			//Прокрутка кадров анимации

	if (current_frame > anim_frames.size())		//Защита от выхода за пределы массива кадров
	{
		current_frame -= anim_frames.size();
	}

	anim_sprite.setTextureRect(anim_frames[current_frame]);								//Задание спрайту анимации текстуры текущего кадра
	if (is_flipped) anim_sprite.setTextureRect(anim_frames_flipped[current_frame]);		//То же, но при отзеркаливании
}

//Класс менеджера анимаций
AnimationManager::AnimationManager() {}

void AnimationManager::createAnim(string anim_name, Texture& anim_texture, int first_frame_x, int first_frame_y, int first_frame_width, int first_frame_height, int frames_count, float anim_speed, int anim_step, bool is_looped)
{
	Animation new_anim;
	new_anim.anim_speed = anim_speed;
	new_anim.is_looped = is_looped;
	new_anim.anim_sprite.setTexture(anim_texture);

	for (int i = 0; i < frames_count; i++)	//Цикл нарезки исходного файла текстур на кадры и их запись в соответствующие массивы
	{
		new_anim.anim_frames.push_back(IntRect(first_frame_x, first_frame_y + i * anim_step, first_frame_width, first_frame_height));
		new_anim.anim_frames_flipped.push_back(IntRect(first_frame_x + first_frame_width, first_frame_y + i * anim_step, -first_frame_width, first_frame_height));
	}
	anim_list[anim_name] = new_anim;
}

void AnimationManager::setAnim(string anim_name)
{
	current_anim = anim_name;
}

void AnimationManager::drawAnim(RenderWindow& game_window, int x, int y)
{
	//Данный метод позиционирует и выводит анимацию на экран. 
	//Некоторые из анимаций отображались некорректно, со сдвигом в ту или иную сторону на несколько пикселей. 
	//Причину этого я определить не смог, поэтому для некоторых анимаций для компенсации сдвигов пришлось городить костыли в виде ручного смещения по осям на несколько пикселей
	if (!anim_list[current_anim].is_flipped)
	{
		if (!(current_anim.compare("move"))) anim_list[current_anim].anim_sprite.setPosition(x - 2, y - 2);
		else if (!(current_anim.compare("move_and_use"))) anim_list[current_anim].anim_sprite.setPosition(x - 2, y);
		else anim_list[current_anim].anim_sprite.setPosition(x, y);
	}
	else 
	{
		if (!(current_anim.compare("move"))) anim_list[current_anim].anim_sprite.setPosition(x - 4, y - 2);
		else if (!(current_anim.compare("move_and_use"))) anim_list[current_anim].anim_sprite.setPosition(x - 4, y);
		else if (!(current_anim.compare("use"))) anim_list[current_anim].anim_sprite.setPosition(x - 2, y);
		else if (!(current_anim.compare("jump")) || !(current_anim.compare("jump_and_use"))) anim_list[current_anim].anim_sprite.setPosition(x - 4, y);
		else anim_list[current_anim].anim_sprite.setPosition(x, y);
	}

	game_window.draw(anim_list[current_anim].anim_sprite);
}