#pragma once
#include <vector>
#include <SFML/Graphics.hpp>

using namespace sf;

class Tower;

class Game;


class Map
{
private:
	bool running;

	Vector2u size;

	bool** boolGrid;
	Tower*** towerGrid;

	std::vector<Vector2f> path;

	friend class Game;
	friend class Minion;
	friend class Projectile;

public:
	Map(int levelNumber);

	~Map();
};

