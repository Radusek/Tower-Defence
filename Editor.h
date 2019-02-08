#pragma once
#include "RenderingScene.h"

class Map;

class Editor : public RenderingScene
{
private:
	Map* map;

	friend class App;

public:
	Editor(App& app0);

	void setView();

	void passEvent(Event event);

	void leftMouseClick(Vector2i pos);

	int checkCursorPosition(Vector2i pos);

	void updateLogic();

	void loadGraphicsToWindow();

	~Editor();
};