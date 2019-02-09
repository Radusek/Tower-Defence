#pragma once
#include <SFML/Graphics.hpp>
using namespace sf;

class Game;

class Animation
{
private:
	Time time;

	float animDuration; // [seconds]

	int nFrames;

	Game* game;

public:
	Animation(float animDuration0, int nFrames0, Game* game0);

	int getFrame(bool isPaused);

	~Animation();
};