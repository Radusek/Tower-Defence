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
class TextAnimation;
class Map;
class App;

enum selected
{
	Path,
	FreeCell,
	OccupiedCell
};

enum controllers
{
	EKeyboard,
	EMouse,
	ControllersCount
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

	bool mapScrolling[ControllersCount][DirectionCount];

	Vector2i selectedTile;
	int selectedStatus;

	Clock clock;

	Texture towerTexture;
	std::vector<Tower*> towers;

	Texture minionTexture;
	std::vector<Minion*> minions;

	Texture projectileTexture;
	std::vector<Projectile*> projectiles;

	std::vector<TextAnimation*> textAnimations;


	Texture tGameInterface;
	Sprite gameInterface;

	Tower* fakeTower;
	bool displayFakeTower;

	float zoom;


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

	void placeTower(int type);

	void movingObjects();

	void loadGraphicsToWindow();

	void printInterface();

	void printButtons();

	void printTiles();

	void destroyingObjects();

	~Game();
};