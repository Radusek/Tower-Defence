#include "App.h"
#include <iostream>
#include <fstream>

#include "Game.h"
#include "Menu.h"
#include "Editor.h"


App::App() : sceneIndex(EMenu)
{
	VideoMode mode = VideoMode::getDesktopMode();

	bool fullscreen = false;

	std::ifstream file("config/startup.txt");

	if (file.is_open())
	{
		std::string s;

		file >> s;
		file >> framerate;

		file >> s;
		file >> fullscreen;

		if (fullscreen == false)
		{
			file >> s >> mode.width;
			file >> s >> mode.height;
		}

		file.close();
	}

	unsigned int screenWidth = mode.width, screenHeight = mode.height;
	int ratio = 100 * screenWidth / screenHeight;

	if (ratio != BASE_100X_RATIO) // is not 16:9 resolution
	{
		if (ratio > BASE_100X_RATIO) // wider native resolution
		{
			mode.width = BASE_WIDTH / BASE_HEIGHT * mode.height;
		}
		else // narrow native resolution
		{
			mode.height = BASE_HEIGHT / BASE_WIDTH * mode.width;
		}
	}

	if(fullscreen && ratio == BASE_100X_RATIO)
		window.create(mode, "El Turret Defenso!", Style::Fullscreen);
	else
		window.create(mode, "El Turret Defenso!", Style::Titlebar | Style::Close);

	scenes[EMenu] = new Menu(*this);
	scenes[EGame] = nullptr;

	window.setFramerateLimit(framerate);
}

void App::run()
{
	while (window.isOpen())
	{
		// listening for input
		listenForInput();

		// operations
		sceneChanging();

		scenes[sceneIndex]->updateLogic();

		// drawing
		updateScreen();
	}
}

void App::listenForInput()
{
	sf::Event event;
	while (window.pollEvent(event))
	{
		if (event.type == sf::Event::Closed)
			window.close();

		scenes[sceneIndex]->passEvent(event);
	}
}

void App::sceneChanging()
{
	if (scenes[sceneIndex]->nextScene == UNSET)
		return;
	

	int newIndex = scenes[sceneIndex]->nextScene;

	delete scenes[sceneIndex];
	scenes[sceneIndex] = nullptr;
		
	sceneIndex = newIndex;

	switch (sceneIndex)
	{
	case EMenu:
		if (scenes[EMenu] == nullptr)
			scenes[EMenu] = new Menu(*this);

		break;
	case EEditor:
		if (scenes[EEditor] == nullptr)
			scenes[EEditor] = new Editor(*this);

		break;
	case EGame:
		if (scenes[EGame] == nullptr)
			scenes[EGame] = new Game(*this);

		break;
	}
}

void App::updateScreen()
{
	window.clear();
	
	scenes[sceneIndex]->loadGraphicsToWindow();

	window.display();
}

App::~App()
{
	for (auto scene : scenes)
		if (scene != nullptr)
			delete scene;
}