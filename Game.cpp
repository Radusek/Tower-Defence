#include "Game.h"
#include <iostream>


#include "Tower.h"
#include "Minion.h"
#include "Projectile.h"

#include "Map.h"

#include "App.h"

Game::Game(App& app0) : RenderingScene(app0), wave(1), dollars(50), livesLeft(20), tileSize(96.f), nMinionsSpawned(0), minionId(0), mapScrolling{}, selectedTile(0, 0), selectedStatus(Path)
{
	std::srand(std::time(NULL));

	float viewWidth = 0.8f*window->getSize().x;
	float viewHeight = 0.8f*window->getSize().y;

	view = View(FloatRect(0.f, 0.f, viewWidth, viewHeight));
	view.setViewport(sf::FloatRect(0.f, 0.f, 0.8f, 0.8f));

	map = new Map(3);

	tGameInterface.loadFromFile("img/gameInterface.png");
	gameInterface.setTexture(tGameInterface);
	gameInterface.setScale(scale, scale);

	towerTexture.loadFromFile("img/towerTexture.png");
	minionTexture.loadFromFile("img/minionTexture.png");
	projectileTexture.loadFromFile("img/projectileTexture.png");

	clock.restart();
}

void Game::passEvent(Event event)
{
	if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)
		nextScene = EMenu;

	if (event.type == Event::MouseButtonPressed) {
		if (event.mouseButton.button == Mouse::Left) {
			leftMouseClick(Mouse::getPosition(*window));
		}
	}

	if (event.type == sf::Event::KeyPressed)
	{
		if (event.key.code >= sf::Keyboard::Left && event.key.code <= sf::Keyboard::Down)
			mapScrolling[event.key.code - sf::Keyboard::Left] = true;
	}

	if (event.type == sf::Event::KeyReleased)
	{
		if (event.key.code >= sf::Keyboard::Left && event.key.code <= sf::Keyboard::Down)
			mapScrolling[event.key.code - sf::Keyboard::Left] = false;
	}
}

void Game::leftMouseClick(Vector2i pos)
{
	if (pos.x < 0.8f * scale * 1920.f && pos.y < 0.8f * scale * 1080.f) // klikniêto w obszar planszy
	{
		Vector2i clickedPoint = pos + Vector2i(view.getCenter()) - Vector2i(0.4f * 1920.f * scale, 0.4f * 1080.f * scale);
		
		selectedTile = clickedPoint / int(scale*tileSize);

		if (map->boolGrid[selectedTile.y][selectedTile.x] == 0)
		{
			selectedStatus = Path;
			return;
		}
		else
		{
			selectedStatus = OccupiedCell;
		}

		Vector2f newPosition(scale*(float(tileSize * selectedTile.x) + tileSize / 2.f), (scale*(float(tileSize * selectedTile.y) + tileSize / 2.f)));
		for (auto &tower : towers)
			if (tower->sprite.getPosition() == newPosition)
				return;

		selectedStatus = FreeCell;

	}
	else if (selectedStatus == FreeCell && pos.x < 0.35f * scale * 1920.f && pos.x >= 0.15f * scale * 1920.f && pos.y >= 0.85f * scale * 1080.f && pos.y < 0.95f * scale * 1080.f) // wybor towera
	{
		int towerChoice = pos.x - 0.15f * scale * 1920.f;
		towerChoice /= 0.1f * scale * 1920.f;

		towers.push_back(new Tower(this, towerChoice));
		towers.back()->sprite.setPosition(scale*(float(tileSize * selectedTile.x) + tileSize / 2.f), (scale*(float(tileSize * selectedTile.y) + tileSize / 2.f)));

		selectedStatus = OccupiedCell;
	}
}

int Game::checkCursorPosition(Vector2i pos)
{
	return 0;
}

void Game::updateLogic()
{
	cameraMoving();

	spawningObjects();

	movingObjects();
	
	destroyingObjects();
}

void Game::cameraMoving()
{
	int dx = mapScrolling[Right] - mapScrolling[Left];
	int dy = mapScrolling[Down] - mapScrolling[Up];

	int scrollingSpeed = 3;

	for (int i = 0; i < scrollingSpeed; i++)
	{
		if (dx > 0)
		{
			if (view.getCenter().x + view.getSize().x / 2.f < scale*(float(map->size.x)*tileSize))
				view.move(scale*1.f, 0.f);
		}
		else if (dx < 0)
		{
			if (view.getCenter().x - view.getSize().x / 2.f > 0.f)
				view.move(scale*-1.f, 0.f);
		}

		if (dy > 0)
		{
			if (view.getCenter().y + view.getSize().y / 2.f < scale*(float(map->size.y)*tileSize))
				view.move(0.f, scale*1.f);
		}
		else if (dy < 0)
		{
			if (view.getCenter().y - view.getSize().y / 2.f > 0.f)
				view.move(0.f, scale*-1.f);
		}
	}
}

void Game::spawningObjects()
{
	if (nMinionsSpawned < 3 + wave/2)
	{
		Time time = clock.getElapsedTime();
		if (time.asSeconds() >= 2.f - 0.00005f * float(wave) * float(wave)) // 2 seconds spawn interval and decreasing each wave
		{
			minions.push_back(new Minion(this, minionId++));
			nMinionsSpawned++;
			clock.restart();
		}
	}
	else if (minions.size() == 0)
	{
		nMinionsSpawned = 0;
		wave++;
	}

	for (auto &tower : towers)
		tower->fire();
}

