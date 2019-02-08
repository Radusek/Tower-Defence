#include "Animation.h"
#include "Game.h"


Animation::Animation(float animDuration0, int nFrames0, Game* game0) : animDuration(animDuration0), nFrames(nFrames0), game(game0)
{
}

int Animation::getFrame(bool isPaused)
{
	if (isPaused == false)
		time += seconds(game->timeScale[game->timeIndex] * game->frameTime.asSeconds());

	int frame = time.asSeconds() / animDuration * nFrames;
	frame %= nFrames;

	return frame;
}

Animation::~Animation()
{
}