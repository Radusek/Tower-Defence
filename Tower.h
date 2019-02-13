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
	static const int TowerDamageIncrease[TowerTypeCount];


	static const int TowerArmorPenetration[TowerTypeCount];
	static const int TowerArmorPenetrationIncrease[TowerTypeCount];

	static const float TowerFireRate[TowerTypeCount];
	static const float TowerFireRateIncrease[TowerTypeCount];

	static const float TowerFireRange[TowerTypeCount];
	static const float TowerFireRangeIncrease[TowerTypeCount];

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

	friend class Game;
	friend class Projectile;

public:
	Tower(Game* game0, int type0);

	void fire();

	void showRange();

	int getRefund();

	void sellTower();

	~Tower();
};