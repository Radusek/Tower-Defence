#include "Animation.h"
#include "Game.h"



Animation::Animation(float animDuration0, int nFrames0, Game* game0) : animDuration(animDuration0), nFrames(nFrames0), game(game0)
{
	clock.restart();
}

int Animation::getFrame(bool isPaused)
{
	if (isPaused)
		clock.restart();
	else
		time += seconds(game->timeScale[game->timeIndex] / game->frameTimeQuotient * clock.restart().asSeconds());

	int frame = time.asSeconds() / animDuration / float(nFrames);
	frame %= nFrames;

	return frame;
}


Animation::~Animation()
{
}
