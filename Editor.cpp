#include "Editor.h"
#include "App.h"
#include "Map.h"


Editor::Editor(App& app0) : RenderingScene(app0), map(nullptr)
{
	map = new Map();
	map->create(Vector2u(24, 16));
}

void Editor::setView()
{
	float screenPart = 1.f;
	Vector2f viewSize(Vector2f(window->getSize()) * screenPart);

	view = View(FloatRect(0.f, 0.f, viewSize.x, viewSize.y));
	view.setViewport(sf::FloatRect(0.f, 0.f, screenPart, screenPart));
}

void Editor::passEvent(Event event)
{
	if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)
		nextScene = EMenu;
}


void Editor::leftMouseClick(Vector2i pos)
{
}

int Editor::checkCursorPosition(Vector2i pos)
{
	return 0;
}

void Editor::updateLogic()
{
}

void Editor::loadGraphicsToWindow()
{
	Sprite tile(tileTexture);
	tile.setScale(Vector2f(1.f, 1.f) * float(tileSize) / 64.f * scale);

	for (unsigned int i = 0; i < map->size.y; i++)
	{
		for (unsigned int j = 0; j < map->size.x; j++)
		{
			tile.setTextureRect(IntRect(map->boolGrid[i][j] * 64, 0, 64, 64));

			Vector2f pos(Vector2f(j, i) * float(tileSize) * scale);
			tile.setPosition(pos);

			window->draw(tile);
		}
	}
}

Editor::~Editor()
{
	if (map != nullptr)
		delete map;
}