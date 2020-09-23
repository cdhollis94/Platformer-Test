#include "platformer.h"
#include <iostream>
#include <string>
#include <sstream>

//BASIC FUNCTIONS//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void initWindow(sf::RenderWindow** window, sf::VideoMode videoMode)
{
	videoMode.height = GameHeight;
	videoMode.width = GameWidth;
	*window = new sf::RenderWindow(videoMode, "Platformer", sf::Style::Titlebar | sf::Style::Close);
	(*window)->setFramerateLimit(60);
}

bool running(sf::RenderWindow* window)
{
	return window->isOpen();
}

void update(sf::RenderWindow* window, sf::Event ev, GameEntities& GameEntities, GameData& GameData, sf::Clock gameClock, Tile tileArray[MaxLevelHeight][MaxLevelLength], sf::Text& centerText)
{
	checkDirections(window, GameEntities.playerCharacter, GameData, tileArray);
	pollEvents(window, ev, GameEntities, GameData, gameClock, tileArray);
	gravity(window, GameEntities, GameData, gameClock, tileArray);
}

void pollEvents(sf::RenderWindow* window, sf::Event ev, GameEntities& GameEntities, GameData& GameData, sf::Clock gameClock, Tile tileArray[MaxLevelHeight][MaxLevelLength])
{
	while (window->pollEvent(ev)) {
		switch (ev.type) {
		case sf::Event::Closed:
			window->close();
			break;
		case sf::Event::KeyReleased:
			if (ev.key.code == sf::Keyboard::A && GameData.levelCompleted == false) {
				GameData.walkingAnimationcCounter = 0;
			}
			if (ev.key.code == sf::Keyboard::D && GameData.levelCompleted == false) {
				GameData.walkingAnimationcCounter = 0;
			}
			if (ev.key.code == sf::Keyboard::L && GameData.levelCompleted == false) {
				GameData.isRunning = false;
			}
			if (ev.key.code == sf::Keyboard::K && GameData.levelCompleted == false) {
				if (GameData.isAscending == true) {
					GameData.jumpAbandoned = true;
					GameData.isAscending = false;
					GameData.jumpAcceleration = 0;
				}
				GameData.releasedJumpKey = true;
			}
			break;
		}
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::D) && sf::Keyboard::isKeyPressed(sf::Keyboard::A) && GameData.levelCompleted == false) {
		;
	}
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::D) && GameData.levelCompleted == false) {
		moveRight(window, GameEntities, GameData, gameClock, tileArray);
	}
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::A) && GameData.levelCompleted == false) {
		moveLeft(GameEntities, GameData, gameClock);
	}
	else {
		GameEntities.playerCharacter.setTextureRect(sf::IntRect(36, 0, 18, 26));
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::L) && GameData.levelCompleted == false) {
		GameData.isRunning = true;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::K) && GameData.levelCompleted == false) {
		jump(window, GameEntities, GameData, gameClock);
	}
}

void render(sf::RenderWindow* window, GameEntities& GameEntities, GameData& GameData, Tile tileArray[MaxLevelHeight][MaxLevelLength], sf::Text& centerText)
{
	window->clear(GameData.levelColor);
	for (int i = 0; i < MaxLevelHeight; i++) {
		for (int j = 0; j < MaxLevelLength; j++) {
			window->draw(tileArray[i][j].shape);
		}
	}
	window->draw(GameEntities.playerCharacter);
	if (GameData.isPlayerDead == false) {
		centerText.setFillColor(sf::Color::Transparent);
	}
	window->draw(centerText);
	window->display();
}

std::string int_to_string(int num)
{
	std::stringstream type;
	type << num;
	return type.str();
}