void Game::movingObjects()
{
	for (auto &minion : minions)
		minion->move();

	for (auto &projectile : projectiles)
		projectile->move();
}

void Game::loadGraphicsToWindow()
{

	window->draw(gameInterface);
	
	if (selectedStatus == FreeCell)
	{
		RectangleShape towerButtonBase;
		towerButtonBase.setFillColor(Color(200, 200, 20, 150));

		towerButtonBase.setSize(Vector2f(0.1f*scale*1920.f, 0.1f*scale*1080.f));

		towerButtonBase.setOutlineThickness(2.f);
		towerButtonBase.setOutlineColor(Color::Black);
		

		for (int i = 0; i < 2; i++)
		{
			Vector2f pos(Vector2f((0.1f*float(i) + 0.15f)*scale*1920.f, 0.85f*scale*1080.f));

			towerButtonBase.setPosition(pos);
			window->draw(towerButtonBase);

			Sprite s(towerTexture);
			s.setTextureRect(IntRect(i * 64, 0, 64, 64));
			s.setPosition(pos);
			s.setScale(tileSize / 64.f * scale, tileSize / 64.f * scale);
			window->draw(s);
		}
	}

	float offsetX = tileSize / 2.f, offsetY = tileSize / 2.f;
	Color color[2] = { {200, 200, 200, 255}, {100, 100, 100, 255} };

	for (unsigned int i = 0; i < map->size.y; i++)
	{
		for (unsigned int j = 0; j < map->size.x; j++)
		{
			RectangleShape rect(Vector2f(tileSize, tileSize));
			rect.setPosition(scale*(offsetX + j*tileSize), scale*(offsetY + i*tileSize));
			rect.setOrigin(tileSize / 2.f, tileSize / 2.f);
			
			rect.setFillColor(color[map->boolGrid[i][j]]);
			rect.setScale(scale, scale);

			window->setView(view);
			window->draw(rect);
		}
	}

	if (selectedStatus > Path) // highlight selected tile
	{
		RectangleShape selection;
		selection.setFillColor(Color(0, 255, 0, 50));

		selection.setSize(Vector2f(scale*tileSize, scale*tileSize));
		selection.setPosition(Vector2f(selectedTile.x, selectedTile.y) * scale * float(tileSize));

		window->setView(view);
		window->draw(selection);
	}

	for (auto &tower : towers)
	{
		tower->sprite.setTextureRect(IntRect(tower->type * 64, 0, 64, 64));
		window->setView(view);
		window->draw(tower->sprite);	
	}

	for (auto &projectile : projectiles)
	{
		projectile->sprite.setTextureRect(IntRect(projectile->type * 64, 0, 64, 64));
		window->setView(view);
		window->draw(projectile->sprite);
	}

	{ // drawing minions with hp bars
		int hpWidth = 60 * scale, hpHeight = 10 * scale;
		int baseOutline = 2;

		RectangleShape hpBarBase;
		hpBarBase.setFillColor(Color::Black);

		hpBarBase.setSize(Vector2f(hpWidth + 2 * baseOutline, hpHeight + 2 * baseOutline));
		hpBarBase.setOrigin(Vector2f(hpWidth / 2 + baseOutline, hpHeight / 2 + baseOutline));

		RectangleShape hpBar;
		

		for (auto &minion : minions)
		{
			minion->sprite.setTextureRect(IntRect(minion->type * 64, 0, 64, 64));
			window->setView(view);
			window->draw(minion->sprite);

			hpBarBase.setPosition(Vector2f(minion->sprite.getPosition()) + Vector2f(0.f, -40.f*scale));
			window->draw(hpBarBase);

			float hpPercent = minion->hp / float(MINION_BASE_HP);

			hpBar.setFillColor(Color(255 * (1 - hpPercent),255 * hpPercent, 0, 255));
			hpBar.setSize(Vector2f(hpPercent * hpWidth, hpHeight));
			
			hpBar.setPosition(hpBarBase.getPosition() + Vector2f(-hpWidth/2, -hpHeight/2));
			window->draw(hpBar);
		}
	}


	window->setView(window->getDefaultView());
}

void Game::destroyingObjects()
{
	int vectorSize = minions.size();
	for (int i = 0; i < vectorSize; i++)
	{
		if (minions[i]->gotToTheEnd)
			livesLeft--;

		if (minions[i]->lives == false)
		{
			delete minions[i];
			minions.erase(minions.begin() + i);
			i--;
			vectorSize--;
		}
	}

	vectorSize = projectiles.size();
	for (int i = 0; i < vectorSize; i++)
	{
		if (projectiles[i]->hit)
		{
			delete projectiles[i];
			projectiles.erase(projectiles.begin() + i);
			i--;
			vectorSize--;
		}
	}
}


Game::~Game()
{
	for (auto &p : projectiles)
		delete p;
	projectiles.clear();

	for (auto &m : minions)
		delete m;
	minions.clear();

	for (auto &t : towers)
		delete t;
	towers.clear();

	delete map;
}
