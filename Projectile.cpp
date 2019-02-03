#include "Projectile.h"

#include "Minion.h"
#include "Tower.h"
#include "Game.h"
#include "Map.h"
#include "Animation.h"


const int Projectile::ProjectileAnimationFrames[] = { 1, 5 };



Projectile::Projectile(Game* game0, Minion*& target0, int type0, int damage0, int armorPenetration0, float velocity0, Vector2f position) :
	game(game0), target(target0), type(type0), damage(damage0), armorPenetration(armorPenetration0), guided(false), hit(false)
{
	velocity = velocity0;
	sprite.setPosition(position);

	float diffX = target->sprite.getPosition().x - sprite.getPosition().x;
	float diffY = target->sprite.getPosition().y - sprite.getPosition().y;

	angle = std::atan2(diffY, diffX);


	if (type == Missile)
	{
		guided = true;
		acceleration = 0.005f;
	}

	Vector2f projectileScale;
	projectileScale.x = float(game->tileSize / 96.f);
	projectileScale.y = float(game->tileSize / 96.f);

	radius = 10.f;

	sprite.setTexture(game->projectileTexture);

	sprite.setScale(game->scale, game->scale);
	sprite.scale(projectileScale);

	sprite.setOrigin(32.f, 32.f);
	sprite.setRotation(angle * 180.f / M_PI);

	movementIteration = sprite.getPosition();

	movementIteration.x -= target->sprite.getPosition().x;
	movementIteration.y -= target->sprite.getPosition().y;

	float length = sqrtf(movementIteration.x*movementIteration.x + movementIteration.y*movementIteration.y);

	movementIteration.x /= -length;
	movementIteration.y /= -length;

	targetId = target->id;

	animation = new Animation(0.02f, Projectile::ProjectileAnimationFrames[type], game);
}

void Projectile::move()
{
	float timeFactor = game->timeScale[game->timeIndex];

	float tileScale = game->tileSize / 64.f;

	if (guided)
	{
		bool targetAvailable = false;
		for (auto &minion : game->minions)
		{
			if (minion->id == targetId)
			{
				chase(minion);
				targetAvailable = true;
			}
		}

		if (targetAvailable == false)
		{
			if (game->minions.size() > 0)
				chase(game->minions[0]);
		}
	}

	velocity += timeFactor * acceleration;

	sprite.move(timeFactor * movementIteration * velocity * tileScale * game->scale);
	
	// Checking collisions with minions
	for (auto minion : game->minions)
	{
		float diffX = minion->sprite.getPosition().x - sprite.getPosition().x;
		float diffY = minion->sprite.getPosition().y - sprite.getPosition().y;

		if (sqrtf(diffX*diffX + diffY*diffY) <= (minion->radius + radius) * game->tileSize/64.f * game->scale)
		{
			minion->takeHit(this);
			return;
		}
	}

	// Checking if it's still within the map
	float px = sprite.getPosition().x;
	float py = sprite.getPosition().y;

	float xBound = game->map->size.x * 64.f * tileScale * game->scale;
	float yBound = game->map->size.y * 64.f * tileScale * game->scale;

	if (px < 0 || py < 0 || px > xBound || py > yBound)
		hit = true;
}

void Projectile::chase(Minion *& target)
{
	float diffX = target->sprite.getPosition().x - sprite.getPosition().x;
	float diffY = target->sprite.getPosition().y - sprite.getPosition().y;

	angle = std::atan2(diffY, diffX);
	sprite.setRotation(angle * 180.f / M_PI);

	movementIteration = sprite.getPosition();

	movementIteration.x -= target->sprite.getPosition().x;
	movementIteration.y -= target->sprite.getPosition().y;

	float length = sqrtf(movementIteration.x*movementIteration.x + movementIteration.y*movementIteration.y);

	movementIteration.x /= -length;
	movementIteration.y /= -length;
}

Projectile::~Projectile()
{
	delete animation;
}