//MECHANICAL FUNCTIONS//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void checkDirections(sf::RenderWindow* window, sf::Sprite sprite, GameData& GameData, Tile tileArray[MaxLevelHeight][MaxLevelLength])
{
	int footIndex = static_cast<int>(sprite.getPosition().y + sprite.getGlobalBounds().height + 1 + TileSize * aboveScreenRows) / TileSize;
	int headindex = static_cast<int>(sprite.getPosition().y - 1 + TileSize * aboveScreenRows) / TileSize;
	int currentYIndex = footIndex - 1;
	float facingLeftCenterCheckpoint = sprite.getPosition().x + (sprite.getLocalBounds().width * GameData.spriteScale / 2);
	float facingLeftLeftCheckpoint = sprite.getPosition().x + (sprite.getLocalBounds().width * GameData.spriteScale / 3) * 2;
	float facingLeftRightCheckpoint = sprite.getPosition().x + (sprite.getLocalBounds().width * GameData.spriteScale / 3);
	float facingRightCenterCheckpoint = sprite.getPosition().x - (sprite.getLocalBounds().width * GameData.spriteScale / 2);
	float facingRightLeftCheckpoint = sprite.getPosition().x - (sprite.getLocalBounds().width * GameData.spriteScale / 3) * 2;
	float facingRightRightCheckpoint = sprite.getPosition().x - (sprite.getLocalBounds().width * GameData.spriteScale / 3);
	for (int i = 0; i < MaxLevelLength; i++) {
		if (GameData.facingRight == true) {
			if ((tileArray[footIndex][i].shape.getPosition().x <= facingRightCenterCheckpoint) && (facingRightCenterCheckpoint <= tileArray[footIndex][i].shape.getPosition().x + TileSize)) {
				//Checking Below
				if (((tileArray[footIndex][i].shape.getPosition().x <= facingRightLeftCheckpoint <= tileArray[footIndex][i].shape.getPosition().x + TileSize) || (tileArray[footIndex][i].shape.getPosition().x <= facingRightRightCheckpoint <= tileArray[footIndex][i].shape.getPosition().x + TileSize)) && (tileArray[footIndex][i].symbol != '-')) {
					GameData.belowObstructed = true;
				}
				else if (i - 1 >= 0) {
					if ((facingRightLeftCheckpoint <= tileArray[footIndex][i - 1].shape.getPosition().x + TileSize) && (tileArray[footIndex][i - 1].symbol != '-')) {
						GameData.belowObstructed = true;
					}
					else {
						GameData.belowObstructed = false;
					}
				}
				else if (i + 1 < MaxLevelLength) {
					if ((facingRightRightCheckpoint >= tileArray[footIndex][i + 1].shape.getPosition().x) && (tileArray[footIndex][i + 1].symbol != '-')) {
						GameData.belowObstructed = true;
					}
					else {
						GameData.belowObstructed = false;
					}
				}
				else {
					GameData.belowObstructed = false;
				}
				//Checking Above
				if (tileArray[headindex][i].symbol != '-' && GameData.isAscending == true) {
					GameData.aboveObstructed = true;
					GameData.jumpCounter = GameData.jumpAscendLength;
				}
				else {
					GameData.aboveObstructed = false;
				}
				//Checking Right
				int rightIndex = static_cast<int>(sprite.getPosition().x + 1 + GameData.scrollTally) / TileSize;
				if (tileArray[currentYIndex][rightIndex].symbol == 'c') {
					GameData.levelCompleted = true;
				}
				else if (tileArray[currentYIndex][rightIndex].symbol != '-' || (tileArray[currentYIndex - 1][rightIndex].symbol != '-' && GameData.onGround == false)) {
					GameData.rightObstructed = true;
				}
				else {
					GameData.rightObstructed = false;
				}
				//Checking Snap Point
				int snapCheckpointIndex = static_cast<int>(sprite.getPosition().y + (sprite.getGlobalBounds().height / 8) * 7 + TileSize * aboveScreenRows) / TileSize;
				if (tileArray[snapCheckpointIndex][rightIndex].symbol == 'g') {
					GameData.rightObstructed = true;
				}
			}
		}
		else if (GameData.facingRight == false) {
			if ((tileArray[footIndex][i].shape.getPosition().x <= facingLeftCenterCheckpoint) && (facingLeftCenterCheckpoint <= tileArray[footIndex][i].shape.getPosition().x + TileSize)) {
				//Checking Below
				if (((tileArray[footIndex][i].shape.getPosition().x <= facingLeftLeftCheckpoint <= tileArray[footIndex][i].shape.getPosition().x + TileSize) || (tileArray[footIndex][i].shape.getPosition().x <= facingLeftRightCheckpoint <= tileArray[footIndex][i].shape.getPosition().x + TileSize)) && (tileArray[footIndex][i].symbol != '-')) {
					GameData.belowObstructed = true;
				}
				else if (i - 1 >= 0) {
					if ((facingLeftLeftCheckpoint <= tileArray[footIndex][i - 1].shape.getPosition().x + TileSize) && (tileArray[footIndex][i - 1].symbol != '-')) {
						GameData.belowObstructed = true;
					}
					else {
						GameData.belowObstructed = false;
					}
				}
				else if (i + 1 < MaxLevelLength) {
					if ((facingLeftRightCheckpoint >= tileArray[footIndex][i + 1].shape.getPosition().x) && (tileArray[footIndex][i + 1].symbol != '-')) {
						GameData.belowObstructed = true;
					}
					else {
						GameData.belowObstructed = false;
					}
				}
				else {
					GameData.belowObstructed = false;
				}
				//Checking Above
				if (tileArray[headindex][i].symbol != '-' && GameData.isAscending == true) {
					GameData.aboveObstructed = true;
					GameData.jumpCounter = GameData.jumpAscendLength;
				}
				else {
					GameData.aboveObstructed = false;
				}
				//Checking Left
				int leftIndex = static_cast<int>(sprite.getPosition().x - 1 + GameData.scrollTally) / (TileSize);
				if (tileArray[currentYIndex][leftIndex].symbol != '-' || (tileArray[currentYIndex - 1][leftIndex].symbol != '-' && GameData.onGround == false)) {
					GameData.leftObstructed = true;
				}
				else {
					GameData.leftObstructed = false;
				}
				//Checking Snap Point
				int snapCheckpointIndex = static_cast<int>(sprite.getPosition().y + (sprite.getGlobalBounds().height / 8) * 7 + TileSize * aboveScreenRows) / TileSize;
				if (tileArray[snapCheckpointIndex][leftIndex].symbol == 'g') {
					GameData.leftObstructed = true;
				}
			}
		}
	}
}
	
