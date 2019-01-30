#include "TextAnimation.h"


TextAnimation::TextAnimation(Text text0, float angle0, float rotation0, float velocity0, float scale0, Time animDuration0) :
text(text0), angle(angle0), rotation(rotation0), velocity(velocity0), scale(scale0), animDuration(animDuration0)
{

}

void TextAnimation::update()
{
	Time currentTime = clock.getElapsedTime();

	if (currentTime.asSeconds() >= animDuration.asSeconds())
		return;
	
	float percentDone = currentTime.asSeconds() / animDuration.asSeconds();

	// fading effect
	if (percentDone > 0.5f)
	{
		Color color = text.getFillColor();
		color.a = Uint8(255.f * ((1.f - percentDone)/0.5f));
		text.setFillColor(color);
	}

	

	// moving, rotation
	text.move(Vector2f(scale*velocity*cos(angle), scale*velocity*sin(angle)));
	text.rotate(rotation);
}

bool TextAnimation::finished()
{
	Time currentTime = clock.getElapsedTime();
	return currentTime.asSeconds() >= animDuration.asSeconds();
}

TextAnimation::~TextAnimation()
{
}