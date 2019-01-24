#pragma once
#include <SFML/Graphics.hpp>
#include "RenderingScene.h"

#define BUTTON_SIZE_X 0.35f
#define BUTTON_SIZE_Y 0.15f

#define BUTTON_GAP_Y (BUTTON_SIZE_Y / 2.f)


enum Buttons
{
	EPlay,
	EExit,
	ButtonCount
};



class Menu : public RenderingScene
{
private:
	Text buttonText[ButtonCount];

	Texture tBackground;
	Sprite background;

	Texture tButton;
	Sprite buttons[ButtonCount];


	float y0; // y postion of the first button


public:
	Menu(App& app0);

	void passEvent(Event event);

	void leftMouseClick(Vector2i pos);

	int checkCursorPosition(Vector2i pos);

	void updateLogic();

	void loadGraphicsToWindow();


	~Menu();
};