void moveRight(sf::RenderWindow* window, GameEntities& GameEntities, GameData& GameData, sf::Clock gameClock, Tile tileArray[MaxLevelHeight][MaxLevelLength])
{
	//Flip Sprite
	if (GameData.facingRight == false) {
		GameEntities.playerCharacter.setScale(GameData.spriteScale * -1, GameData.spriteScale);
		GameEntities.playerCharacter.move(sf::Vector2f(GameEntities.playerCharacter.getLocalBounds().width * GameData.spriteScale, 0.f));
		GameData.facingRight = true;
	}
	//Snap to Scroll Point
	if (GameEntities.playerCharacter.getPosition().x >= ScrollPoint) {
		GameEntities.playerCharacter.setPosition(ScrollPoint, GameEntities.playerCharacter.getPosition().y);
	}
	//Speed Check
	if ((GameEntities.playerCharacter.getPosition().x <= ScrollPoint) && (GameData.rightObstructed == false)) {
		//Falling
		if ((GameData.onGround == false) && (GameData.deliberateJump == false)) {
			GameEntities.playerCharacter.setPosition(GameEntities.playerCharacter.getPosition().x + GameData.fallMoveSpeed, GameEntities.playerCharacter.getPosition().y);
		}
		//Walking
		else if (GameData.isRunning == false) {
			GameEntities.playerCharacter.setPosition(GameEntities.playerCharacter.getPosition().x + GameData.walkSpeed, GameEntities.playerCharacter.getPosition().y);
		}
		//Running
		else {
			GameEntities.playerCharacter.setPosition(GameEntities.playerCharacter.getPosition().x + GameData.runSpeed, GameEntities.playerCharacter.getPosition().y);
		}
	}
	//Cycle Sprite Animation
	if (GameData.isRunning == false) {
		if (GameData.walkingAnimationcCounter >= 20) {
			GameData.walkingAnimationcCounter = 0;
		}
		if (GameData.walkingAnimationcCounter < 10) {
			GameEntities.playerCharacter.setTextureRect(sf::IntRect(18, 0, 18, 26));
		}
		else {
			GameEntities.playerCharacter.setTextureRect(sf::IntRect(36, 0, 18, 26));
		}
	}
	if (GameData.isRunning == true) {
		if (GameData.walkingAnimationcCounter >= 12) {
			GameData.walkingAnimationcCounter = 0;
		}
		if (GameData.walkingAnimationcCounter < 6) {
			GameEntities.playerCharacter.setTextureRect(sf::IntRect(18, 0, 18, 26));
		}
		else {
			GameEntities.playerCharacter.setTextureRect(sf::IntRect(36, 0, 18, 26));
		}
	}
	//Scrolling
	if ((GameEntities.playerCharacter.getPosition().x >= ScrollPoint) && (GameData.rightObstructed == false)) {
		if (GameData.isRunning == false) {
			for (int i = 0; i < MaxLevelHeight; i++) {
				for (int j = 0; j < MaxLevelLength; j++) {
					tileArray[i][j].shape.setPosition(tileArray[i][j].shape.getPosition().x - GameData.walkSpeed, tileArray[i][j].shape.getPosition().y);
				}
			}
			GameData.scrollTally += GameData.walkSpeed;
		}
		else if (GameData.isRunning == true) {
			for (int i = 0; i < MaxLevelHeight; i++) {
				for (int j = 0; j < MaxLevelLength; j++) {
					tileArray[i][j].shape.setPosition(tileArray[i][j].shape.getPosition().x - GameData.runSpeed, tileArray[i][j].shape.getPosition().y);
				}
			}
			GameData.scrollTally += GameData.runSpeed;
		}
	}
	GameData.walkingAnimationcCounter++;
}

void moveLeft(GameEntities& GameEntities, GameData& GameData, sf::Clock gameClock)
{
	//Flip Sprite
	if (GameData.facingRight == true) {
		GameEntities.playerCharacter.setScale(GameData.spriteScale, GameData.spriteScale);
		GameEntities.playerCharacter.move(sf::Vector2f(GameEntities.playerCharacter.getLocalBounds().width * (GameData.spriteScale * -1), 0.f));
		GameData.facingRight = false;
	}
	//Speed Check
	if ((GameEntities.playerCharacter.getPosition().x >= 0) && (GameData.leftObstructed == false)) {
		//Falling
		if ((GameData.onGround == false) && (GameData.deliberateJump == false)) {
			GameEntities.playerCharacter.setPosition(GameEntities.playerCharacter.getPosition().x - GameData.fallMoveSpeed, GameEntities.playerCharacter.getPosition().y);
		}
		//Walking
		else if (GameData.isRunning == false) {
			GameEntities.playerCharacter.setPosition(GameEntities.playerCharacter.getPosition().x - GameData.walkSpeed, GameEntities.playerCharacter.getPosition().y);
		}
		//Running
		else {
			GameEntities.playerCharacter.setPosition(GameEntities.playerCharacter.getPosition().x - GameData.runSpeed, GameEntities.playerCharacter.getPosition().y);
		}
	}
	//Cycle Sprite Animation
	if (GameEntities.playerCharacter.getPosition().x >= 0) {
		if (GameData.isRunning == false) {
			if (GameData.walkingAnimationcCounter >= 20) {
				GameData.walkingAnimationcCounter = 0;
			}
			if (GameData.walkingAnimationcCounter < 10) {
				GameEntities.playerCharacter.setTextureRect(sf::IntRect(18, 0, 18, 26));
			}
			else {
				GameEntities.playerCharacter.setTextureRect(sf::IntRect(36, 0, 18, 26));
			}
		}
		if (GameData.isRunning == true) {
			if (GameData.walkingAnimationcCounter >= 12) {
				GameData.walkingAnimationcCounter = 0;
			}
			if (GameData.walkingAnimationcCounter < 6) {
				GameEntities.playerCharacter.setTextureRect(sf::IntRect(18, 0, 18, 26));
			}
			else {
				GameEntities.playerCharacter.setTextureRect(sf::IntRect(36, 0, 18, 26));
			}
		}
	}
	else {
		GameEntities.playerCharacter.setTextureRect(sf::IntRect(36, 0, 18, 26));
	}
	GameData.walkingAnimationcCounter++;
}

