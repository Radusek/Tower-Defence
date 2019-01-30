#include "App.h"
#include <iostream>

#include "Game.h"
#include "Menu.h"


//App::App() : window(VideoMode(1280, 720), "El Turret Defenso!", Style::Titlebar | Style::Close), sceneIndex(EMenu)
App::App() : window(VideoMode::getDesktopMode(), "El Turret Defenso!", Style::Fullscreen), sceneIndex(EMenu)
{
	scenes[EMenu] = new Menu(*this);
	scenes[EGame] = nullptr;

	window.setFramerateLimit(240);
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