#pragma once
#include "Movable.h"

#define M_PI 3.14159265359f

class Game;
class Minion;

class Projectile : public Movable
{
private:
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

	friend class Game;
	friend class Minion;

public:
	Projectile(Game* game0, Minion*& target0, int type0, int damage0, int armorPenetration0, float velocity0, Vector2f position);

	void move();

	void chase(Minion*& minion);

	~Projectile();
};