void jump(sf::RenderWindow* window, GameEntities& GameEntities, GameData& GameData, sf::Clock gameClock)
{
	if (GameData.jumpTimeout == false) {
		if (GameData.jumpAbandoned == false) {
			GameData.deliberateJump = true;
			GameData.isAscending = true;
			GameData.onGround = false;
			if (GameData.jumpCounter < GameData.jumpAscendLength) {
				GameEntities.playerCharacter.setPosition(GameEntities.playerCharacter.getPosition().x, GameEntities.playerCharacter.getPosition().y - (GameData.jumpInitialSpeed - GameData.jumpAcceleration));
			}
			else {
				GameData.jumpTimeout = true;
				GameData.isAscending = false;
				GameData.jumpAcceleration = 0;
			}
		}
	}
	if (GameData.isAscending == true) {
		GameData.jumpCounter++;
		GameData.jumpAcceleration += GameData.jumpSpeedDecay;
	}
	GameData.releasedJumpKey = false;
}

void gravity(sf::RenderWindow* window, GameEntities& GameEntities, GameData& GameData, sf::Clock gameClock, Tile tileArray[MaxLevelHeight][MaxLevelLength])
{
	//Prevents jumps from looping
	if ((shouldFall(window, GameEntities.playerCharacter, GameData, tileArray) == false) && (GameData.releasedJumpKey == true)) {
		GameData.isAscending = false;
		GameData.jumpAbandoned = false;
		GameData.jumpTimeout = false;
		GameData.deliberateJump = false;
	}
	//General falling
	bool firstLanding = false;
	if (shouldFall(window, GameEntities.playerCharacter, GameData, tileArray) == false) {
		if ((GameData.onGround == false) && (GameData.isAscending == false)) {
			firstLanding = true;
		}
		GameData.onGround = true;
		GameData.jumpAcceleration = 0.f;
		GameData.jumpCounter = 0;
		if (firstLanding == true) {
			int footIndex = static_cast<int>(GameEntities.playerCharacter.getPosition().y + GameEntities.playerCharacter.getGlobalBounds().height + 1 + TileSize * aboveScreenRows) / TileSize;
			float y_pos = tileArray[footIndex][0].shape.getPosition().y - GameEntities.playerCharacter.getLocalBounds().height * GameData.spriteScale;
			GameEntities.playerCharacter.setPosition(GameEntities.playerCharacter.getPosition().x, y_pos);
		}
	}
	else {
		GameData.onGround = false;
	}
	if (GameData.isAscending == false) {
		if (GameData.onGround == false) {
			GameEntities.playerCharacter.setPosition(GameEntities.playerCharacter.getPosition().x, GameEntities.playerCharacter.getPosition().y + (0.f + GameData.jumpAcceleration));
			if (GameData.jumpAcceleration < GameData.gravityMaxSpeed) {
				GameData.jumpAcceleration += GameData.gravityAcceleration;
			}
		}
	}
	//Change sprite
	if (GameData.onGround == false) {
		GameEntities.playerCharacter.setTextureRect(sf::IntRect(0, 0, 18, 26));
	}
	//Death check
	if (GameEntities.playerCharacter.getPosition().y > (MaxLevelHeight - aboveScreenRows - 2) * TileSize) {
		GameData.onGround = true;
		GameData.isPlayerDead = true;
	}
}

bool shouldFall(sf::RenderWindow* window, sf::Sprite sprite, GameData& GameData, Tile tileArray[MaxLevelHeight][MaxLevelLength])
{
	if (GameData.belowObstructed == true) {
		return false;
	}
	else {
		return true;
	}
}

//LEVEL FUNCTIONS///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void completeLevel(sf::RenderWindow* window, sf::Text& centerText, GameData& GameData, std::string textContent)
{
	sf::sleep(sf::seconds(3 / 2));
	centerText.setFillColor(sf::Color::White);
	centerText.setString(textContent);
	centerText.setPosition(window->getSize().x / 2 - centerText.getGlobalBounds().width / 2, window->getSize().y / 2 - centerText.getGlobalBounds().height);
	window->draw(centerText);
	window->display();
	sf::sleep(sf::seconds(3));
	window->clear(sf::Color::Black);
	window->display();
	sf::sleep(sf::seconds(2));
	GameData.scrollTally = 0;
	centerText.setFillColor(sf::Color::Transparent);
}

void completeGame(sf::RenderWindow* window, sf::Text& centerText, GameData& GameData, std::string textContent)
{
	sf::sleep(sf::seconds(3 / 2));
	window->clear(sf::Color::Black);
	centerText.setFillColor(sf::Color::White);
	centerText.setString(textContent);
	window->draw(centerText);
	window->display();
}

