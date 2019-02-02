#include "Animation.h"



Animation::Animation(float animDuration0, int nFrames0) : animDuration(animDuration0), nFrames(nFrames0)
{
	clock.restart();
}

int Animation::getFrame(bool isPaused)
{
	if (isPaused)
		clock.restart();
	else
		time += clock.restart();

	int frame = time.asSeconds() / animDuration / float(nFrames);
	frame %= nFrames;

	return frame;
}


Animation::~Animation()
{
}
