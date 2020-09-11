#pragma once

#include <iostream>
#include <SFML/System.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

//STRUCTS

struct GameEntities {
	sf::Sprite playerCharacter;
};

struct GameData {
	//Player Data--------------------
	bool facingRight = true;
	int walkingAnimationcCounter = 0;
	bool isRunning = false;
	bool jumpTimeout = false;
	bool isAscending = false;
	bool jumpAbandoned = false;
	bool releasedJumpKey = true;
	bool onGround = true;
	int jumpCounter = 0;
	float jumpAcceleration = 0;
	//Physics Alterations
	float jumpAscendLength = 20;
	float jumpInitialSpeed = 10;
	float jumpSpeedDecay = 0.5;
	float gravityAcceleration = 0.5;
	float gravityMaxSpeed = 10;
};

struct Textures {
	sf::Texture playerTexture;
};

//BASIC FUNCTIONS

void initWindow(sf::RenderWindow** windowPoint, sf::VideoMode videoMode);
bool running(sf::RenderWindow* window);
void update(sf::RenderWindow* window, sf::Event ev, GameEntities& GameEntities, GameData& GameData, sf::Clock gameClock);
void pollEvents(sf::RenderWindow* window, sf::Event ev, GameEntities& GameEntities, GameData& GameData, sf::Clock gameClock);
void render(sf::RenderWindow* window, GameEntities& GameEntities, GameData& GameData);

//MECHANICAL FUNCTIONS

void moveRight(sf::RenderWindow* window, GameEntities& GameEntities, GameData& GameData, sf::Clock gameClock);
void moveLeft(GameEntities& GameEntities, GameData& GameData, sf::Clock gameClock);
void jump(sf::RenderWindow* window, GameEntities& GameEntities, GameData& GameData, sf::Clock gameClock);
void gravity(sf::RenderWindow* window, GameEntities& GameEntities, GameData& GameData, sf::Clock gameClock);