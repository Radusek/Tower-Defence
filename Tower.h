#pragma once
#include <SFML/Graphics.hpp>
using namespace sf;

class Game;

enum Upgrades
{
	Damage,
	ArmorPenetration,
	FireRate,
	FireRange,
	UpgradesCount
};

enum TowerType
{
	Bow,
	Missile,
	TowerTypeCount
};


class Tower
{
private:
	static const int TowerCost[TowerTypeCount];

	static const int TowerDamage[TowerTypeCount];
	static const int TowerArmorPenetration[TowerTypeCount];

	static const float TowerFireRate[TowerTypeCount];
	static const float TowerFireRange[TowerTypeCount];

	static const float TowerProjectileSpeed[TowerTypeCount];

	static const int TowerUpgradeCost[UpgradesCount];

	static const int TowerUpgradeLimit[TowerTypeCount][UpgradesCount];


	Game* game;

	bool sold;

	unsigned char upgrades[UpgradesCount];
	int type;

	int damage;
	int armorPenetration;

	float fireRate;
	float fireRange;

	Sprite sprite;

	Time timeFromLastShot;
	Clock clock;

	friend class Game;

public:
	Tower(Game* game0, int type0);

	void fire();

	void showRange();

	int getRefund();

	void sellTower();

	~Tower();
};