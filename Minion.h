#pragma once
#include "Movable.h"


#define M_PI 3.14159265359f

#define MINION_BASE_HP 12

class Game;
class Projectile;
class Animation;

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

	int id;
	int type;
	
	int hp;
	int armor;

	float angle;

	Sprite sprite;

	Animation* animation;

	friend class Game;
	friend class Projectile;
	friend class Tower;

public:
	Minion(Game* game);

	static int getWaveHp(int wave);
	
	static int getWaveArmor(int wave);

	void move();

	void takeHit(Projectile* projectile);

	void showHpBar();

	~Minion();
};