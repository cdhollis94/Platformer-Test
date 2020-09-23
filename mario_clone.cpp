#include <iostream>
#include "platformer.h"
#include <string>

int main() {

	//SETUP//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	sf::VideoMode videoMode;
	sf::RenderWindow* window;
	sf::RenderWindow** windowPoint = &window;
	initWindow(windowPoint, videoMode);
	sf::Event ev = sf::Event();
	sf::Clock gameClock;

	//Text setup
	sf::Font font;
	if (!font.loadFromFile("cambria.ttc")) {
		std::cout << "FONT ERROR";
	}
	sf::Text centerText;
	centerText.setFont(font);
	centerText.setCharacterSize(48);
	centerText.setFillColor(sf::Color::Transparent);
	centerText.setPosition(window->getSize().x - centerText.getGlobalBounds().width/2, window->getSize().y - centerText.getGlobalBounds().height);

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
	GameEntities.playerCharacter.setPosition(TileSize * 2, window->getSize().y - TileSize * 2 - GameEntities.playerCharacter.getLocalBounds().height * GameData.spriteScale - 1);

	GameTiles GameTiles;
	Tile tileArray[MaxLevelHeight][MaxLevelLength];


	//OPERATIONS BEGIN///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	gameClock.restart();
	char currentLevel[MaxLevelHeight][MaxLevelLength];
	for (int i = 0; i < MaxLevelHeight; i++) {
		for (int j = 0; j < MaxLevelLength; j++) {
			currentLevel[i][j] = '0';
		}
	}

	initLevel1(currentLevel, GameData);
	loadLevel(window, currentLevel, tileArray, GameTiles, GameEntities, GameData);

	//Game Loop
	while (running(window)) {
		if (GameData.isPlayerDead == true) {
			handleDeath(window, centerText, GameData);
			loadLevel(window, currentLevel, tileArray, GameTiles, GameEntities, GameData);
		}
		else if (GameData.levelCompleted == true && GameData.currentLevelCounter == 1) {
			completeLevel(window, centerText, GameData, "Course 1 Completed!");
			initLevel2(currentLevel, GameData);
			loadLevel(window, currentLevel, tileArray, GameTiles, GameEntities, GameData);
			GameData.currentLevelCounter++;
		}
		else if (GameData.levelCompleted == true && GameData.currentLevelCounter == 2) {
			completeLevel(window, centerText, GameData, "Course 2 Completed!");
			initLevel3(currentLevel, GameData);
			loadLevel(window, currentLevel, tileArray, GameTiles, GameEntities, GameData);
			GameData.currentLevelCounter++;
		}
		else if (GameData.levelCompleted == true && GameData.currentLevelCounter >= 3) {
			completeGame(window, centerText, GameData, "Thanks for playing!");
		}
		else {
			update(window, ev, GameEntities, GameData, gameClock, tileArray, centerText);
			render(window, GameEntities, GameData, tileArray, centerText);
		}
		sf::sleep(sf::seconds(1 / 60));
	}
	return 0;
}