#pragma once
#include <SFML/Graphics.hpp>
using namespace sf;

class Animation
{
private:
	Time time;
	Clock clock;

	float animDuration; // [seconds]

	int nFrames; 

public:
	Animation(float animDuration0, int nFrames0);

	int getFrame(bool isPaused);

	~Animation();
};

