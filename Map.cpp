#include "Map.h"
#include <fstream>

#include "Tower.h"


Map::Map(int levelNumber) : running(true)
{
	std::ifstream file("levels/level" + std::to_string(levelNumber) + ".txt");

	if (file.is_open())
	{
		unsigned int x, y;
		file >> x;
		file >> y;

		size = Vector2u(x, y);

		boolGrid = new bool*[size.y];

		for (unsigned int i = 0; i < size.y; i++)
		{
			boolGrid[i] = new bool[size.x];
		}

		for (unsigned int i = 0; i < size.y; i++)
		{
			for (unsigned int j = 0; j < size.x; j++)
			{
				file >>	boolGrid[i][j];
			}
		}

		int nListElements;
		file >> nListElements;

		for (int i = 0; i < nListElements; i++)
		{
			float x, y;
			file >> x;
			file >> y;

			path.push_back(Vector2f(x, y));
		}

		file.close();
	}
	
}

Map::~Map()
{
	for (unsigned int i = 0; i < size.y; i++) 
		delete[] boolGrid[i];

	delete[] boolGrid;
}