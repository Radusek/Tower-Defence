#include "Animation.h"



Animation::Animation(float animDuration0, int nFrames0) : animDuration(animDuration0), nFrames(nFrames0)
{
	clock.restart();
}

int Animation::getFrame()
{
	Time time = clock.getElapsedTime();

	int frame = time.asSeconds() / animDuration / float(nFrames);
	frame %= nFrames;

	return frame;
}


Animation::~Animation()
{
}