void handleDeath(sf::RenderWindow* window, sf::Text& centerText, GameData& GameData)
{
	GameData.lifeCounter -= 1;
	if (GameData.lifeCounter > 1) {
		sf::sleep(sf::seconds(3 / 2));
		window->clear(sf::Color::Black);
		centerText.setFillColor(sf::Color::White);
		centerText.setString(int_to_string(GameData.lifeCounter) + " lives remaining");
		centerText.setPosition(window->getSize().x / 2 - centerText.getGlobalBounds().width / 2, window->getSize().y / 2 - centerText.getGlobalBounds().height);
		window->draw(centerText);
		window->display();
		sf::sleep(sf::seconds(3));
		GameData.isPlayerDead = false;
	}
	else if (GameData.lifeCounter == 1) {
		sf::sleep(sf::seconds(3 / 2));
		window->clear(sf::Color::Black);
		centerText.setFillColor(sf::Color::White);
		centerText.setString(int_to_string(GameData.lifeCounter) + " life remaining");
		centerText.setPosition(window->getSize().x / 2 - centerText.getGlobalBounds().width / 2, window->getSize().y / 2 - centerText.getGlobalBounds().height);
		window->draw(centerText);
		window->display();
		sf::sleep(sf::seconds(3));
		GameData.isPlayerDead = false;
	}
	else {
		sf::sleep(sf::seconds(3 / 2));
		window->clear(sf::Color::Black);
		centerText.setFillColor(sf::Color::White);
		centerText.setString("Game Over");
		centerText.setPosition(window->getSize().x / 2 - centerText.getGlobalBounds().width / 2, window->getSize().y / 2 - centerText.getGlobalBounds().height);
		window->draw(centerText);
		window->display();
	}
	GameData.scrollTally = 0;
}

void loadLevel(sf::RenderWindow* window, char levelArray[MaxLevelHeight][MaxLevelLength], Tile tileArray[MaxLevelHeight][MaxLevelLength], GameTiles Tiles, GameEntities& GameEntities, GameData& GameData)
{
	float arrayLength = sizeof(levelArray[0]) / sizeof(levelArray[0][0]);
	for (int i = 0; i < MaxLevelHeight; i++) {
		for (int j = 0; j < arrayLength; j++) {
			if (levelArray[i][j] == '\0' || levelArray[i][j] == '-') {
				tileArray[i][j] = Tiles.air;
				tileArray[i][j].shape.setPosition(TileSize * j, TileSize * i - TileSize * aboveScreenRows);
				continue;
			}
			else if (levelArray[i][j] == 'g') {
				tileArray[i][j] = Tiles.ground;
				tileArray[i][j].shape.setPosition(TileSize * j, TileSize * i - TileSize * aboveScreenRows);
				continue;
			}
			else if (levelArray[i][j] == 'c') {
				tileArray[i][j] = Tiles.complete;
				tileArray[i][j].shape.setPosition(TileSize * j, TileSize * i - TileSize * aboveScreenRows);
				continue;
			}
		}
	}
	GameEntities.playerCharacter.setPosition(TileSize * 2, window->getSize().y - TileSize * 2 - GameEntities.playerCharacter.getLocalBounds().height * GameData.spriteScale - 1);
	GameData.levelCompleted = false;
}

void overwrite2dArray(char array1[MaxLevelHeight][MaxLevelLength], char array2[MaxLevelHeight][MaxLevelLength])
{
	for (int i = 0; i < MaxLevelHeight; i++) {
		for (int j = 0; j < MaxLevelLength; j++) {
			array1[i][j] = array2[i][j];
		}
	}
}

