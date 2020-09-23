#pragma once

#include <iostream>
#include <SFML/System.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

//CONSTANTS


const int MaxLevelHeight = 30;
const int MaxLevelLength = 110;
const int TileSize = 40;
const int GameHeight = TileSize * (MaxLevelHeight - 10);
const int GameWidth = (GameHeight / 3) * 4 - (TileSize * 5);
const int ScrollPoint = GameWidth / 12 * 5;
const int aboveScreenRows = 8;

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
	bool deliberateJump = true;
	bool onGround = true;
	int jumpCounter = 0;
	float jumpAcceleration = 0;
	float spriteScale = 3;
	bool isPlayerDead = false;
	//Physics Alterations
	float jumpAscendLength = 20;
	float jumpInitialSpeed = 10;
	float jumpSpeedDecay = 0.5;
	float gravityAcceleration = 0.5;
	float gravityMaxSpeed = 10;
	float walkSpeed = 4;
	float runSpeed = 7;
	float fallMoveSpeed = 3;
	int standingCheckPoints = 54 / 9;
	//Direction Checks
	bool leftObstructed = false;
	bool rightObstructed = false;
	bool belowObstructed = false;
	bool aboveObstructed = false;
	//Level Data---------------------
	sf::Color levelColor;
	float scrollTally = 0;
	bool levelCompleted = false;
	int currentLevelCounter = 1;
	int lifeCounter = 3;
};

struct Textures {
	sf::Texture playerTexture;
};

struct Tile {
	sf::RectangleShape shape;
	bool passThroughBottom;
	char symbol;
	Tile(sf::RectangleShape paramShape, sf::Color color, sf::Color outlineColor, int outlineThickness, bool paramPassThroughBottom, char paramSymbol) {
		shape = paramShape;
		shape.setSize(sf::Vector2f(TileSize, TileSize));
		shape.setFillColor(color);
		shape.setOutlineColor(outlineColor);
		shape.setOutlineThickness(outlineThickness);
		symbol = paramSymbol;
		passThroughBottom = paramPassThroughBottom;
	}
	Tile() {
		shape.setSize(sf::Vector2f(TileSize, TileSize));
		shape.setFillColor(sf::Color::Transparent);
		passThroughBottom = true;
		symbol = '-';
	}
	Tile(char paramSymbol) {
		shape.setSize(sf::Vector2f(TileSize, TileSize));
		shape.setFillColor(sf::Color::Transparent);
		passThroughBottom = true;
		symbol = paramSymbol;
	}
};

struct GameTiles {
	Tile ground = Tile(sf::RectangleShape(sf::Vector2f(TileSize, TileSize)), (sf::Color(168, 110, 18)), (sf::Color(198, 140, 28)), 3, false, 'g');
	Tile air;
	Tile complete = Tile('c');
};

//BASIC FUNCTIONS

void initWindow(sf::RenderWindow** windowPoint, sf::VideoMode videoMode);
bool running(sf::RenderWindow* window);
void update(sf::RenderWindow* window, sf::Event ev, GameEntities& GameEntities, GameData& GameData, sf::Clock gameClock, Tile tileArray[MaxLevelHeight][MaxLevelLength], sf::Text &centerText);
void pollEvents(sf::RenderWindow* window, sf::Event ev, GameEntities& GameEntities, GameData& GameData, sf::Clock gameClock, Tile tileArray[MaxLevelHeight][MaxLevelLength]);
void render(sf::RenderWindow* window, GameEntities& GameEntities, GameData& GameData, Tile tileArray[MaxLevelHeight][MaxLevelLength], sf::Text &centerText);
std::string int_to_string(int num);

//MECHANICAL FUNCTIONS

void checkDirections(sf::RenderWindow* window, sf::Sprite sprite, GameData& GameData, Tile tileArray[MaxLevelHeight][MaxLevelLength]);
void moveRight(sf::RenderWindow* window, GameEntities& GameEntities, GameData& GameData, sf::Clock gameClock, Tile tileArray[MaxLevelHeight][MaxLevelLength]);
void moveLeft(GameEntities& GameEntities, GameData& GameData, sf::Clock gameClock);
void jump(sf::RenderWindow* window, GameEntities& GameEntities, GameData& GameData, sf::Clock gameClock);
void gravity(sf::RenderWindow* window, GameEntities& GameEntities, GameData& GameData, sf::Clock gameClock, Tile tileArray[MaxLevelHeight][MaxLevelLength]);
bool shouldFall(sf::RenderWindow* window, sf::Sprite sprite, GameData& GameData, Tile tileArray[MaxLevelHeight][MaxLevelLength]);

//LEVEL FUNCTIONS

void completeLevel(sf::RenderWindow* window, sf::Text &centerText, GameData& GameData, std::string textContent);
void completeGame(sf::RenderWindow* window, sf::Text &centerText, GameData& GameData, std::string textContent);
void handleDeath(sf::RenderWindow* window, sf::Text& centerText, GameData& GameData);
void loadLevel(sf::RenderWindow* window, char levelArray[MaxLevelHeight][MaxLevelLength], Tile tileArray[MaxLevelHeight][MaxLevelLength], GameTiles GameTiles, GameEntities& GameEntities, GameData& GameData);
void overwrite2dArray(char array1[MaxLevelHeight][MaxLevelLength], char array2[MaxLevelHeight][MaxLevelLength]);
void initLevel1(char levelArray[][MaxLevelLength], GameData& GameData);
void initLevel2(char levelArray[][MaxLevelLength], GameData& GameData);
void initLevel3(char levelArray[][MaxLevelLength], GameData& GameData);