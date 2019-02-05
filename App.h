#pragma once
#include <SFML/Graphics.hpp>
using namespace sf;

class RenderingScene;

enum scenes
{
	EMenu,
	EEditor,
	EGame,
	ScenesCount
};


class App
{
private:
	RenderWindow window;
	int sceneIndex;
	int framerate;

	friend class RenderingScene;

	RenderingScene* scenes[ScenesCount];

public:
	App();

	void run();

	void listenForInput();

	void sceneChanging();

	void updateScreen();

	~App();
};