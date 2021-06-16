#pragma once
#include <SFML/Graphics.hpp>
#include <sstream>
#include <iomanip>
#include "Pathfinding.h"

// definicije kontrola
#define KEY_START    sf::Keyboard::Q
#define KEY_END      sf::Keyboard::W
#define KEY_PLAY     sf::Keyboard::Space
#define KEY_FASTER   sf::Keyboard::Left
#define KEY_SLOWER   sf::Keyboard::Right
#define KEY_CLEAR    sf::Keyboard::E
#define KEY_DIAGONAL sf::Keyboard::R
#define MOUSE_ON     sf::Mouse::Left
#define MOUSE_OFF    sf::Mouse::Right

struct HUD_Item {
	string text;
	int x, y;

	HUD_Item(int _x, int _y, string _text) {
		text = _text;
		x = _x * 32;
		y = _y * 32;
	}
};

class UI {
public:
	UI(sf::RenderWindow* window, int game_width, int game_height);
	void run();

private:
	sf::RenderWindow* window;
	Pathfinding game;
	int game_width;
	int game_height;
	sf::Clock clock;
	sf::Time last_update;
	sf::Time last_frame;
	int hud_offset_x;
	int hud_offset_y;

	stack<sNode*> path_nodes;
	sNode* current_node = nullptr;

	void draw_grid(sf::Time current_time);
	void draw_hud();
	void draw_item(int x, int y, HUD_Item item);

	sf::Color modulate_color(sf::Color color1);

	bool running;
	bool fading;
	int update_time_index;

	sf::Texture keys;
	sf::Font font;

	vector<vector<float>> cell_opacity;

	// prethodni input
	bool pkey_play;
	bool pkey_faster;
	bool pkey_slower;
	bool pkey_clear;
	bool pkey_diagonal;

	// konstante
	const int cell_size = 16;
	const int border_size = 1;
	const int offset = 16;
	const int hud_offset = 8;
	const int icon_size = 32;
	const int icon_spacing = 4;
	const int font_size = 10;
	const int font_offset = (icon_size - font_size) / 2;

	const sf::Color background_top    = sf::Color(64, 32, 0);
	const sf::Color background_bottom = sf::Color(128, 32, 0);
	const sf::Color cell_off          = sf::Color(64, 32, 0);
	const sf::Color path              = sf::Color(192, 128, 0);
	const sf::Color visited           = sf::Color(96, 64, 0);
	const sf::Color obstacle          = sf::Color(255, 255, 255);
	const sf::Color a_point           = sf::Color(255, 0, 128);
	const sf::Color b_point           = sf::Color(0, 255, 128);
	const sf::Color moving_point      = sf::Color(255, 255, 128);
	const sf::Color text_color        = sf::Color(255, 255, 255);

	const float outline_color_multiplier = 0.5;

	const float min_fade_speed = 6;
	const float max_fade_speed = 9;

	const vector<float> update_time = { 0, 0.1, 0.25, 0.5 };

	const vector<HUD_Item> hud_item = {
		HUD_Item(0, 0, "Place Start Point Under Cursor"),
		HUD_Item(1, 0, "Place End Point Under Cursor"),
		HUD_Item(2, 1, "Draw Obstacle"),
		HUD_Item(3, 1, "Erase Obstacle"),
		HUD_Item(2, 0, "Erase All Obstacles"),
		HUD_Item(3, 2, "Toggle Diagonal Movement"),
		HUD_Item(3, 0, "Play Animation"),
		HUD_Item(0, 1, "Decrease Step Time"),
		HUD_Item(1, 1, "Increase Step Time"),
	};
	const HUD_Item playing = HUD_Item(0, 2, "Playing");
	const HUD_Item paused  = HUD_Item(1, 2, "Paused");
	const HUD_Item time    = HUD_Item(2, 2, "Current Step Time: ");
};

