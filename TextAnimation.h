#pragma once
#include <SFML/Graphics.hpp>
using namespace sf;

class TextAnimation
{
private:
	Text text;
	
	float angle;
	float rotation;
	float velocity;
	float scale;

	Clock clock;
	Time timeElapsed;
	Time animDuration;

	friend class Game;

public:
	TextAnimation(Text text0, float angle0, float rotation0, float velocity0, float scale0, Time animDuration0);

	void update(bool isPaused);

	bool finished();

	~TextAnimation();
};