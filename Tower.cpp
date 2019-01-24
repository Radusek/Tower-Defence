#include "Tower.h"

#include "Game.h"
#include "Minion.h"
#include "Projectile.h"

const int Tower::TowerCost[] = { 20, 30 };

const int Tower::TowerDamage[] = {3, 7};
const int Tower::TowerArmorPenetration[] = {1, 0};

const float Tower::TowerFireRate[] = {0.8f, 1.4f}; // reload time [s]
const float Tower::TowerFireRange[] = { 140.f, 185.f }; // !WITHOUT SCALING! needs to be done elsewhere

const float Tower::TowerProjectileSpeed[] = {2.8f, 0.1f};



Tower::Tower(Game* game0, int type0) : upgrades{}, game(game0)
{
	type = type0;

	damage = TowerDamage[type];
	armorPenetration = TowerArmorPenetration[type];

	fireRate = TowerFireRate[type];
	fireRange = TowerFireRange[type];


	Vector2f towerScale;
	towerScale.x = float(game->tileSize / 64.f);
	towerScale.y = float(game->tileSize / 64.f);


	sprite.setTexture(game->towerTexture);

	sprite.setScale(game->scale, game->scale);
	sprite.scale(towerScale);

	sprite.setOrigin(32.f, 32.f);

	clock.restart();
}

void Tower::fire()
{
	Time timeFromLastShot = clock.getElapsedTime();
	for (auto &minion : game->minions)
	{
		float diffX = sprite.getPosition().x - minion->sprite.getPosition().x;
		float diffY = sprite.getPosition().y - minion->sprite.getPosition().y;

		if (sqrtf(diffX*diffX + diffY*diffY) <= fireRange * game->tileSize/64.f * game->scale)
		{

			float diffX = minion->sprite.getPosition().x - sprite.getPosition().x;
			float diffY = minion->sprite.getPosition().y - sprite.getPosition().y;

			float angle = std::atan2(diffY, diffX);
			sprite.setRotation(angle * 180.f / M_PI);

			if (timeFromLastShot.asSeconds() >= fireRate)
			{
				game->projectiles.push_back(new Projectile(game, minion, type, damage, armorPenetration, TowerProjectileSpeed[type], sprite.getPosition()));
				clock.restart();
			}
			return;
		}
	}
}



Tower::~Tower()
{
}
