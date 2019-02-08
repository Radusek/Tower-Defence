#pragma once
#include <SFML/Graphics.hpp>
#define UNSET -1
#define BASE_WIDTH 1920.f
#define BASE_HEIGHT 1080.f
#define BASE_100X_RATIO 177
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