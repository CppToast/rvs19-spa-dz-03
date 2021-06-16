#include <SFML/Graphics.hpp>
#include "UI.h"

int main() {

	sf::RenderWindow window(sf::VideoMode(800, 600), "", sf::Style::Titlebar | sf::Style::Close);
	//window.setVerticalSyncEnabled(false);
	UI ui(&window, 32, 28);

	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
		}

		ui.run();
		window.display();
	}

	return 0;
}