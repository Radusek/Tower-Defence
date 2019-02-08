#include "Map.h"
#include <fstream>

#include "Tower.h"


Map::Map() : size(0, 0)
{	
}

void Map::create(Vector2u size0)
{
	size = size0;

	boolGrid = new bool*[size.y];

	for (unsigned int i = 0; i < size.y; i++)
		boolGrid[i] = new bool[size.x];

	for (unsigned int i = 0; i < size.y; i++)
		for (unsigned int j = 0; j < size.x; j++)
			boolGrid[i][j] = 1;
	
	path.push_back(Vector2f(0.f, 0.f));
	path.push_back(Vector2f(5.f, 5.f));
}

void Map::load(int levelNumber)
{
	std::ifstream file("levels/level" + std::to_string(levelNumber) + ".txt");

	if (file.is_open())
	{
		free();

		file >> size.x;
		file >> size.y;

		boolGrid = new bool*[size.y];

		for (unsigned int i = 0; i < size.y; i++)	
			boolGrid[i] = new bool[size.x];


		for (unsigned int i = 0; i < size.y; i++)
			for (unsigned int j = 0; j < size.x; j++)
				file >> boolGrid[i][j];


		int nPathPoints;
		file >> nPathPoints;

		for (int i = 0; i < nPathPoints; i++)
		{
			float x, y;
			file >> x >> y;
			path.push_back(Vector2f(x, y));
		}

		file.close();
	}
}

void Map::free()
{
	if (size != Vector2u(0, 0))
	{
		for (unsigned int i = 0; i < size.y; i++)
			delete[] boolGrid[i];

		delete[] boolGrid;

		path.clear();

		size = Vector2u(0, 0);
	}
}

Map::~Map()
{
	free();
}