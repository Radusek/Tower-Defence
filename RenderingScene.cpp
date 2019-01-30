#include "RenderingScene.h"

#include "App.h"

RenderingScene::RenderingScene(App& app0) : app(app0), nextScene(-1)
{
	window = &(app.window);

	Vector2f baseResolution(1920.f, 1080.f);
	scale = window->getSize().x / baseResolution.x;

	font.loadFromFile("font/font.ttf");
}

RenderingScene::~RenderingScene()
{
}