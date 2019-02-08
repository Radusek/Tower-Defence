#pragma once
#include <SFML/Graphics.hpp>
using namespace sf;

class Game;

class TextAnimation
{
private:
	Text text;
	
	float angle;
	float rotation;
	float velocity;
	float scale;

	Time timeElapsed;
	float animDuration;

	friend class Game;

public:
	TextAnimation(Text text0, float angle0, float rotation0, float velocity0, float scale0, float animDuration0);

	void update(Game* game);

	bool finished();

	~TextAnimation();
};