void initLevel1(char levelArray[][MaxLevelLength], GameData& GameData)
{
	GameData.levelColor = sf::Color(170, 170, 255);
	char tempLevelArray[MaxLevelHeight][MaxLevelLength] = {
		{'-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', 'c', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-'},
		{'-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', 'c', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-'},
		{'-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', 'c', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-'},
		{'-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', 'c', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-'},
		{'-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', 'c', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-'},
		{'-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', 'c', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-'},
		{'-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', 'c', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-'},
		{'-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', 'c', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-'},
		{'-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', 'c', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-'},
		{'-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', 'c', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-'},
		{'-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', 'c', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', 'g'},
		{'-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', 'c', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', 'g', 'g'},
		{'-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', 'c', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', 'g', 'g'},
		{'-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', 'c', '-', '-', '-', '-', '-', '-', '-', '-', '-', 'g', 'g', 'g'},
		{'-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', 'c', '-', '-', '-', '-', '-', '-', '-', '-', '-', 'g', 'g', 'g'},
		{'-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', 'c', '-', '-', '-', '-', '-', '-', '-', '-', 'g', 'g', 'g', 'g'},
		{'-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', 'g', 'g', 'g', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', 'c', '-', '-', '-', '-', '-', '-', '-', 'g', 'g', 'g', 'g', 'g'},
		{'-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', 'g', 'g', 'g', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', 'c', '-', '-', '-', '-', '-', '-', '-', 'g', 'g', 'g', 'g', 'g'},
		{'-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', 'g', 'g', 'g', 'g', 'g', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', 'c', '-', '-', '-', '-', '-', '-', 'g', 'g', 'g', 'g', 'g', 'g'},
		{'-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', 'g', 'g', '-', '-', 'g', 'g', 'g', 'g', 'g', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', 'g', 'g', 'g', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', 'c', '-', '-', '-', '-', '-', 'g', 'g', 'g', 'g', 'g', 'g', 'g'},
		{'-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', 'g', 'g', '-', '-', '-', '-', '-', '-', 'g', 'g', 'g', 'g', 'g', 'g', 'g', '-', '-', '-', 'g', 'g', 'g', '-', '-', '-', 'g', 'g', 'g', '-', '-', '-', '-', 'g', 'g', 'g', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', 'c', '-', '-', '-', '-', '-', 'g', 'g', 'g', 'g', 'g', 'g', 'g'},
		{'-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', 'g', 'g', 'g', 'g', 'g', 'g', 'g', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', 'c', '-', '-', '-', '-', '-', 'g', 'g', 'g', 'g', 'g', 'g', 'g'},
		{'-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', 'g', 'g', '-', '-', '-', 'g', 'g', 'g', '-', '-', '-', '-', 'g', 'g', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', 'g', 'g', 'g', 'g', 'g', 'g', 'g', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', 'c', '-', '-', '-', '-', '-', 'g', 'g', 'g', 'g', 'g', 'g', 'g'},
		{'-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', 'g', 'g', '-', '-', '-', '-', 'g', 'g', '-', '-', '-', 'g', 'g', 'g', '-', '-', '-', '-', 'g', 'g', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', 'g', 'g', 'g', 'g', 'g', 'g', 'g', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', 'c', '-', '-', '-', '-', '-', '-', '-', 'g', 'g', 'g', 'g', 'g'},
		{'-', '-', '-', '-', '-', '-', '-', 'g', 'g', '-', '-', '-', '-', 'g', 'g', '-', '-', '-', '-', 'g', 'g', '-', '-', '-', 'g', 'g', 'g', '-', '-', '-', '-', 'g', 'g', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', 'g', 'g', 'g', 'g', 'g', 'g', 'g', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', 'c', '-', '-', '-', '-', '-', '-', '-', '-', '-', 'g', 'g', 'g'},
		{'-', '-', '-', '-', '-', '-', '-', 'g', 'g', '-', '-', '-', '-', 'g', 'g', '-', '-', '-', '-', 'g', 'g', '-', '-', '-', 'g', 'g', 'g', '-', '-', '-', '-', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', '-', '-', 'g', 'g', 'g', 'g', 'g', 'g', 'g', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', 'c', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', 'g'},
		{'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', '-', '-', '-', 'g', 'g', 'g', '-', '-', '-', '-', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', '-', '-', 'g', 'g', 'g', 'g', 'g', 'g', 'g', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', 'g', 'g', 'g', '-', '-', '-', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', '-', '-', '-', '-', '-', '-', '-', '-'},
		{'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', '-', '-', '-', 'g', 'g', 'g', '-', '-', '-', '-', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', '-', '-', 'g', 'g', 'g', 'g', 'g', 'g', 'g', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', '-', '-', '-', '-', '-', '-'},
		{'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', '-', '-', '-', 'g', 'g', 'g', '-', '-', '-', '-', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', '-', '-', 'g', 'g', 'g', 'g', 'g', 'g', 'g', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', '-', '-', '-', '-'},
		{'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', '-', '-', '-', 'g', 'g', 'g', '-', '-', '-', '-', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', '-', '-', 'g', 'g', 'g', 'g', 'g', 'g', 'g', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', '-', '-'}
	};
	overwrite2dArray(levelArray, tempLevelArray);
}

void initLevel2(char levelArray[][MaxLevelLength], GameData& GameData)
{
	GameData.levelColor = sf::Color(65, 5, 30);
	char tempLevelArray[MaxLevelHeight][MaxLevelLength] = {
		{'-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', 'c', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-'},
		{'-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', 'c', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-'},
		{'-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', 'c', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-'},
		{'-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', 'c', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-'},
		{'-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', 'c', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-'},
		{'-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', 'c', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-'},
		{'-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', 'c', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-'},
		{'-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', 'c', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-'},
		{'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g'},
		{'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g'},
		{'-', '-', 'g', 'g', 'g', '-', '-', 'g', 'g', 'g', 'g', '-', '-', '-', '-', '-', 'g', 'g', 'g', 'g', '-', '-', '-', '-', 'g', 'g', 'g', 'g', '-', '-', '-', '-', '-', '-', '-', 'g', 'g', 'g', 'g', 'g', '-', '-', '-', '-', 'g', 'g', 'g', 'g', 'g', 'g', 'g', '-', '-', '-', '-', '-', 'g', 'g', 'g', '-', '-', '-', '-', '-', '-', 'g', 'g', 'g', 'g', 'g', 'g', '-', '-', '-', '-', '-', 'g', 'g', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g'},
		{'-', '-', 'g', 'g', 'g', '-', '-', 'g', 'g', 'g', 'g', '-', '-', '-', '-', '-', 'g', 'g', 'g', 'g', '-', '-', '-', '-', 'g', 'g', 'g', 'g', '-', '-', '-', '-', '-', '-', '-', 'g', 'g', 'g', 'g', 'g', '-', '-', '-', '-', 'g', 'g', 'g', 'g', 'g', 'g', 'g', '-', '-', '-', '-', '-', 'g', 'g', 'g', '-', '-', '-', '-', '-', '-', '-', 'g', 'g', 'g', 'g', 'g', '-', '-', '-', '-', '-', 'g', 'g', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g'},
		{'-', '-', 'g', 'g', 'g', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', 'g', 'g', 'g', 'g', '-', '-', '-', '-', 'g', 'g', 'g', 'g', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', 'g', 'g', 'g', '-', '-', '-', '-', '-', '-', '-', '-', 'g', 'g', 'g', 'g', '-', '-', '-', '-', '-', 'g', 'g', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g'},
		{'-', '-', 'g', 'g', 'g', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', 'g', 'g', 'g', 'g', '-', '-', '-', '-', 'g', 'g', 'g', 'g', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', 'g', 'g', 'g', '-', '-', '-', '-', '-', 'g', 'g', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', '-'},
		{'-', '-', 'g', 'g', 'g', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', 'g', 'g', 'g', 'g', '-', '-', '-', '-', 'g', 'g', 'g', 'g', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', 'g', 'g', '-', '-', '-', '-', '-', 'g', 'g', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', '-', '-', '-'},
		{'-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', 'g', 'g', 'g', 'g', '-', '-', '-', '-', 'g', 'g', 'g', 'g', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', 'g', 'g', '-', '-', '-', '-', '-', 'g', 'g', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', '-', '-', '-', '-', '-'},
		{'-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', 'g', 'g', 'g', 'g', '-', '-', '-', '-', 'g', 'g', 'g', 'g', '-', '-', '-', '-', '-', 'g', 'g', 'g', '-', '-', '-', 'g', 'g', 'g', 'g', 'g', 'g', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', 'g', 'g', 'g', 'g', 'g', 'g', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', '-', '-', '-', '-', '-', '-', '-'},
		{'-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', 'g', 'g', 'g', 'g', '-', '-', '-', '-', 'g', 'g', 'g', 'g', '-', '-', '-', '-', '-', 'g', 'g', 'g', '-', '-', '-', 'g', 'g', 'g', 'g', 'g', 'g', '-', '-', '-', '-', 'g', 'g', 'g', 'g', 'g', '-', '-', '-', 'g', 'g', 'g', 'g', 'g', 'g', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', '-', '-', '-', '-', '-', '-', '-', '-', '-'},
		{'-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', 'g', 'g', 'g', '-', '-', '-', 'g', 'g', 'g', 'g', 'g', 'g', '-', '-', '-', '-', 'g', 'g', 'g', 'g', 'g', '-', '-', '-', 'g', 'g', 'g', 'g', 'g', 'g', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', '-', '-', '-', '-', '-', '-', '-', '-', '-', 'g', 'g'},
		{'-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', 'g', 'g', 'g', 'g', 'g', '-', '-', '-', 'g', 'g', 'g', 'g', 'g', 'g', '-', '-', '-', '-', 'g', 'g', 'g', 'g', 'g', '-', '-', '-', 'g', 'g', 'g', 'g', 'g', 'g', '-', '-', '-', '-', '-', '-', 'g', 'g', '-', '-', '-', '-', '-', 'g', 'g', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', 'g', 'g', 'g', '-', '-', '-', '-', '-', '-', 'c', '-', '-', '-', '-', '-', '-', '-', '-', 'g', 'g', 'g', 'g'},
		{'-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', 'g', 'g', 'g', 'g', 'g', '-', '-', '-', 'g', 'g', 'g', 'g', 'g', 'g', '-', '-', '-', '-', 'g', 'g', 'g', 'g', 'g', '-', '-', '-', 'g', 'g', 'g', 'g', 'g', 'g', '-', '-', '-', '-', '-', '-', 'g', 'g', '-', '-', '-', '-', '-', 'g', 'g', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', 'g', 'g', '-', '-', '-', '-', '-', '-', '-', 'c', '-', '-', '-', '-', '-', '-', 'g', 'g', 'g', 'g', 'g', 'g'},
		{'-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', 'g', 'g', 'g', 'g', 'g', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', 'g', 'g', 'g', 'g', 'g', '-', '-', '-', '-', '-', 'g', 'g', 'g', 'g', '-', '-', '-', '-', 'g', 'g', 'g', 'g', '-', '-', '-', '-', '-', 'g', 'g', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', 'c', '-', '-', '-', '-', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g'},
		{'-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', 'g', 'g', 'g', 'g', 'g', 'g', '-', '-', 'g', 'g', 'g', 'g', 'g', 'g', '-', '-', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', 'g', 'g', 'g', 'g', '-', '-', '-', '-', '-', 'g', 'g', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g'},
		{'-', '-', '-', '-', '-', '-', '-', '-', '-', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', '-', '-', 'g', 'g', 'g', 'g', 'g', 'g', '-', '-', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', 'g', 'g', 'g', 'g', 'g', 'g', '-', '-', '-', '-', '-', 'g', 'g', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g'},
		{'-', '-', '-', '-', '-', '-', '-', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', '-', '-', 'g', 'g', 'g', 'g', 'g', 'g', '-', '-', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', 'g', 'g', 'g', 'g', 'g', 'g', '-', '-', '-', '-', '-', 'g', 'g', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', 'g', 'g', '-', '-', '-', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g'},
		{'-', '-', '-', '-', '-', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', '-', '-', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', '-', '-', '-', '-', 'g', 'g', 'g', 'g', 'g', 'g', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', '-', '-', '-', '-', '-', 'g', 'g', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', 'g', 'g', '-', '-', '-', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g'},
		{'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', '-', '-', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', '-', '-', '-', '-', 'g', 'g', 'g', 'g', 'g', 'g', '-', '-', '-', '-', 'g', 'g', 'g', 'g', '-', '-', '-', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', '-', '-', '-', '-', '-', 'g', 'g', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', 'g', 'g', '-', '-', '-', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g'},
		{'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', '-', '-', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', '-', '-', '-', '-', 'g', 'g', 'g', 'g', 'g', 'g', '-', '-', '-', '-', 'g', 'g', 'g', 'g', '-', '-', '-', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', '-', '-', '-', '-', '-', 'g', 'g', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', 'g', 'g', '-', '-', '-', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g'},
		{'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', '-', '-', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', '-', '-', '-', '-', 'g', 'g', 'g', 'g', 'g', 'g', '-', '-', '-', '-', 'g', 'g', 'g', 'g', '-', '-', '-', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', '-', '-', '-', '-', '-', 'g', 'g', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', 'g', 'g', '-', '-', '-', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g'},
		{'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', '-', '-', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', '-', '-', '-', '-', 'g', 'g', 'g', 'g', 'g', 'g', '-', '-', '-', '-', 'g', 'g', 'g', 'g', '-', '-', '-', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', '-', '-', '-', '-', '-', 'g', 'g', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', 'g', 'g', '-', '-', '-', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g'},
	};
	overwrite2dArray(levelArray, tempLevelArray);
}

void initLevel3(char levelArray[][MaxLevelLength], GameData& GameData)
{
	GameData.levelColor = sf::Color(255, 110, 34);
	char tempLevelArray[MaxLevelHeight][MaxLevelLength] = {
		{'-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', 'c', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-'},
		{'-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', 'c', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-'},
		{'-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', 'c', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-'},
		{'-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', 'c', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-'},
		{'-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', 'c', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-'},
		{'-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', 'c', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-'},
		{'-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', 'c', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-'},
		{'-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', 'c', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-'},
		{'-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', 'c', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-'},
		{'-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', 'c', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-'},
		{'-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', 'g', 'g', 'g', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', 'c', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-'},
		{'-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', 'g', 'g', 'g', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', 'c', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-'},
		{'-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', 'g', 'g', 'g', 'g', 'g', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', 'c', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-'},
		{'g', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', 'g', 'g', 'g', 'g', '-', '-', '-', 'g', 'g', 'g', 'g', 'g', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', 'g', 'g', 'g', 'g', 'g', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', 'c', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-'},
		{'g', 'g', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', 'g', 'g', 'g', 'g', '-', '-', '-', 'g', 'g', 'g', 'g', 'g', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', 'g', 'g', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', 'c', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-'},
		{'g', 'g', 'g', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', 'g', 'g', 'g', 'g', 'g', 'g', '-', '-', '-', 'g', 'g', 'g', 'g', 'g', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', 'g', 'g', 'g', '-', '-', '-', '-', '-', '-', 'g', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', 'c', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-'},
		{'g', 'g', 'g', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', 'g', 'g', 'g', 'g', 'g', 'g', '-', '-', '-', 'g', 'g', 'g', 'g', 'g', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', 'g', 'g', 'g', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', 'c', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-'},
		{'g', 'g', 'g', 'g', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', '-', '-', '-', 'g', 'g', 'g', 'g', 'g', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', 'c', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-'},
		{'g', 'g', 'g', 'g', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', '-', '-', '-', '-', 'g', 'g', 'g', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', 'g', 'g', 'g', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', 'c', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-'},
		{'g', 'g', 'g', 'g', 'g', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', '-', '-', '-', '-', 'g', 'g', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', 'g', 'g', 'g', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', 'c', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-'},
		{'g', 'g', 'g', 'g', 'g', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', 'g', 'g', '-', '-', '-', '-', '-', '-', 'g', 'g', '-', '-', 'g', 'g', '-', '-', '-', '-', '-', 'g', 'g', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', 'c', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-'},
		{'g', 'g', 'g', 'g', 'g', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', 'g', 'g', '-', '-', '-', '-', '-', '-', 'g', '-', '-', '-', '-', 'g', '-', '-', '-', '-', '-', '-', 'g', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', 'g', 'g', 'g', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', 'c', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-'},
		{'-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', 'g', 'g', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', 'g', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', 'g', 'g', 'g', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', 'c', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-'},
		{'-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', 'g', 'g', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', 'c', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-'},
		{'-', '-', '-', '-', '-', '-', '-', '-', '-', 'g', 'g', '-', '-', '-', '-', '-', 'g', 'g', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', 'g', 'g', 'g', 'g', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', 'c', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-'},
		{'-', '-', '-', '-', '-', '-', '-', '-', '-', 'g', 'g', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', 'g', 'g', 'g', 'g', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', 'c', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-'},
		{'g', 'g', 'g', 'g', 'g', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', 'c', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-'},
		{'g', 'g', 'g', 'g', 'g', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', 'c', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-'},
		{'g', 'g', 'g', 'g', 'g', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', 'c', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-'},
		{'g', 'g', 'g', 'g', 'g', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', 'c', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-'},
	};
	overwrite2dArray(levelArray, tempLevelArray);
}






/*
blank level data
char tempLevelArray[MaxLevelHeight][MaxLevelLength] = {
		{'-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', 'c', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-'},
		{'-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', 'c', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-'},
		{'-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', 'c', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-'},
		{'-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', 'c', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-'},
		{'-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', 'c', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-'},
		{'-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', 'c', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-'},
		{'-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', 'c', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-'},
		{'-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', 'c', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-'},
		{'-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', 'c', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-'},
		{'-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', 'c', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-'},
		{'-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', 'c', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-'},
		{'-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', 'c', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-'},
		{'-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', 'c', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-'},
		{'-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', 'c', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-'},
		{'-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', 'c', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-'},
		{'-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', 'c', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-'},
		{'-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', 'c', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-'},
		{'-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', 'c', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-'},
		{'-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', 'c', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-'},
		{'-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', 'c', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-'},
		{'-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', 'c', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-'},
		{'-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', 'c', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-'},
		{'-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', 'c', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-'},
		{'-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', 'c', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-'},
		{'-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', 'c', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-'},
		{'-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', 'c', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-'},
		{'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g'},
		{'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g'},
		{'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g'},
		{'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g'},
	};
*/
