#include <iostream>
#include "platformer.h"

int main() {

	//SETUP/////////////////////////////////////////

	sf::VideoMode videoMode;
	sf::RenderWindow* window;
	sf::RenderWindow** windowPoint = &window;
	initWindow(windowPoint, videoMode);
	sf::Event ev = sf::Event();
	sf::Clock gameClock;

	GameData GameData;

	Textures Textures;
	if (!Textures.playerTexture.loadFromFile("Novice Spritesheet.png")) {
		std::cout << "Texture load error";
	}

	//Sprite setup
	GameEntities GameEntities;
	GameEntities.playerCharacter.setTexture(Textures.playerTexture);
	GameEntities.playerCharacter.setScale(-3.f, 3.f);
	GameEntities.playerCharacter.setTextureRect(sf::IntRect(36, 0, 18, 26));
	GameEntities.playerCharacter.setPosition(static_cast<float>(window->getSize().x)/4, window->getSize().y - GameEntities.playerCharacter.getGlobalBounds().height);

	//OPERATIONS BEGIN/////////////////////////////
	gameClock.restart();
	int timer = 0;
	while (running(window)) {
		update(window, ev, GameEntities, GameData, gameClock);
		render(window, GameEntities, GameData);
		sf::sleep(sf::seconds(1/60));
	}
	return 0;
}