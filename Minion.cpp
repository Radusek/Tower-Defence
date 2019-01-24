#include "Minion.h"
#include <iostream>

#include "Game.h"
#include "Projectile.h"
#include "Map.h"

Minion::Minion(Game* game0, int minionId) : game(game0), pathIndex(1), hp(MINION_BASE_HP), armor(0), type(Normal), lives(true), gotToTheEnd(false), id(minionId), angle(0.f)
{
	setVelocity(0.3f);
		
	radius = 16.f;

	Vector2f position;
	position.x = (game->map->path[0].x + 0.5f) * game->tileSize * game->scale;
	position.y = (game->map->path[0].y + 0.5f) * game->tileSize * game->scale;
	sprite.setPosition(position);


	Vector2f minionScale;
	minionScale.x = float(game->tileSize / 64.f);
	minionScale.y = float(game->tileSize / 64.f);

	sprite.setTexture(game->minionTexture);

	sprite.setScale(game->scale, game->scale);
	sprite.scale(minionScale);

	sprite.setOrigin(32.f, 32.f);
}

void Minion::move()
{
	float tileScale = game->tileSize / 64.f;

	float currentX = sprite.getPosition().x;
	float currentY = sprite.getPosition().y;

	float destX = game->scale * game->tileSize * (game->map->path[pathIndex].x + 0.5f);
	float destY = game->scale * game->tileSize * (game->map->path[pathIndex].y + 0.5f);

	float diffX = destX - currentX;
	float diffY = destY - currentY;

	float diffAngle = std::atan2(diffY, diffX) - angle;

	if (diffAngle > M_PI)
	{
		diffAngle -= 2.f*M_PI;
	}

	if (diffAngle < -M_PI)
	{
		diffAngle += 2.f*M_PI;
	}

	if(std::abs(diffAngle) * 180.f / M_PI < 4.f)
		angle = std::atan2(diffY, diffX);
	else
	{
		if (diffAngle > 0)
			angle += M_PI / 50.f;
		else
			angle -= M_PI / 50.f;
	}

	float dx = game->scale*cos(angle)*velocity*tileScale;
	float dy = game->scale*sin(angle)*velocity*tileScale;

	
	if (sqrtf(diffX*diffX + diffY*diffY) <= sqrtf(dx*dx + dy*dy)) // Point reached
	{
		if (pathIndex == game->map->path.size() - 1)
		{
			lives = false;
			gotToTheEnd = true;
			return;
		}
		pathIndex++;
	}	

	sprite.setRotation(angle * 180.f / M_PI);
	sprite.move(dx, dy);
}

void Minion::takeHit(Projectile* projectile)
{
	int damageReduction = armor - projectile->armorPenetration;

	if (damageReduction < 0)
		damageReduction = 0;

	hp -= projectile->damage - damageReduction;

	if (hp <= 0)
		lives = false;

	projectile->hit = true;
}


Minion::~Minion()
{
}
