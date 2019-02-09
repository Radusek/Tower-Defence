#pragma once
#include <SFML/Graphics.hpp>
#include <cstdlib>
#include <time.h>
#include <vector>
#include "RenderingScene.h"

#define SELECTION_COLOR Color(0, 255, 0, 50)
#define REWARD_TEXT_COLOR Color(10, 100, 10, 255)
#define BUTTON_COLOR Color(200, 200, 20, 150)
#define SELL_BUTTON_COLOR Color(180, 0, 0, 255)
#define FULL_UPGRADE_COLOR Color(0, 255, 0, 80)
#define NO_MONEY_COLOR Color(0, 0, 0, 150)
#define INTERFACE_TEXT_COLOR Color(56, 56, 56, 255)
#define FAKE_TOWER_COLOR Color(128, 128, 128, 255)

#define REWARD_TEXT_SIZE 48
#define TIME_MODES 3
#define BUTTONS 4

#define TEXT_Y_OFFSET 0.1213f
#define VIEW_PART 0.8f
#define BUTTON_SIZE 0.1f
#define BUTTON_0_X 0.15f
#define BUTTON_N_X (BUTTON_0_X + BUTTONS * BUTTON_SIZE)
#define BUTTON_TOP 0.85f * BASE_HEIGHT
#define BUTTON_DOWN 0.95f * BASE_HEIGHT

#define SCREEN_EDGE 0.05f

#define SELL_BUTTON_X 0.05f * BASE_WIDTH
#define SELL_BUTTON_Y 0.85f * BASE_HEIGHT
#define SELL_SIZE 0.1f * BASE_HEIGHT

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
	float timeScale[TIME_MODES];

	int wave;
	int nMinionsSpawned;
	int spawnLimit;
	int dollars;
	int livesLeft;

	int minionId;

	bool mapScrolling[ControllersCount][DirectionCount];

	Vector2i selectedTile;
	int selectedStatus;

	Clock clock;
	Time frameTime;

	Time spawnTime;

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


	friend class Tower;
	friend class Minion;
	friend class Projectile;
	friend class App;
	friend class Animation;
	friend class TextAnimation;

public:
	Game(App& app0);

	void setView();

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

	Tower* getSelectedTower();

	~Game();
};