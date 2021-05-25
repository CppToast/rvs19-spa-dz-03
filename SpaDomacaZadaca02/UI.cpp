#include "UI.h"
#include "game_of_life.h"

UI::UI(sf::RenderWindow* _window, int _game_width, int _game_height) {
	window = _window;
	game = game_of_life(_game_height, _game_width);
	game.randomize();
	game_width = _game_width;
	game_height = _game_height;

	hud_offset_x = offset + game_width * (cell_size + border_size * 2) + hud_offset;
	hud_offset_y = offset + game_height * (cell_size + border_size * 2) + hud_offset;

	pkey_toggle = sf::Keyboard::isKeyPressed(KEY_TOGGLE);
	pkey_step   = sf::Keyboard::isKeyPressed(KEY_STEP);
	pkey_faster = sf::Keyboard::isKeyPressed(KEY_FASTER);
	pkey_slower = sf::Keyboard::isKeyPressed(KEY_SLOWER);
	pkey_clear  = sf::Keyboard::isKeyPressed(KEY_CLEAR);
	pkey_random = sf::Keyboard::isKeyPressed(KEY_RANDOM);
	pkey_fade   = sf::Keyboard::isKeyPressed(KEY_FADE);

	running = true;
	fading = true;

	update_time_index = update_time.size() - 1;
	last_frame = clock.getElapsedTime();

	cell_opacity.resize(game_height);
	for (size_t i = 0; i < cell_opacity.size(); i++) {
		cell_opacity[i].resize(game_width);
		for (size_t j = 0; j < cell_opacity[i].size(); j++) {
			cell_opacity[i][j] = 0;
		}
	}

	keys.loadFromFile("keys.png");
	font.loadFromFile("tahoma.ttf");
}

void UI::run() {
	sf::Time current_time = clock.getElapsedTime();

	if (running) {
		if (current_time.asSeconds() >= last_update.asSeconds() + update_time[update_time_index]) {
			game.sljedeca_generacija();
			last_update = current_time;
		}
	}
	
	if (pkey_toggle == false and sf::Keyboard::isKeyPressed(KEY_TOGGLE)) {
		running = !running;
	}

	if (pkey_step == false and sf::Keyboard::isKeyPressed(KEY_STEP)) {
		game.sljedeca_generacija();
		last_update = current_time;
	}

	if (pkey_faster == false and sf::Keyboard::isKeyPressed(KEY_FASTER)) {
		update_time_index--;
		if (update_time_index < 0) update_time_index = 0;
	}

	if (pkey_slower == false and sf::Keyboard::isKeyPressed(KEY_SLOWER)) {
		update_time_index++;
		if (update_time_index >= update_time.size()) update_time_index = update_time.size() - 1;
	}

	if (pkey_clear == false and sf::Keyboard::isKeyPressed(KEY_CLEAR)) {
		game = game_of_life(game_height, game_width);
	}

	if (pkey_random == false and sf::Keyboard::isKeyPressed(KEY_RANDOM)) {
		game.randomize();
	}

	if (pkey_fade == false and sf::Keyboard::isKeyPressed(KEY_FADE)) {
		fading = !fading;
	}

	if (sf::Mouse::isButtonPressed(MOUSE_ON) or sf::Mouse::isButtonPressed(MOUSE_OFF)) {
		sf::Vector2i position = sf::Mouse::getPosition(*window);
		int cell_x = (position.x - offset) / (cell_size + border_size * 2);
		int cell_y = (position.y - offset) / (cell_size + border_size * 2);
		if (cell_x >= 0 and cell_x < game_width and cell_y >= 0 and cell_y < game_height) {
			game.setCell(cell_y, cell_x, sf::Mouse::isButtonPressed(MOUSE_ON));
		}
	}

	pkey_toggle = sf::Keyboard::isKeyPressed(KEY_TOGGLE);
	pkey_step   = sf::Keyboard::isKeyPressed(KEY_STEP);
	pkey_faster = sf::Keyboard::isKeyPressed(KEY_FASTER);
	pkey_slower = sf::Keyboard::isKeyPressed(KEY_SLOWER);
	pkey_clear  = sf::Keyboard::isKeyPressed(KEY_CLEAR);
	pkey_random = sf::Keyboard::isKeyPressed(KEY_RANDOM);
	pkey_fade   = sf::Keyboard::isKeyPressed(KEY_FADE);

	sf::Vertex background[] = {
		sf::Vertex(sf::Vector2f(0,                   0),                   background_top),
		sf::Vertex(sf::Vector2f(window->getSize().x, 0),                   background_top),
		sf::Vertex(sf::Vector2f(window->getSize().x, window->getSize().y), background_bottom),
		sf::Vertex(sf::Vector2f(0,                   window->getSize().y), background_bottom)
	};
	window->draw(background, 4, sf::Quads);

	draw_grid(current_time);
	draw_hud();
	window->display();
	
	last_frame = current_time;
}

