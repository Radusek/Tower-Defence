#include "Menu.h"
#include <iostream>

#include "App.h"


Menu::Menu(App& app0) : RenderingScene(app0)
{
	// BACKGROUND
	tBackground.loadFromFile("img/menuBackground.png");
	background.setTexture(tBackground);

	background.setScale(scale, scale);


	// BUTTONS
	String buttonNames[ButtonCount] = { "Play", "Exit" };

	tButton.loadFromFile("img/menuButton.png");
	tButton.setSmooth(true);

	y0 = window->getSize().y * (0.5f - float((ButtonCount-1)/2.f) * (BUTTON_SIZE_Y + BUTTON_GAP_Y));

	for (int i = 0; i < ButtonCount; i++)
	{
		// Graphics
		buttons[i].setTexture(tButton);
		buttons[i].setOrigin(Vector2f(tButton.getSize() / 2u));
			  

		buttons[i].scale(Vector2f(window->getSize().x*BUTTON_SIZE_X/tButton.getSize().x, window->getSize().y*BUTTON_SIZE_Y/tButton.getSize().y));
			  
		buttons[i].setPosition(float(window->getSize().x / 2u), y0 + float(i)*(BUTTON_SIZE_Y + BUTTON_GAP_Y)*window->getSize().y);

		// Text
		buttonText[i].setFont(font);
		buttonText[i].setCharacterSize(72);
		buttonText[i].setString(buttonNames[i]);

		Vector2f newTextOrigin = { buttonText[i].getLocalBounds().width / 2.f, buttonText[i].getLocalBounds().height / 2.f + 24.f };
		buttonText[i].setOrigin(newTextOrigin);

		buttonText[i].setScale(scale, scale);

		buttonText[i].setPosition(buttons[i].getPosition());

	}

}

void Menu::passEvent(Event event)
{
	if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)
		window->close();

	if (event.type == Event::MouseButtonPressed) {
		if (event.mouseButton.button == Mouse::Left) {
			leftMouseClick(Mouse::getPosition(*window));
		}
	}

	if (event.type == sf::Event::MouseMoved)
		checkCursorPosition(Mouse::getPosition(*window));
}

void Menu::leftMouseClick(Vector2i pos)
{
	int option = checkCursorPosition(pos);

	if (option >= 0 && option < EExit)
	{
		Cursor cursor;
		if (cursor.loadFromSystem(sf::Cursor::Arrow))
			window->setMouseCursor(cursor);

		if (option == EPlay)
			nextScene = 1; // EGame
	}
	else if (option == EExit)
		window->close();
}

int Menu::checkCursorPosition(Vector2i pos)
{
	Cursor cursor;
	for (int i = 0; i < ButtonCount; i++)
	{
		if (std::abs(pos.x - (float(window->getSize().x / 2u))) < BUTTON_SIZE_X * window->getSize().x / 2.f
			&& std::abs(pos.y - (y0 + float(i)*(BUTTON_SIZE_Y + BUTTON_GAP_Y)*window->getSize().y)) < BUTTON_SIZE_Y * window->getSize().y / 2.f)
		{		
			if (cursor.loadFromSystem(sf::Cursor::Hand))
				window->setMouseCursor(cursor);
			return i;
		}
	}

	if (cursor.loadFromSystem(sf::Cursor::Arrow))
		window->setMouseCursor(cursor);

	return -1;
}

void Menu::updateLogic()
{
}

void Menu::loadGraphicsToWindow()
{
	window->draw(background);

	for (int i = 0; i < ButtonCount; i++)
	{
		window->draw(buttons[i]);
		window->draw(buttonText[i]);
	}
}

Menu::~Menu()
{
}