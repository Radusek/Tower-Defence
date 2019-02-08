#pragma once
#include "Movable.h"

#define M_PI 3.14159265359f

class Game;
class Tower;
class Minion;
class Animation;

enum ProjectileType
{
	EBow,
	EMissile,
	ETowerTypeCount
};

class Projectile : public Movable
{
private:
	static const int ProjectileAnimationFrames[ETowerTypeCount];

	static const float ProjectileRadius[ETowerTypeCount];

	static const float ProjectileSpeed[ETowerTypeCount];
	static const float ProjectileAcceleration[ETowerTypeCount];

	static const bool ProjectileGuided[ETowerTypeCount];



	Game* game;

	Minion*& target;
	int targetId;

	bool hit;

	bool guided; // straightly approaches the target (true) or moves in the same direction in each iteration (false)
	float angle;
	int type;

	int damage;
	int armorPenetration;

	Vector2f movementIteration;

	Sprite sprite;

	Animation* animation;

	friend class Game;
	friend class Minion;

public:
	// we we to zmieñ ziomuœ
	Projectile(Game* game0, Minion* target0, Tower* tower0);

	void setMovementIteration();

	void move();

	void checkCollisions();

	void destroyIfBeyondMap();

	void chase(Minion* newTarget);

	~Projectile();
};