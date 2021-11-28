#pragma once
#include <vector>
#include <random>
#include <string>
#include <fstream>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

using namespace sf;
using namespace std;

class GameField;

class Interface
{
private:
	int hotbar_x;
	int hotbar_y;
	int selected_tile_type;
	int tile_types;
	vector<int> tiles_counter;
	Texture hotbar_texture, selected_tile_type_texture, tile_type_texture;
	Sprite hotbar_sprite, selected_tile_type_sprite, tile_type_sprite;
	Font tiles_counter_font;
	vector<Text> tiles_counter_text;

	void loadHotbarTiles();

public:
	Interface(GameField& game_field, bool new_or_load);

	void updateHotbar(RenderWindow& game_window, GameField& game_field);
	void setSelectedTileType(float& delta);
	void saveHotbarTiles(ofstream& write_to_file);
	int& getSelectedTileType() { return selected_tile_type; }
	int& getTilesAmount(int& tile_type) { return tiles_counter[tile_type]; }
	void setTilesCounterMinus(int& tile_type) { tiles_counter[tile_type]--; }
	void setTilesCounterPlus(int& tile_type) { if (tile_type != 0 && tile_type != 1) tiles_counter[tile_type - 2]++; }
};

class Animation
{
private:
	vector<IntRect> animation_frames, animation_frames_flipped;
	float current_frame;
	float animation_speed;
	bool is_looped;
	bool is_flipped;
	Sprite animation_sprite;

	void animationPlayer(float& dt);

public:
	Animation();

	friend class AnimationsManager;
};

class AnimationsManager
{
private:
	string current_animation;
	map<string, Animation> animations_list;

public:
	AnimationsManager();

	void createAnimation(string animation_name, Texture& animation_texture, int first_frame_x, int first_frame_y,  int first_frame_width, int first_frame_height, int frames_amount, float animation_speed, int animation_step, bool is_looped);
	void updateAnimation(RenderWindow& game_window, int x, int y);
	void flipAnimation(bool flip) { animations_list[current_animation].is_flipped = flip; }
	void playAnimation(float dt) { animations_list[current_animation].animationPlayer(dt); }
	void setAnimation(string animation_name) { current_animation = animation_name; }
};

class PhysicalObject
{
protected:
	float velocity_x;
	float velocity_y;
	float acceleration_x;
	float acceleration_y;
	bool is_on_ground;
	FloatRect physical_box;

	PhysicalObject(FloatRect physical_box_size, float acceleration_x);

	void checkCollisions(GameField& game_field, bool axis);
	void applyGravity(GameField& game_field, float& dt);
};

class Character : public PhysicalObject
{
private:
	int character_game_field_x;
	int character_game_field_y;	
	bool space_state;
	int move_direction;
	int view_direction;
	int character_offset_x;
	int character_offset_y;
	map<string, bool> control_key;
	enum CharacterState
	{
		IDLE,
		MOVE,
		JUMP,
		USE,
		MOVE_AND_USE,
		JUMP_AND_USE
	}	character_state;
	AnimationsManager character_animations;
	Texture character_texture;
	SoundBuffer get_item_sound_buffer;
	Sound get_item_sound;

	void characterMove(GameField& game_field, float& dt);
	void characterJump();
	void setCharacterGameFieldCoordinates(GameField& game_field);
	void setCharacterSpawnPosition(GameField& game_field);
	void loadCharacterSpawnPosition(GameField& game_field);
	void setAndPlayCharacterAnimation(RenderWindow& game_window, float& dt);
	void keyCheck();
	void getItem(GameField& game_field, Interface& hotbar);
	void characterOffset(GameField& game_field, int& game_window_width, int& game_window_height);

public:
	Character(GameField& game_field, bool new_or_load);

	void updateCharacter(RenderWindow& game_window, float& dt, GameField& game_field, int& game_window_width, int& game_window_height, Interface& hotbar);
	void setSpaceState(bool state) { space_state = state; }
	int& getCharacterGameFieldX() { return character_game_field_x; }
	int& getCharacterGameFieldY() { return character_game_field_y; }
	FloatRect& getCharacterPhysicalBox() { return physical_box; }
	int& getCharacterOffsetX() { return character_offset_x; }
	int& getCharacterOffsetY() { return character_offset_y; }
	void setControlKeyStateTrue(string key) { control_key[key] = true; }
};

class TileType
{
private:
	string tile_type_name;
	bool has_collision;
	Sprite tile_type_sprite;

public:
	TileType();

	Sprite& getTileTypeSprite() { return tile_type_sprite; }
	bool& getTileTypeCollision() { return has_collision; }

	friend class TileTypesManager;
};

class TileTypesManager
{
private:
	map<int, TileType> tile_types_list;

public:
	TileTypesManager();

	void createTileType(int tile_type, string tile_type_name, Texture& tile_types_texture, bool has_collision);
	TileType& getTileType(const int& tile_type) { return tile_types_list[tile_type]; }
	int getTileTypesAmount() { return tile_types_list.size(); }
};

class Tree
{
private:
	int tree_x;
	int tree_y;
	int tree_height;
	int branches_amount;
	Vector2f tree_top_sprite_position;
	vector<Vector2f> tree_branches_sprite_position;
	Texture tree_top_texture, tree_branches_texture;
	Sprite tree_top_sprite;
	vector<Sprite> tree_branches_sprite;

public:
	Tree();

