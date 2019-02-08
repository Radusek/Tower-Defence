#include "TextAnimation.h"
#include "Game.h"


TextAnimation::TextAnimation(Text text0, float angle0, float rotation0, float velocity0, float scale0, float animDuration0) :
text(text0), angle(angle0), rotation(rotation0), velocity(velocity0), scale(scale0), animDuration(animDuration0)
{
}

void TextAnimation::update(Game* game)
{
	if (game->isPaused)
		return;

	timeElapsed += game->frameTime;

	if (timeElapsed.asSeconds() >= animDuration)
		return;
	
	float percentDone = timeElapsed.asSeconds() / animDuration;

	// fading effect
	float fadingTreshold = 0.5f;
	if (percentDone > fadingTreshold)
	{
		Color color = text.getFillColor();
		color.a = Uint8(255.f * ((1.f - percentDone) / fadingTreshold));
		text.setFillColor(color);
	}

	// moving, rotation
	text.move(Vector2f(cos(angle), sin(angle)) * scale * velocity);
	text.rotate(rotation);
}

bool TextAnimation::finished()
{
	return timeElapsed.asSeconds() >= animDuration;
}

TextAnimation::~TextAnimation()
{
}