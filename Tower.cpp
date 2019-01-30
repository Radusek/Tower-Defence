#include "Tower.h"

#include "Game.h"
#include "Minion.h"
#include "Projectile.h"

const int Tower::TowerCost[] = { 100, 125 };

const int Tower::TowerDamage[] = {3, 7};
const int Tower::TowerArmorPenetration[] = {1, 0};

const float Tower::TowerFireRate[] = {0.4f, 1.4f}; // reload time [s]
const float Tower::TowerFireRange[] = { 140.f, 180.f }; // !WITHOUT SCALING! needs to be done elsewhere

const float Tower::TowerProjectileSpeed[] = {2.5f, 0.1f};



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

void Tower::showRange()
{
	float r = fireRange * game->scale * game->tileSize / 64.f;
	CircleShape rangeCircle;

	rangeCircle.setRadius(r);
	rangeCircle.setOrigin(r, r);
	rangeCircle.setPosition(sprite.getPosition());

	rangeCircle.setFillColor(Color(0, 200, 0, 15));
	rangeCircle.setOutlineColor(Color(0, 0, 0, 255));

	rangeCircle.setOutlineThickness(1.5f * game->scale);

	game->window->draw(rangeCircle);
}

void Tower::setRange(float range)
{
	fireRange = range;
}

Tower::~Tower()
{
}