	friend class TreesManager;
};

class TreesManager
{
private:
	map<int, Tree> trees_list;

	void checkTreesCollisions(GameField& game_field, bool& is_check_ok, Tree& new_tree, int& tree_x, int& grass_layer_y);

public:
	TreesManager();

	void createTree(GameField& game_field, int& tree, int& tree_x, int& tree_height);
	void updateTrees(RenderWindow& game_window, int& offset_x, int& offset_y);
	void chopTree(GameField& game_field, int& mouse_x, int& mouse_y);
	void saveTrees(ofstream& write_to_file);
	void loadTrees(ifstream& read_from_file, vector<int>& buffer, int& buffer_counter);
};

class Item : public PhysicalObject
{
private:
	int item_type;
	Texture item_texture;
	Sprite item_sprite;

	void moveItem(GameField& game_field, float& dt);

public:
	Item();

	int& getItemType() { return item_type; }
	FloatRect& getItemPhysicalBox() { return physical_box; }

	friend class ItemsManager;
};

class ItemsManager
{
private:
	vector<Item> items_list;

public:
	ItemsManager();

	void createItem(const int& tile_type, int& tile_size, int& item_create_x, const int& item_create_y);
	void updateItems(RenderWindow& game_window, float& dt, GameField& game_field, int& offset_x, int& offset_y);
	Item& getItem(int& item_id) { return items_list[item_id]; }
	int getItemsAmount() { return items_list.size(); }
	void deleteItem(int& item_id) { items_list.erase(items_list.begin() + item_id); }
};

class GameField
{
private:
	int game_field_width;
	int game_field_height;
	int tile_size;
	int mouse_x;
	int mouse_y;
	vector<vector<int>> game_field_array, back_game_field_array;
	TileTypesManager game_field_tiles, back_game_field_tiles;
	TreesManager game_field_trees;
	ItemsManager game_field_items;
	random_device random_device;
	Texture background_texture, tile_types_texture, back_tile_types_texture;
	Sprite background_sprite;
	SoundBuffer place_tile_sound_buffer_1, place_tile_sound_buffer_2, place_tile_sound_buffer_3,
				mine_dirt_tile_sound_buffer_1, mine_dirt_tile_sound_buffer_2, mine_dirt_tile_sound_buffer_3,
				mine_stone_tile_sound_buffer_1, mine_stone_tile_sound_buffer_2, mine_stone_tile_sound_buffer_3;
	Sound place_tile_sound, mine_tile_sound;

	void generateDirtBase(mt19937& random_generator);
	void generateWalls();
	void generateGrass();
	void checkOreClustersCollisions(int& ore_spawn_y, int& ore_spawn_x, int& ore_size_y, int& ore_size_x, bool& is_check_ok);
	void generateOres(mt19937& random_generator);
	void generateTrees(mt19937& random_generator);
	void generateGameField();
	void loadGameField();
	void setTileActionSound(string action_type);

public:
	GameField(bool new_or_load);

	void updateGameField(RenderWindow& game_window, Character& player, float& dt, int& game_window_width, int& game_window_height);
	void updateBackground(RenderWindow& game_window) { game_window.draw(background_sprite); }
	void mineTile(RenderWindow& game_window, Character& player, Interface& hotbar, int& game_window_width, int& game_window_height);
	void placeTile(RenderWindow& game_window, Character& player, Interface& hotbar, int& game_window_width, int& game_window_height);
	int& getGameFieldWidth() { return game_field_width; }
	int& getGameFieldHeight() { return game_field_height; }
	int& getTileSize() { return tile_size; }
	const int& getGameFieldTileType(int i, int j) { return game_field_array[i][j]; }
	int& setGameFieldTileType(int i, int j) { return game_field_array[i][j]; }
	const int& getBackGameFieldTileType(int i, int j) { return back_game_field_array[i][j]; }
	TileTypesManager& getGameFieldTilesList() { return game_field_tiles; }
	TreesManager& getGameFieldTreesList() { return game_field_trees; }
	ItemsManager& getGameFieldItemsList() { return game_field_items; }
};

class GameWindow
{
private:
	int game_window_width;
	int game_window_height;
	int fps_limit;
	float dt;
	int selected_menu_item;
	bool sound_played[4];
	Texture menu_background_texture, logo_texture, save_before_exit_texture;
	Sprite menu_background_sprite, logo_sprite, save_before_exit_sprite;
	Font menu_font;
	Text menu_item_text[4], about_text[5], save_before_exit_text[3];
	SoundBuffer menu_item_sound_buffer, save_before_exit_open_sound_buffer, save_before_exit_close_sound_buffer;
	Sound menu_item_sound, save_before_exit_open_sound, save_before_exit_close_sound;
	Music menu_music, game_music;

	void saveGame(GameField& game_field, Character& player, Interface& hotbar);
	void about(RenderWindow& game_window);
	bool saveAndExitToMenu(RenderWindow& game_window, GameField& game_field, Character& player, Interface& hotbar);
	void startGame(RenderWindow& game_window, bool new_or_load);

public:
	GameWindow(int& set_game_window_width, int& set_game_window_height);

	void mainMenu();
};