#pragma once
#include <vector>
#include <SFML/Graphics.hpp>

using namespace sf;

class Game;


class Map
{
private:
	bool running;

	Vector2u size;

	bool** boolGrid;

	std::vector<Vector2f> path;

	friend class Game;
	friend class Minion;
	friend class Projectile;

public:
	Map(int levelNumber);

	~Map();
};