void UI::draw_grid(sf::Time current_time) {
	float time_delta = current_time.asSeconds() - last_frame.asSeconds();
	float fade_speed;
	sf::RectangleShape tile;
	for (size_t i = 0; i < game_height; i++) {
		for (size_t j = 0; j < game_width; j++) {
			tile.setPosition(sf::Vector2f(offset + j * (cell_size + border_size * 2), offset + i * (cell_size + border_size * 2)));
			tile.setSize(sf::Vector2f(cell_size, cell_size));
			tile.setOutlineThickness((float)border_size);

			if (fading) {
				if (update_time[update_time_index] != 0) {
					if (1 / update_time[update_time_index] < min_fade_speed) {
						fade_speed = min_fade_speed;
					}
					else if (1 / update_time[update_time_index] > max_fade_speed) {
						fade_speed = max_fade_speed;
					}
					else {
						fade_speed = 1 / update_time[update_time_index];
					}
				}
				else {
					fade_speed = max_fade_speed;
				}

				if (game.getCell(i, j) == true) {
					cell_opacity[i][j] += fade_speed * time_delta;
				}
				else {
					cell_opacity[i][j] -= fade_speed * time_delta;
				}

				if (cell_opacity[i][j] < 0) { cell_opacity[i][j] = 0; }
				if (cell_opacity[i][j] > 1) { cell_opacity[i][j] = 1; }
			}
			else {
				cell_opacity[i][j] = game.getCell(i, j);
			}

			tile.setFillColor(interpolate_colors(cell_off, cell_on, cell_opacity[i][j]));
			tile.setOutlineColor(interpolate_colors(outline_off, outline_on, cell_opacity[i][j]));

			window->draw(tile);
		}
	}
}

void UI::draw_hud() {
	// nacrtaj kontrole
	for (size_t i = 0; i < hud_item.size(); i++) {
		draw_item(hud_offset_x, i * (icon_size + icon_spacing) + icon_spacing + offset, hud_item[i]);
	}

	// nacrtaj status
	draw_item(offset, hud_offset_y, running ? playing : paused);

	// nacrtaj vrijeme
	HUD_Item time_appended = time;
	stringstream ss;
	ss << setprecision(8) << noshowpoint << update_time[update_time_index];
	time_appended.text = time_appended.text + ss.str() + " s";
	draw_item(offset, hud_offset_y + icon_size + icon_spacing, time_appended);

}

void UI::draw_item(int x, int y, HUD_Item item) {
	sf::Sprite item_icon;
	item_icon.setTexture(keys);

	sf::Text item_text;
	item_text.setFont(font);
	item_text.setCharacterSize(font_size);
	item_text.setFillColor(text_color);

	item_icon.setTextureRect(sf::IntRect(item.x, item.y, icon_size, icon_size));
	item_icon.setPosition(sf::Vector2f(x, y));
	window->draw(item_icon);
	item_text.setString(item.text);
	item_text.setPosition(sf::Vector2f(x + icon_size + icon_spacing, y + font_offset));
	window->draw(item_text);
}

sf::Color UI::interpolate_colors(sf::Color color1, sf::Color color2, float ratio) {
	return sf::Color(
		color1.r * (1 - ratio) + color2.r * ratio,
		color1.g * (1 - ratio) + color2.g * ratio,
		color1.b * (1 - ratio) + color2.b * ratio
	);
}
