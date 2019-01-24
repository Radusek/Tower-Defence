#pragma once
#include <SFML/Graphics.hpp>

using namespace sf;

class App;

enum Directions
{
	Left,
	Right,
	Up,
	Down,
	DirectionCount
};


class RenderingScene
{
protected:
	App& app;

	RenderWindow* window;
	int nextScene;

	float scale;

	Font font;

	friend class App;

public:
	RenderingScene(App& app0);

	virtual void passEvent(Event event) = 0;

	virtual void leftMouseClick(Vector2i pos) = 0;

	virtual int checkCursorPosition(Vector2i pos) = 0;

	virtual void updateLogic() = 0;

	virtual void loadGraphicsToWindow() = 0;


	virtual ~RenderingScene();
};

