#include <fstream>
#include "Editor.h"
#include "App.h"
#include "Map.h"

using namespace sf;


Editor::Editor(App& app0) : RenderingScene(app0), map(nullptr), readingSize(true), draggingSlider(false), mapSize{ 3, 3 }, mapScrolling{}
{
	map = new Map();

	setView();

	backgroundTexture.loadFromFile("img/menuBackground.png");
	sizeBarTexture.loadFromFile("img/sizeBar.png");
	sizeBarSliderTexture.loadFromFile("img/sizeBarSlider.png");
}

void Editor::setView()
{
	float screenPart = 1.f;
	Vector2f viewSize(Vector2f(window->getSize()) * screenPart);

	view = View(FloatRect(0.f, 0.f, viewSize.x, viewSize.y));
	view.setViewport(sf::FloatRect(0.f, 0.f, screenPart, screenPart));

	view.setCenter(view.getSize() / 2.f);
}

void Editor::passEvent(Event event)
{
	if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)
		nextScene = EMenu;

	if (event.type == Event::MouseButtonPressed) {
		Vector2i pos = Mouse::getPosition(*window);

		if (event.mouseButton.button == Mouse::Left) {
			leftMouseClick(Mouse::getPosition(*window));
		}
	}

	if (event.type == Event::MouseButtonReleased) {
		if (event.mouseButton.button == Mouse::Left) {
			draggingSlider = false;
		}
	}

	if (event.type == Event::MouseMoved) {
		checkCursorPosition(Mouse::getPosition(*window));
	}

	if (event.type == sf::Event::KeyPressed)
	{
		if (event.key.code == sf::Keyboard::Enter)
			if (readingSize)
			{
				Vector2u vec(mapSize[0], mapSize[1]);
				map->create(vec);
				readingSize = false;
			}

		if (event.key.code >= sf::Keyboard::Left && event.key.code <= sf::Keyboard::Down)	
			mapScrolling[event.key.code - sf::Keyboard::Left] = true;		
		

		Vector2f pos = Vector2f(Mouse::getPosition(*window));
		pos = window->mapPixelToCoords(Vector2i(pos), view);
		pos /= float(tileSize) * scale;

		Vector2i tile = Vector2i(pos);

		if (event.key.code >= sf::Keyboard::Num1 && event.key.code <= sf::Keyboard::Num2)
		{
			int val = event.key.code - Keyboard::Num1;

			if (tile.x > 0 && tile.y > 0 && tile.x < mapSize[0] + 1 && tile.y < mapSize[1] + 1)
				map->boolGrid[tile.y - 1][tile.x - 1] = val;
		}

		if (event.key.code == sf::Keyboard::F)
		{
			if (tile.x < mapSize[0] + 2 && tile.y < mapSize[1] + 2)
				path.push_back({ tile.x, tile.y });
		}

		if (event.key.code == sf::Keyboard::S)	
			saveMap();
		
	}

	if (event.type == sf::Event::KeyReleased)
	{
		if (event.key.code >= sf::Keyboard::Left && event.key.code <= sf::Keyboard::Down)
			mapScrolling[event.key.code - sf::Keyboard::Left] = false;
	}

}


void Editor::leftMouseClick(Vector2i pos)
{
	if (readingSize)
	{
		for (int i = 0; i < 2; i++)
		{
			if (pos.x >= 710.f * scale && pos.x < 1210.f * scale && pos.y >= (150.f + 150.f*i) * scale && pos.y < (208.f + 150.f*i) * scale)
			{
				draggingSlider = i + 1;
				sliderPos[i] = (pos.x - 710.f * scale) / scale;
				mapSize[draggingSlider - 1] = 3 + sliderPos[i] / 6;
			}
		}
	}
}

int Editor::checkCursorPosition(Vector2i pos)
{
	if (readingSize && draggingSlider)
	{
		if (draggingSlider)
		{
			pos.x -= 710.f * scale;
			float newPos;

			if (pos.x < 0)
				newPos = 0.f;
			else if (pos.x > 500.f * scale)
				newPos = 500.f;
			else
				newPos = pos.x / scale;

			sliderPos[draggingSlider - 1] = newPos;
			mapSize[draggingSlider - 1] = 3 + newPos / 6;
		}
	}

	return 0;
}

