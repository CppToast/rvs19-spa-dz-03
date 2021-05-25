#pragma once
#include <SFML/Graphics.hpp>
#include <sstream>
#include <iomanip>
#include "game_of_life.h"

// definicije kontrola
#define KEY_TOGGLE   sf::Keyboard::Q
#define KEY_STEP     sf::Keyboard::Space
#define KEY_FASTER   sf::Keyboard::Left
#define KEY_SLOWER   sf::Keyboard::Right
#define KEY_CLEAR    sf::Keyboard::W
#define KEY_RANDOM   sf::Keyboard::E
#define KEY_FADE     sf::Keyboard::R
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
	game_of_life game;
	int game_width;
	int game_height;
	sf::Clock clock;
	sf::Time last_update;
	sf::Time last_frame;
	int hud_offset_x;
	int hud_offset_y;

	void draw_grid(sf::Time current_time);
	void draw_hud();
	void draw_item(int x, int y, HUD_Item item);

	sf::Color interpolate_colors(sf::Color color1, sf::Color color2, float ratio);

	bool running;
	bool fading;
	int update_time_index;

	sf::Texture keys;
	sf::Font font;

	vector<vector<float>> cell_opacity;

	// prethodni input
	bool pkey_toggle;
	bool pkey_step;
	bool pkey_faster;
	bool pkey_slower;
	bool pkey_clear;
	bool pkey_random;
	bool pkey_fade;

	// konstante
	const int cell_size = 16;
	const int border_size = 1;
	const int offset = 16;
	const int hud_offset = 8;
	const int icon_size = 32;
	const int icon_spacing = 4;
	const int font_size = 16;
	const int font_offset = (icon_size - font_size) / 2 - 3;

	const sf::Color background_top    = sf::Color(0, 32, 64);
	const sf::Color background_bottom = sf::Color(0, 128, 128);
	const sf::Color outline_off       = sf::Color(0, 16, 32);
	const sf::Color outline_on        = sf::Color(0, 128, 128);
	const sf::Color cell_off          = sf::Color(0, 32, 64 );
	const sf::Color cell_on           = sf::Color(192, 255, 255);
	const sf::Color text_color        = sf::Color(255, 255, 255);

	const float min_fade_speed = 6;
	const float max_fade_speed = 9;

	const vector<float> update_time = { 0, 0.1, 0.25, 0.5, 1, 2 };

	const vector<HUD_Item> hud_item = {
		HUD_Item(0, 0, "Play/Pause"),
		HUD_Item(1, 0, "Clear Field"),
		HUD_Item(2, 0, "Randomize Field"),
		HUD_Item(3, 2, "Toggle Fading"),
		HUD_Item(3, 0, "Step"),
		HUD_Item(0, 1, "Decrease Step Time"),
		HUD_Item(1, 1, "Increase Step Time"),
		HUD_Item(2, 1, "Draw Living Cells"),
		HUD_Item(3, 1, "Draw Dead Cells"),
	};
	const HUD_Item playing = HUD_Item(0, 2, "Playing");
	const HUD_Item paused  = HUD_Item(1, 2, "Paused");
	const HUD_Item time    = HUD_Item(2, 2, "Current Step Time: ");
};

