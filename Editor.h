#pragma once
#include "RenderingScene.h"

class Map;

class Editor : public RenderingScene
{
private:
	Map* map;
	std::vector<std::pair<int, int>> path;

	bool readingSize;

	Texture backgroundTexture;
	Texture sizeBarTexture;
	Texture sizeBarSliderTexture;

	bool mapScrolling[DirectionCount];

	int draggingSlider; // 0 - not dragging, 1 or 2 mean which slider is being dragged
	float sliderPos[2];

	int mapSize[2];

	friend class App;

public:
	Editor(App& app0);

	void setView();

	void passEvent(Event event);

	void leftMouseClick(Vector2i pos);

	int checkCursorPosition(Vector2i pos);

	void saveMap();

	void updateLogic();

	void cameraMoving();

	void loadGraphicsToWindow();

	void printTiles();

	void printPath();

	void drawInitialScreen();

	~Editor();
};