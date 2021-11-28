#include "game.h"

Animation::Animation()
	: current_frame{ 0 }, animation_speed{ 0 }, is_looped{ true }, is_flipped{ false } {}

void Animation::animationPlayer(float& dt)
{
	current_frame += animation_speed * dt;
	if (current_frame > animation_frames.size()) current_frame = 0;

	animation_sprite.setTextureRect(animation_frames[current_frame]);
	if (is_flipped) animation_sprite.setTextureRect(animation_frames_flipped[current_frame]);
}

AnimationsManager::AnimationsManager() {}

void AnimationsManager::createAnimation(string animation_name, Texture& animation_texture, int first_frame_x, int first_frame_y, int first_frame_width, int first_frame_height, int frames_amount, float animation_speed, int animation_step, bool is_looped)
{
	Animation new_animation;
	new_animation.animation_speed = animation_speed;
	new_animation.is_looped = is_looped;
	new_animation.animation_sprite.setTexture(animation_texture);

	for (int frame{ 0 }; frame < frames_amount; frame++)
	{
		new_animation.animation_frames.push_back(IntRect(first_frame_x, first_frame_y + frame * animation_step, first_frame_width, first_frame_height));
		new_animation.animation_frames_flipped.push_back(IntRect(first_frame_x + first_frame_width, first_frame_y + frame * animation_step, -first_frame_width, first_frame_height));
	}
	animations_list[animation_name] = new_animation;
}

void AnimationsManager::updateAnimation(RenderWindow& game_window, int x, int y)
{
	//Some of the animations were displayed incorrectly, with a shift in one direction or another by a few pixels. 
	//I could not determine the reason for this, so to compensate these shifts, 
	//I had to manually add offsets along the axes by several pixels.
	if (!animations_list[current_animation].is_flipped)
	{
		if (!(current_animation.compare("move"))) animations_list[current_animation].animation_sprite.setPosition(x - 2, y - 2);
		else if (!(current_animation.compare("move_and_use"))) animations_list[current_animation].animation_sprite.setPosition(x - 2, y);
		else animations_list[current_animation].animation_sprite.setPosition(x, y);
	}
	else 
	{
		if (!(current_animation.compare("move"))) animations_list[current_animation].animation_sprite.setPosition(x - 4, y - 2);
		else if (!(current_animation.compare("move_and_use"))) animations_list[current_animation].animation_sprite.setPosition(x - 4, y);
		else if (!(current_animation.compare("use"))) animations_list[current_animation].animation_sprite.setPosition(x - 2, y);
		else if (!(current_animation.compare("jump")) || !(current_animation.compare("jump_and_use"))) animations_list[current_animation].animation_sprite.setPosition(x - 4, y);
		else animations_list[current_animation].animation_sprite.setPosition(x, y);
	}

	game_window.draw(animations_list[current_animation].animation_sprite);
}