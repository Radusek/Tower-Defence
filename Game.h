#pragma once
#include <SFML/Graphics.hpp>
#include <cstdlib>
#include <time.h>
#include <vector>

#include "RenderingScene.h"
using namespace sf;

class Minion;
class Projectile;
class Tower;
class Map;
class App;

enum selected
{
	Path,
	FreeCell,
	OccupiedCell
};


class Game : public RenderingScene
{
private:

	int wave;
	int nMinionsSpawned;
	int dollars;
	int livesLeft;
	int tileSize;

	int minionId;

	bool mapScrolling[DirectionCount];

	Vector2i selectedTile;
	int selectedStatus;

	Clock clock;

	Texture towerTexture;
	std::vector<Tower*> towers;

	Texture minionTexture;
	std::vector<Minion*> minions;

	Texture projectileTexture;
	std::vector<Projectile*> projectiles;


	Texture tGameInterface;
	Sprite gameInterface;

	Map* map;

	View view;

	friend class Tower;
	friend class Minion;
	friend class Projectile;
	friend class App;

public:
	Game(App& app0);

	void passEvent(Event event);

	void leftMouseClick(Vector2i pos);

	int checkCursorPosition(Vector2i pos);

	void updateLogic();

	void cameraMoving();

	void spawningObjects();

	void movingObjects();

	void loadGraphicsToWindow();

	void destroyingObjects();

	~Game();
};