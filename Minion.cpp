#include "Minion.h"
#include <iostream>

#include "Game.h"
#include "Projectile.h"
#include "Map.h"
#include "Animation.h"

using namespace sf;

Minion::Minion(Game* game0) : game(game0), pathIndex(1), hp(getWaveHp(game->wave)), armor(0), type(Normal), lives(true), gotToTheEnd(false), id(game->minionId++), angle(0.f)
{
	setVelocity(0.4f + float(game->wave/50.f));
		
	radius = 26.f;

	Vector2f position = game->map->path[0] + Vector2f(0.5f, 0.5f);
	position *= game->tileSize * game->scale;
	sprite.setPosition(position);

	sprite.setTexture(game->minionTexture);
	sprite.setScale(Vector2f(1.f, 1.f) * game->scale * float(game->tileSize) / 64.f);
	sprite.setOrigin(32.f, 32.f);

	animation = new Animation(0.15f / velocity, 2, game);
}

int Minion::getWaveHp(int wave)
{
	return MINION_BASE_HP + 4 * int(sqrtf(wave)*wave - 1);
}

void Minion::move()
{
	Vector2f current = sprite.getPosition();

	Vector2f dest = game->map->path[pathIndex] + Vector2f(0.5f, 0.5f);
	dest *= game->scale * game->tileSize;

	Vector2f diff = dest - current;

	float angleDiff = std::atan2(diff.y, diff.x) - angle;
	int sign = 2 * ((angleDiff >= 0) - 0.5f); // -1 or 1

	if (angleDiff * sign > M_PI)
		angleDiff -= sign * 2.f*M_PI;

	bool rotating = false;
	float timeFactor = 240.f / game->framerate * game->timeScale[game->timeIndex];

	if (std::abs(angleDiff) * 180.f / M_PI < timeFactor * 3.f * velocity / 0.3f)
		angle = std::atan2(diff.y, diff.x);
	else
	{
		angle += std::abs(angleDiff)/angleDiff * timeFactor * velocity / 0.3f * M_PI / 70.f;
		rotating = true;
	}

	float tileScale = game->tileSize / 64.f;
	
	Vector2f vel(cos(angle), sin(angle));
	vel *= timeFactor * velocity * tileScale;

	bool moved = false;
	
	if (sqrtf(diff.x*diff.x + diff.y*diff.y) <= sqrtf(vel.x*vel.x + vel.y*vel.y)) // Point reached
	{
		if (pathIndex == game->map->path.size() - 1)
		{
			lives = false;
			gotToTheEnd = true;
			return;
		}
		sprite.setPosition(dest);
		moved = true;

		pathIndex++;	
	}	

	sprite.setRotation(angle * 180.f / M_PI);
	if(rotating == false && moved == false)
		sprite.move(vel);
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

void Minion::showHpBar()
{
	float scale = game->scale;
	int wave = game->wave;

	int hpWidth = 60 * scale, hpHeight = 10 * scale;
	int baseOutline = 1 + scale;

	RectangleShape hpBarBase;

	hpBarBase.setFillColor(Color::Black);

	hpBarBase.setSize(Vector2f(hpWidth + 2 * baseOutline, hpHeight + 2 * baseOutline));
	hpBarBase.setOrigin(Vector2f(hpWidth / 2 + baseOutline, hpHeight / 2 + baseOutline));
	hpBarBase.setPosition(Vector2f(sprite.getPosition()) + Vector2f(0.f, -40.f*scale));

	game->window->draw(hpBarBase);


	RectangleShape hpBar;

	float hpPercent = float(hp) / getWaveHp(wave);

	hpBar.setSize(Vector2f(hpPercent * hpWidth, hpHeight));
	hpBar.setFillColor(Color(255 * (1 - hpPercent), 255 * hpPercent, 0, 255));

	hpBar.setPosition(hpBarBase.getPosition() + Vector2f(-hpWidth / 2, -hpHeight / 2));
	
	game->window->draw(hpBar);
}

Minion::~Minion()
{
	delete animation;
}