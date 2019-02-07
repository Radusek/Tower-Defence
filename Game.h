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

	bool isPaused;

	int timeIndex;
	float baseTimeScale[3];
	float timeScale[3];

	int wave;
	int nMinionsSpawned;
	int dollars;
	int livesLeft;
	int tileSize;

	int minionId;

	bool mapScrolling[ControllersCount][DirectionCount];

	Vector2i selectedTile;
	int selectedStatus;

	Time time;
	Clock clock;
	Clock debugClock;

	Texture tileTexture;

	Texture towerTexture;
	std::vector<Tower*> towers;

	Texture upgradeTexture;

	Texture minionTexture;
	std::vector<Minion*> minions;

	Texture projectileTexture;
	std::vector<Projectile*> projectiles;

	std::vector<TextAnimation*> textAnimations;

	RectangleShape pauseRect;
	Text pauseText;


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
	friend class Animation;

public:
	Game(App& app0);

	void passEvent(Event event);

	void leftMouseClick(Vector2i pos);

	int checkCursorPosition(Vector2i pos);

	void updateLogic();

	void cameraMoving();

	void spawningObjects();

	void spendMoney(int type);

	void movingObjects();

	void staticCollision();

	bool isColliding(Minion* m1, Minion* m2);

	void loadGraphicsToWindow();

	void printInterface();

	void printButtons();

	void printTiles();

	void destroyingObjects();

	void manageTime();

	~Game();
};