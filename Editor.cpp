#include "Editor.h"
#include "App.h"
#include "Map.h"


Editor::Editor(App& app0) : RenderingScene(app0), map(nullptr)
{
	map = new Map();
	map->create(Vector2u(24, 16));
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
	float tileSize = 60.f;

	float offsetX = 3.f*tileSize / 2.f, offsetY = 3.f*tileSize / 2.f;
	Color color[2] = { {165, 132, 33, 255}, {100, 100, 100, 255} };
	float outlineThickness[2] = { 0.f, 1.f };

	for (unsigned int i = 0; i < map->size.y; i++)
	{
		for (unsigned int j = 0; j < map->size.x; j++)
		{
			bool tileType = map->boolGrid[i][j];

			float rectSize = tileSize - 2 * outlineThickness[tileType];
			RectangleShape rect(Vector2f(rectSize, rectSize));
			rect.setScale(scale, scale);

			float posX = scale * (offsetX + j * tileSize);
			float posY = scale * (offsetY + i * tileSize);

			rect.setPosition(posX, posY);
			rect.setOrigin(rectSize / 2.f, rectSize / 2.f);

			rect.setOutlineColor(Color::Black);
			rect.setOutlineThickness(outlineThickness[tileType]);

			rect.setFillColor(color[tileType]);

			window->draw(rect);
		}
	}
}

Editor::~Editor()
{
	if (map != nullptr)
		delete map;
}