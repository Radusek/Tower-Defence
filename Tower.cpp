#include "Tower.h"

#include "Game.h"
#include "Minion.h"
#include "Projectile.h"

const int Tower::TowerCost[] = { 100, 125 };

const int Tower::TowerDamage[] = {3, 7};
const int Tower::TowerArmorPenetration[] = {1, 0};

const float Tower::TowerFireRate[] = {0.4f, 1.4f}; // reload time [s]
const float Tower::TowerFireRange[] = { 140.f, 180.f }; // !WITHOUT SCALING! needs to be done elsewhere

const int Tower::TowerUpgradeCost[] = { 75, 50, 65, 100 };

const int Tower::TowerUpgradeLimit[TowerTypeCount][UpgradesCount] = { {5, 3, 6, 6}, {10, 2, 9, 9} };


Tower::Tower(Game* game0, int type0) : upgrades{}, game(game0), sold(false), type(type0)
{
	damage = TowerDamage[type];
	armorPenetration = TowerArmorPenetration[type];

	fireRate = TowerFireRate[type];
	fireRange = TowerFireRange[type];

	sprite.setTexture(game->towerTexture);
	sprite.setScale(Vector2f(1.f, 1.f) * game->scale * float(game->tileSize) / 64.f);
	sprite.setOrigin(32.f, 32.f);

	Vector2f towerPosition = Vector2f(game->selectedTile) + Vector2f(0.5f, 0.5f);
	sprite.setPosition(towerPosition * game->scale * float(game->tileSize));
}

void Tower::fire()
{
	if (game->isPaused)
		return;
	
	timeFromLastShot += seconds(game->timeScale[game->timeIndex] * game->frameTime.asSeconds());

	for (auto &minion : game->minions)
	{
		Vector2f diff = minion->sprite.getPosition() - sprite.getPosition();

		if (sqrtf(diff.x*diff.x + diff.y*diff.y) <= fireRange * game->tileSize/64.f * game->scale)
		{
			float angle = std::atan2(diff.y, diff.x);
			sprite.setRotation(angle * 180.f / M_PI);

			if (timeFromLastShot.asSeconds() >= fireRate)
			{
				game->projectiles.push_back(new Projectile(game, minion, this));
				timeFromLastShot = seconds(0.f);
			}

			return;
		}
	}
}

void Tower::showRange()
{
	float r = fireRange * game->scale * game->tileSize / 64.f;

	CircleShape rangeCircle(r);
	rangeCircle.setOrigin(r, r);
	rangeCircle.setPosition(sprite.getPosition());

	rangeCircle.setFillColor(Color(0, 200, 0, 15));
	rangeCircle.setOutlineColor(Color(0, 0, 0, 255));
	rangeCircle.setOutlineThickness(1.5f * game->scale);

	game->window->draw(rangeCircle);
}

int Tower::getRefund()
{
	int refund = 0.8f * Tower::TowerCost[type];
	for (int i = 0; i < UpgradesCount; i++)
		refund += 0.6f * upgrades[i] * Tower::TowerUpgradeCost[i];
	
	return refund;
}

void Tower::sellTower()
{
	game->dollars += getRefund();
	sold = true;
	game->selectedStatus = FreeCell;
}

Tower::~Tower()
{
}