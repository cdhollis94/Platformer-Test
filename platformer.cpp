#include "platformer.h"
#include <iostream>

//BASIC FUNCTIONS//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void initWindow(sf::RenderWindow** window, sf::VideoMode videoMode)
{
	videoMode.height = 600;
	videoMode.width = 800;
	*window = new sf::RenderWindow(videoMode, "Platformer", sf::Style::Titlebar | sf::Style::Close);
	(*window)->setFramerateLimit(60);
}

bool running(sf::RenderWindow* window)
{
	return window->isOpen();
}

void update(sf::RenderWindow* window, sf::Event ev, GameEntities& GameEntities, GameData& GameData, sf::Clock gameClock)
{
	pollEvents(window, ev, GameEntities, GameData, gameClock);
	gravity(window, GameEntities, GameData, gameClock);
}

void pollEvents(sf::RenderWindow* window, sf::Event ev, GameEntities& GameEntities, GameData& GameData, sf::Clock gameClock)
{
	while (window->pollEvent(ev)) {
		switch (ev.type) {
		case sf::Event::Closed:
			window->close();
			break;
		case sf::Event::KeyReleased:
			if (ev.key.code == sf::Keyboard::A) {
				GameData.walkingAnimationcCounter = 0;
			}
			if (ev.key.code == sf::Keyboard::D) {
				GameData.walkingAnimationcCounter = 0;
			}
			if (ev.key.code == sf::Keyboard::L) {
				GameData.isRunning = false;
			}
			if (ev.key.code == sf::Keyboard::K) {
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
	if ((sf::Keyboard::isKeyPressed(sf::Keyboard::D)) && (sf::Keyboard::isKeyPressed(sf::Keyboard::A))) {
		;
	}
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
		moveRight(window, GameEntities, GameData, gameClock);
	}
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
		moveLeft(GameEntities, GameData, gameClock);
	}
	else {
		GameEntities.playerCharacter.setTextureRect(sf::IntRect(36, 0, 18, 26));
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::L)) {
		GameData.isRunning = true;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::K)) {
		jump(window, GameEntities, GameData, gameClock);
	}
}

void render(sf::RenderWindow* window, GameEntities& GameEntities, GameData& GameData)
{
	window->clear(sf::Color(170, 170, 255));
	window->draw(GameEntities.playerCharacter);
	window->display();
}

//MECHANICAL FUNCTIONS//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void moveRight(sf::RenderWindow* window, GameEntities& GameEntities, GameData& GameData, sf::Clock gameClock)
{
	if (GameData.facingRight == false) {
		GameEntities.playerCharacter.setScale(-3.f, 3.f);
		GameEntities.playerCharacter.move(sf::Vector2f(54.f, 0.f));
		GameData.facingRight = true;
	}
	if (GameEntities.playerCharacter.getPosition().x <= static_cast<float>(window->getSize().x) / 12 * 5) {
		if (GameData.isRunning == false) {
			GameEntities.playerCharacter.setPosition(GameEntities.playerCharacter.getPosition().x + 5.f, GameEntities.playerCharacter.getPosition().y);
		}
		else {
			GameEntities.playerCharacter.setPosition(GameEntities.playerCharacter.getPosition().x + 8.f, GameEntities.playerCharacter.getPosition().y);
		}
	}
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
	GameData.walkingAnimationcCounter++;
}

void moveLeft(GameEntities& GameEntities, GameData& GameData, sf::Clock gameClock)
{
	if (GameData.facingRight == true) {
		GameEntities.playerCharacter.setScale(3.f, 3.f);
		GameEntities.playerCharacter.move(sf::Vector2f(-54.f, 0.f));
		GameData.facingRight = false;
	}
	if (GameEntities.playerCharacter.getPosition().x >= 0) {
		if (GameData.isRunning == false) {
			GameEntities.playerCharacter.setPosition(GameEntities.playerCharacter.getPosition().x - 5.f, GameEntities.playerCharacter.getPosition().y);
		}
		else {
			GameEntities.playerCharacter.setPosition(GameEntities.playerCharacter.getPosition().x - 8.f, GameEntities.playerCharacter.getPosition().y);
		}
	}
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

void gravity(sf::RenderWindow* window, GameEntities& GameEntities, GameData& GameData, sf::Clock gameClock)
{
	if (GameEntities.playerCharacter.getPosition().y + GameEntities.playerCharacter.getGlobalBounds().height == window->getSize().y) {
		GameData.onGround = true;
		GameData.jumpAcceleration = 0.f;
		GameData.jumpCounter = 0;
	}
	//Prevents jumps from looping
	if ((GameEntities.playerCharacter.getPosition().y + GameEntities.playerCharacter.getGlobalBounds().height == window->getSize().y) && (GameData.releasedJumpKey == true)) {
		GameData.isAscending = false;
		GameData.jumpAbandoned = false;
		GameData.jumpTimeout = false;
	}
	if (GameData.isAscending == false) {
		if (GameData.onGround == false) {
			GameEntities.playerCharacter.setPosition(GameEntities.playerCharacter.getPosition().x, GameEntities.playerCharacter.getPosition().y + (0.f + GameData.jumpAcceleration));
			if (GameData.jumpAcceleration < GameData.gravityMaxSpeed) {
				GameData.jumpAcceleration += GameData.gravityAcceleration;
			}
		}
	}
	//Snap to ground level
	if (GameEntities.playerCharacter.getPosition().y + GameEntities.playerCharacter.getGlobalBounds().height > window->getSize().y) {
		GameEntities.playerCharacter.setPosition(GameEntities.playerCharacter.getPosition().x, window->getSize().y - GameEntities.playerCharacter.getGlobalBounds().height);
	}
}
