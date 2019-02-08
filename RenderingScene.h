#pragma once
#include <SFML/Graphics.hpp>
#define TSIZE 64 // Size of a single texture element
#define TSIZE_F 64.f // Size of a single texture element (float)
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

	int framerate;

	float scale;

	Font font;

	View view;
	
	Texture tileTexture;
	int tileSize;
	   
	
	friend class App;

public:
	RenderingScene(App& app0);

	virtual void setView() = 0;

	virtual void passEvent(Event event) = 0;

	virtual void leftMouseClick(Vector2i pos) = 0;

	virtual int checkCursorPosition(Vector2i pos) = 0;

	virtual void updateLogic() = 0;

	virtual void loadGraphicsToWindow() = 0;

	virtual ~RenderingScene();
};