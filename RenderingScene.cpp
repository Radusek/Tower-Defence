#include "RenderingScene.h"

#include "App.h"

RenderingScene::RenderingScene(App& app0) : app(app0), nextScene(UNSET), tileSize(96)
{
	window = &(app.window);

	Vector2f baseResolution(BASE_WIDTH, BASE_HEIGHT);
	scale = window->getSize().x / baseResolution.x;

	framerate = app.framerate;

	font.loadFromFile("font/font.ttf");

	tileTexture.loadFromFile("img/tileTexture.png");
}

RenderingScene::~RenderingScene()
{
}