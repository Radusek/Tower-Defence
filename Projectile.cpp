#include "Projectile.h"

#include "Minion.h"
#include "Tower.h"
#include "Game.h"
#include "Map.h"
#include "Animation.h"

using namespace sf;


const int Projectile::ProjectileAnimationFrames[] = { 1, 5 };



Projectile::Projectile(Game* game0, Minion* target0, int type0, int damage0, int armorPenetration0, float velocity0, Vector2f position) :
	game(game0), target(target0), type(type0), damage(damage0), armorPenetration(armorPenetration0), guided(false), hit(false)
{
	velocity = velocity0;
	sprite.setPosition(position);

	targetId = target->id;
	chase(target);

	if (type == Missile)
	{
		guided = true;
		acceleration = 0.005f;
	}

	radius = 10.f;

	sprite.setTexture(game->projectileTexture);
	sprite.setScale(Vector2f(1.f, 1.f) * game->scale * float(game->tileSize) / 96.f);
	sprite.setOrigin(32.f, 32.f);

	float animDuration = 0.15f;
	animation = new Animation(animDuration, Projectile::ProjectileAnimationFrames[type], game);
}

void Projectile::setMovementIteration()
{
	movementIteration = target->sprite.getPosition() - sprite.getPosition();

	Vector2f versor = movementIteration;
	float length = sqrtf(versor.x*versor.x + versor.y*versor.y);

	movementIteration /= length;
}

void Projectile::move()
{
	if (guided)
	{
		bool targetAvailable = false;
		for (auto &minion : game->minions)
		{
			if (minion->id == targetId)
			{
				chase(minion);
				targetAvailable = true;
				break;
			}
		}

		if (targetAvailable == false && game->minions.size() > 0)
			chase(game->minions[0]);
	}

	float timeFactor = 240.f / game->framerate * game->timeScale[game->timeIndex];
	velocity += timeFactor * acceleration;

	float tileScale = game->tileSize / 64.f;
	sprite.move(movementIteration * (timeFactor * velocity * tileScale * game->scale));

	checkCollisions();
	destroyIfBeyondMap();
}

void Projectile::checkCollisions()
{
	float tileScale = game->tileSize / 64.f;

	for (auto minion : game->minions)
	{
		Vector2f diff(minion->sprite.getPosition() - sprite.getPosition());

		if (sqrtf(diff.x*diff.x + diff.y*diff.y) <= (minion->radius + radius) * tileScale * game->scale)
		{
			minion->takeHit(this);
			return;
		}
	}
}

void Projectile::destroyIfBeyondMap()
{
	Vector2f pos = sprite.getPosition();
	Vector2f bound = Vector2f(game->map->size) * float(game->tileSize) * game->scale;

	if (pos.x < 0 || pos.y < 0 || pos.x > bound.x || pos.y > bound.y)
		hit = true;
}

void Projectile::chase(Minion* newTarget)
{
	target = newTarget;
	setMovementIteration();

	angle = std::atan2(movementIteration.y, movementIteration.x);
	sprite.setRotation(angle * 180.f / M_PI);
}

Projectile::~Projectile()
{
	delete animation;
}