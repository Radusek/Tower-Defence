#pragma once
#include "Movable.h"


#define M_PI 3.14159265359f

#define MINION_BASE_HP 15

class Game;
class Projectile;

enum MinionType
{
	Normal,
	MinionTypesCount
};

class Minion : public Movable
{
private:
	bool lives;
	bool gotToTheEnd;

	Game* game;
	int pathIndex;
	int hp;
	int armor;
	int type;

	float angle;

	int id;

	Sprite sprite;

	friend class Game;
	friend class Projectile;
	friend class Tower;


public:
	Minion(Game* game, int minionId);

	void move();

	void takeHit(Projectile* projectile);

	~Minion();
};

