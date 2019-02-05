#pragma once
#include <vector>
#include <SFML/Graphics.hpp>

using namespace sf;

class Game;


class Map
{
private:
	Vector2u size;

	bool** boolGrid;

	std::vector<Vector2f> path;

	friend class Editor;
	friend class Game;
	friend class Minion;
	friend class Projectile;

public:
	Map();

	void create(Vector2u size0);

	void load(int levelNumber);

	void free();

	~Map();
};