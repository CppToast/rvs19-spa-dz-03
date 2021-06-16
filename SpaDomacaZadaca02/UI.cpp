#include "UI.h"
#include "Pathfinding.h"

UI::UI(sf::RenderWindow* _window, int _game_width, int _game_height) {
	window = _window;
	game = Pathfinding(_game_width, _game_height);
	game_width = _game_width;
	game_height = _game_height;

	hud_offset_x = offset + game_width * (cell_size + border_size * 2) + hud_offset;
	hud_offset_y = offset + game_height * (cell_size + border_size * 2) + hud_offset;

	pkey_play     = sf::Keyboard::isKeyPressed(KEY_PLAY);
	pkey_faster   = sf::Keyboard::isKeyPressed(KEY_FASTER);
	pkey_slower   = sf::Keyboard::isKeyPressed(KEY_SLOWER);
	pkey_clear    = sf::Keyboard::isKeyPressed(KEY_CLEAR);
	pkey_diagonal = sf::Keyboard::isKeyPressed(KEY_DIAGONAL);

	running = false;
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
		if (path_nodes.empty()) {
			running = false;
			current_node = nullptr;
		}
		else if (current_time.asSeconds() >= last_update.asSeconds() + update_time[update_time_index]) {
			current_node = path_nodes.top();
			path_nodes.pop();
			last_update = current_time;
		}
	}
	else {
		current_node = nullptr;
	}
	
	if (pkey_play == false and sf::Keyboard::isKeyPressed(KEY_PLAY)) {
		if (running) {
			running = false;
			current_node = nullptr;
		}
		else {
			running = true;
			path_nodes = game.getPath();
		}
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
		game.InitializeNodes();
		running = false;
	}

	if (pkey_diagonal == false and sf::Keyboard::isKeyPressed(KEY_DIAGONAL)) {
		game.toggleDiagonals();
		running = false;
	}

	if (
		sf::Mouse::isButtonPressed(MOUSE_ON) or 
		sf::Mouse::isButtonPressed(MOUSE_OFF) or
		sf::Keyboard::isKeyPressed(KEY_START) or
		sf::Keyboard::isKeyPressed(KEY_END)
	) {

		sf::Vector2i position = sf::Mouse::getPosition(*window);
		int cell_x = (position.x - offset) / (cell_size + border_size * 2);
		int cell_y = (position.y - offset) / (cell_size + border_size * 2);
		if (cell_x >= 0 and cell_x < game_width and cell_y >= 0 and cell_y < game_height) {
			if (sf::Mouse::isButtonPressed(MOUSE_ON )) {game.setObstacle (cell_x, cell_y, true);}
			if (sf::Mouse::isButtonPressed(MOUSE_OFF)) {game.setObstacle (cell_x, cell_y, false);}
			if (sf::Keyboard::isKeyPressed(KEY_START)) {game.setStartNode(cell_x, cell_y);}
			if (sf::Keyboard::isKeyPressed(KEY_END  )) {game.setEndNode  (cell_x, cell_y);}
		}
		running = false;
	}

	pkey_play     = sf::Keyboard::isKeyPressed(KEY_PLAY);
	pkey_faster   = sf::Keyboard::isKeyPressed(KEY_FASTER);
	pkey_slower   = sf::Keyboard::isKeyPressed(KEY_SLOWER);
	pkey_clear    = sf::Keyboard::isKeyPressed(KEY_CLEAR);
	pkey_diagonal = sf::Keyboard::isKeyPressed(KEY_DIAGONAL);

	game.Solve_AStar();

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

			// odredi boju plocice
			sf::Color active_color = cell_off;

			if (game.isVisited (j, i)) {active_color = visited;}
			if (game.isPath    (j, i)) {active_color = path;}
			if (game.isStart   (j, i)) {active_color = a_point;}
			if (game.isEnd     (j, i)) {active_color = b_point;}
			if (game.isObstacle(j, i)) {active_color = obstacle;}

			if (game.getNode(j, i) == current_node) { active_color = moving_point; }

			tile.setFillColor(active_color);
			tile.setOutlineColor(modulate_color(active_color));

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

sf::Color UI::modulate_color(sf::Color color1) {
	return sf::Color(
		color1.r * outline_color_multiplier,
		color1.g * outline_color_multiplier,
		color1.b * outline_color_multiplier
	);
}