void Editor::saveMap()
{
	std::ofstream file("levels/editorlevel.txt");

	file << mapSize[0] << " " << mapSize[1] << "\n";

	for (int i = 0; i < mapSize[1]; i++)
	{
		for (int j = 0; j < mapSize[0]; j++)
		{
			file << map->boolGrid[i][j] << " ";
		}
		file << "\n";
	}

	file << path.size() << "\n";

	for (int i = 0; i < path.size(); i++)
	{
		file << path[i].first - 1 << " " << path[i].second - 1 << "\n";
	}

	file.close();
}

void Editor::updateLogic()
{
	cameraMoving();
}

void Editor::cameraMoving()
{
	int dx = mapScrolling[Right] - mapScrolling[Left];
	int dy = mapScrolling[Down] - mapScrolling[Up];

	int scrollingSpeed = 3;

	for (int i = 0; i < scrollingSpeed * 240.f / framerate; i++)
	{
		if (dx > 0)
		{
			if (view.getCenter().x + view.getSize().x / 2.f < scale*(float(map->size.x + 2)*tileSize))
				view.move(scale*1.f, 0.f);
		}
		else if (dx < 0)
		{
			if (view.getCenter().x - view.getSize().x / 2.f > 0.f)
				view.move(scale*-1.f, 0.f);
		}

		if (dy > 0)
		{
			if (view.getCenter().y + view.getSize().y / 2.f < scale*(float(map->size.y + 2)*tileSize))
				view.move(0.f, scale*1.f);
		}
		else if (dy < 0)
		{
			if (view.getCenter().y - view.getSize().y / 2.f > 0.f)
				view.move(0.f, scale*-1.f);
		}
	}
}

void Editor::loadGraphicsToWindow()
{
	if (readingSize)
	{
		drawInitialScreen();
		return;
	}

	window->setView(view);

	printTiles();
	printPath();

	window->setView(window->getDefaultView());
}

void Editor::printTiles()
{
	Sprite tile(tileTexture);
	tile.setScale(Vector2f(1.f, 1.f) * float(tileSize) / TSIZE_F * scale);

	for (unsigned int i = 0; i < map->size.y; i++)
	{
		for (unsigned int j = 0; j < map->size.x; j++)
		{
			tile.setTextureRect(IntRect(map->boolGrid[i][j] * TSIZE, 0, TSIZE, TSIZE));

			Vector2f pos(Vector2f(j + 1, i + 1) * float(tileSize) * scale);
			tile.setPosition(pos);

			window->draw(tile);
		}
	}
}

void Editor::printPath()
{
	RectangleShape pathElement(Vector2f(1.f, 1.f) * float(tileSize) * scale);

	for (int i = 0; i < path.size(); i++)
	{
		float percent = float(i+1) / path.size();
		Color color(percent * 255, (1.f - percent) * 255, 0, 80);
		pathElement.setFillColor(color);
		pathElement.setPosition(Vector2f(path[i].first, path[i].second) * float(tileSize) * scale);
		
		window->draw(pathElement);
	}
}

void Editor::drawInitialScreen()
{
	Sprite s(backgroundTexture);
	s.setScale(scale, scale);
	window->draw(s);

	{
		Sprite bar(sizeBarTexture);
		Sprite slider(sizeBarSliderTexture);

		bar.setScale(scale, scale);
		slider.setScale(scale, scale);
		slider.setOrigin(19.f, 0.f);

		String str[] = { "width = ", "height = " };

		for (int i = 0; i < 2; i++)
		{
			Text text(str[i] + std::to_string(mapSize[i]), font, 54);
			text.setScale(scale, scale);
			text.setPosition(Vector2f(1250.f, 150.f*(i + 1)) * scale);

			bar.setPosition(Vector2f(710.f, 150.f*(i + 1)) * scale);
			slider.setPosition(Vector2f(710.f + sliderPos[i], 144.f + 150.f*i) * scale);

			window->draw(bar);
			window->draw(slider);
			window->draw(text);
		}
	}
}

Editor::~Editor()
{
	delete map;
}