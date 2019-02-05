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
			file >> s;
			file >> mode.width;

			file >> s;
			file >> mode.height;
		}

		file.close();
	}

	unsigned int screenWidth = mode.width, screenHeight = mode.height;
	int ratio = (100 * screenWidth) / screenHeight;

	if (ratio != 177) // is not 16:9 resolution
	{
		if (ratio > 177) // wider native resolution
		{
			mode.width = 16.f / 9.f * mode.height;
		}
		else // narrow native resolution
		{
			mode.height = 9.f / 16.f * mode.width;
		}
	}



	if(fullscreen && ratio == 177)
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
	if (scenes[sceneIndex]->nextScene != -1)
	{
		int newIndex = scenes[sceneIndex]->nextScene;

		delete scenes[sceneIndex];
		scenes[sceneIndex] = nullptr;
		
		sceneIndex = newIndex;
	}
	else
		return;

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