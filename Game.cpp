#include "Game.h"
#include <iostream>

#include "Tower.h"
#include "Minion.h"
#include "Projectile.h"
#include "TextAnimation.h"
#include "Animation.h"

#include "Map.h"
#include "App.h"

Game::Game(App& app0) : RenderingScene(app0), wave(1), dollars(120), livesLeft(20), tileSize(96.f), nMinionsSpawned(0), minionId(0), mapScrolling{}, selectedTile(0, 0), selectedStatus(Path), displayFakeTower(false), zoom(1.f), isPaused(false), timeIndex(0), baseTimeScale{1.f, 1.5f, 2.25f}, timeScale{ 1.f, 1.5f, 2.25f }
{
	std::srand(std::time(NULL));

	float viewWidth = 0.8f*window->getSize().x;
	float viewHeight = 0.8f*window->getSize().y;

	view = View(FloatRect(0.f, 0.f, viewWidth, viewHeight));
	view.setViewport(sf::FloatRect(0.f, 0.f, 0.8f, 0.8f));

	map = new Map();
	map->load(2);

	tGameInterface.loadFromFile("img/gameInterface.png");
	gameInterface.setTexture(tGameInterface);
	gameInterface.setScale(scale, scale);

	tileTexture.loadFromFile("img/tileTexture.png");
	towerTexture.loadFromFile("img/towerTexture.png");
	upgradeTexture.loadFromFile("img/upgradeTexture.png");
	minionTexture.loadFromFile("img/minionTexture.png");
	projectileTexture.loadFromFile("img/projectileTexture.png");

	fakeTower = new Tower(this, 0);

	fakeTower->sprite.setColor(Color(128, 128, 128, 255));

	pauseRect.setSize(Vector2f(0.8f*1920.f, 0.8f*1080.f));
	pauseRect.setScale(scale, scale);
	pauseRect.setFillColor(Color(0, 0, 0, 140));

	pauseText.setFont(font);
	pauseText.setScale(scale, scale);
	pauseText.setCharacterSize(72);
	pauseText.setPosition(0.4f * scale * 1920.f, 0.4f * scale * 1080.f);
	pauseText.setString("PAUSE");
	pauseText.setOrigin(pauseText.getLocalBounds().width / 2, pauseText.getLocalBounds().height / 2 + 30);

	debugClock.restart();
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

	if (event.type == Event::MouseMoved) {
		checkCursorPosition(Mouse::getPosition(*window));
	}

	if (event.type == Event::MouseWheelMoved)
	{
		if (event.mouseWheel.delta > 0)
		{
			if (zoom > 0.4f)
			{
				view.zoom(0.8f);
				zoom *= 0.8f;
			}
		}
		else
		{
			if (map->size.x * tileSize >= 1920.f*0.8f*zoom*1.25f - 0.1f && map->size.y * tileSize >= 1080.f*0.8f*zoom*1.25f - 0.1f)
			{
				view.zoom(1.25f);
				zoom *= 1.25f;

				Vector2f centerPos = view.getCenter();
				
				if (view.getCenter().x + view.getSize().x / 2.f > scale*(float(map->size.x)*tileSize))
					centerPos.x = scale * (float(map->size.x)*tileSize) - view.getSize().x / 2.f;
	
				else if (view.getCenter().x - view.getSize().x / 2.f < 0.f)
					centerPos.x = view.getSize().x / 2.f;

				
				if (view.getCenter().y + view.getSize().y / 2.f > scale*(float(map->size.y)*tileSize))
					centerPos.y = scale * (float(map->size.y)*tileSize) - view.getSize().y / 2.f;

				else if (view.getCenter().y - view.getSize().y / 2.f < 0.f)
					centerPos.y = view.getSize().y / 2.f;

				view.setCenter(centerPos);
			}
		}
	}

	if (event.type == sf::Event::KeyPressed)
	{
		if (event.key.code >= sf::Keyboard::Left && event.key.code <= sf::Keyboard::Down)
			mapScrolling[EKeyboard][event.key.code - sf::Keyboard::Left] = true;
		
		if (selectedStatus != Path && event.key.code >= Keyboard::Num1 && event.key.code <= Keyboard::Num4)
		{
			int purchaseChoice = event.key.code - Keyboard::Num1;

			if (purchaseChoice > 1 && selectedStatus == FreeCell)
				return;

			spendMoney(purchaseChoice);
		}

		if (event.key.code == Keyboard::P)
			isPaused = !isPaused;

		if (event.key.code == Keyboard::T)
			++timeIndex %= 3;
	}

	if (event.type == sf::Event::KeyReleased)
	{
		if (event.key.code >= sf::Keyboard::Left && event.key.code <= sf::Keyboard::Down)
			mapScrolling[EKeyboard][event.key.code - sf::Keyboard::Left] = false;
	}
}

void Game::leftMouseClick(Vector2i pos)
{
	if (pos.x < 0.8f * scale * 1920.f && pos.y < 0.8f * scale * 1080.f) // klikniêto w obszar planszy
	{
		Vector2f clickedPoint = window->mapPixelToCoords(pos, view);
		
		if (pos.x > map->size.x * tileSize * scale || pos.y > map->size.y * tileSize * scale) // klikniecie na obszar poza polem rozgrywki
		{
			selectedStatus = Path;
			return;
		}

		selectedTile = Vector2i(clickedPoint / (scale*tileSize));

		if (map->boolGrid[selectedTile.y][selectedTile.x] == 0)
		{
			selectedStatus = Path;
			return;
		}
		else
		{
			selectedStatus = OccupiedCell;
		}

		Vector2f newPosition((selectedTile.x + 0.5f) * scale * tileSize, (selectedTile.y + 0.5f) * scale * tileSize);
		for (auto &tower : towers)
			if (tower->sprite.getPosition() == newPosition)
				return;

		selectedStatus = FreeCell;

	}
	else if (selectedStatus != Path && pos.x < 0.55f * scale * 1920.f && pos.x >= 0.15f * scale * 1920.f && pos.y >= 0.85f * scale * 1080.f && pos.y < 0.95f * scale * 1080.f) // wybor towera
	{
		int purchaseChoice = pos.x - 0.15f * scale * 1920.f;
		purchaseChoice /= 0.1f * scale * 1920.f;

		if (purchaseChoice > 1 && selectedStatus == FreeCell)
			return;

		spendMoney(purchaseChoice);
	}

	if (selectedStatus == OccupiedCell && pos.x < scale * (0.05f * 1920.f + 0.1f * 1080.f) && pos.x >= 0.05f && pos.y >= 0.85f * scale * 1080.f && pos.y < 0.95f * scale * 1080.f)
	{
		Tower* t = nullptr;
		for (auto &tower : towers)
		{
			if (tower->sprite.getPosition() == Vector2f((selectedTile.x + 0.5f) * scale * tileSize, (selectedTile.y + 0.5f) * scale * tileSize))
			{
				t = tower;
				break;
			}
		}

		t->sellTower();
	}
}

int Game::checkCursorPosition(Vector2i pos)
{
	mapScrolling[EMouse][Left] = pos.x < 0.05f * scale * 1920.f;
	mapScrolling[EMouse][Right] = pos.x >= 0.95f * scale * 1920.f;

	mapScrolling[EMouse][Up] = pos.y < 0.05f * scale * 1080.f;
	mapScrolling[EMouse][Down] = pos.y >= 0.95f * scale * 1080.f;


	if (selectedStatus == FreeCell && pos.x < 0.35f * scale * 1920.f && pos.x >= 0.15f * scale * 1920.f && pos.y >= 0.85f * scale * 1080.f && pos.y < 0.95f * scale * 1080.f)
	{
		int towerType = pos.x - 0.15f * scale * 1920.f;
		towerType /= 0.1f * scale * 1920.f;

		fakeTower->type = towerType;

		float towerFireRange[] = { 140.f, 180.f };
		fakeTower->fireRange = towerFireRange[towerType];
		fakeTower->sprite.setTextureRect(IntRect(towerType * 64, 0, 64, 64));
		fakeTower->sprite.setPosition((selectedTile.x + 0.5f) * scale * tileSize, (selectedTile.y + 0.5f) * scale * tileSize);

		displayFakeTower = true;
	}
	else
	{
		displayFakeTower = false;
	}

	return 0;
}

void Game::updateLogic()
{
	cameraMoving();
	
	spawningObjects();

	movingObjects();

	staticCollision();
	
	destroyingObjects();

	manageTime();
}

void Game::cameraMoving()
{
	int dx = mapScrolling[EKeyboard][Right] + mapScrolling[EMouse][Right] - mapScrolling[EKeyboard][Left] - mapScrolling[EMouse][Left];
	if (dx > 1) dx = 1;
	if (dx < -1) dx = -1;

	int dy = mapScrolling[EKeyboard][Down] + mapScrolling[EMouse][Down] - mapScrolling[EKeyboard][Up] - mapScrolling[EMouse][Up];
	if (dy > 1) dy = 1;
	if (dy < -1) dy = -1;

	int scrollingSpeed = 3;

	for (int i = 0; i < scrollingSpeed / zoom * 240.f / framerate; i++)
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
	if (nMinionsSpawned < 10)
	{
		if (isPaused)
			clock.restart();
		else
		{	
			time += seconds(timeScale[timeIndex] / frameTimeQuotient * clock.restart().asSeconds());
			float spawnInterval = 2.f - 0.00005f * float(wave) * float(wave);
			if (time.asSeconds() >= spawnInterval || minions.size() == 0) // 2 seconds spawn interval and decreasing each wave
			{
				minions.push_back(new Minion(this));
				nMinionsSpawned++;
				time = seconds(0.f);
			}
		}
	}
	else if (minions.size() == 0)
	{
		nMinionsSpawned = 0;
		wave++;

		Text animText;

		animText.setFont(font);
		animText.setFillColor(Color(100, 10, 100, 255));
		animText.setScale(scale, scale);
		animText.setCharacterSize(96);

		animText.setString("NOICE! NEXT WAVE!!!");
		animText.setOrigin(Vector2f(animText.getLocalBounds().width / 2.f, animText.getLocalBounds().height / 2.f));
		animText.setPosition(0.5f*1920.f*scale, 0.2f*1080.f*scale);

		float direction = rand()%360;
		float rotation = 0.2f - (rand()%5)/10.f;
		float velocity = 0.5f;
		float duration = 3.f;

		textAnimations.push_back(new TextAnimation(animText, direction, rotation, velocity, scale, duration));
	}

	for (auto &tower : towers)
		tower->fire();
}

void Game::spendMoney(int type)
{
	int index = selectedStatus - 1; // FreeCell = 0, OccupiedCell = 1

	int cost[2] = { Tower::TowerCost[type], Tower::TowerUpgradeCost[type] };
	bool maxUpgraded = false;

	if (dollars >= cost[index])
	{
		if (index == 0)
		{
			towers.push_back(new Tower(this, type));
		}
		else
		{		
			Tower* t = nullptr;
			for (auto &tower : towers)
			{
				if (tower->sprite.getPosition() == Vector2f((selectedTile.x + 0.5f) * scale * tileSize, (selectedTile.y + 0.5f) * scale * tileSize))
				{
					t = tower;
					break;
				}
			}

			maxUpgraded = t->upgrades[type] < Tower::TowerUpgradeLimit[t->type][type] ? false : true;

			if (maxUpgraded == false)
			{
				t->upgrades[type]++;

				switch (type)
				{
				case Damage:
					t->damage = Tower::TowerDamage[t->type] + t->upgrades[type] * 2;
					break;
				case ArmorPenetration:
					t->armorPenetration = Tower::TowerArmorPenetration[t->type] + t->upgrades[type];
					break;
				case FireRate:
					t->fireRate = Tower::TowerFireRate[t->type] * float(12 - t->upgrades[type])/12.f;
					break;
				case FireRange:
					t->fireRange = Tower::TowerFireRange[t->type] + t->upgrades[type] * 16;
					break;
				}
			}
		}

		if(maxUpgraded == false)
			dollars -= cost[index];

		selectedStatus = OccupiedCell;
		displayFakeTower = false;
	}
}

void Game::movingObjects()
{
	if (!isPaused)
	{
		for (auto &minion : minions)
		{
			minion->move();
			minion->sprite.setTextureRect(IntRect(minion->type * 64, minion->animation->getFrame(isPaused) * 64, 64, 64));
		}

		for (auto &projectile : projectiles)
		{
			projectile->move();
			projectile->sprite.setTextureRect(IntRect(projectile->type * 64, projectile->animation->getFrame(isPaused) * 64, 64, 64));
		}
	}

	
	for (auto &anim : textAnimations)
	{
		anim->update(isPaused);
	}

	if (!isPaused)
	{
		for (auto &tower : towers)
		{
			tower->sprite.setTextureRect(IntRect(tower->type * 64, 0, 64, 64));
		}
	}
}

void Game::staticCollision()
{
	for (auto &m1 : minions)
	{
		for (auto &m2 : minions)
		{
			if (m1->id != m2->id && (m1->lives && m2->lives) && isColliding(m1, m2))
			{
				Vector2f pos1 = m1->sprite.getPosition();
				Vector2f pos2 = m2->sprite.getPosition();

				float distance = sqrtf((pos1.x - pos2.x)*(pos1.x - pos2.x) + (pos1.y - pos2.y)*(pos1.y - pos2.y));

				if (distance != 0.f)
				{
					float overlap = 0.5f * (distance - tileSize / 64.f * scale * (m1->radius + m2->radius));

					Vector2f pos1copy = pos1;

					pos1copy.x -= overlap * (pos1.x - pos2.x) / distance;
					pos1copy.y -= overlap * (pos1.y - pos2.y) / distance;

					pos2.x -= overlap * (pos2.x - pos1.x) / distance;
					pos2.y -= overlap * (pos2.y - pos1.y) / distance;

					m1->sprite.setPosition(pos1copy);
					m2->sprite.setPosition(pos2);
				}
			}
		}
	}
}

bool Game::isColliding(Minion* m1, Minion* m2)
{
	Vector2f pos1 = m1->sprite.getPosition();
	Vector2f pos2 = m2->sprite.getPosition();

	float distance = sqrtf((pos1.x - pos2.x)*(pos1.x - pos2.x) + (pos1.y - pos2.y)*(pos1.y - pos2.y));
	float twoTimesRadius = tileSize/64.f * scale * (m1->radius + m2->radius);

	return distance < twoTimesRadius;
}

void Game::loadGraphicsToWindow()
{
	window->setView(view);

	printTiles();

	for (auto &tower : towers)
	{
		tower->sprite.setTextureRect(IntRect(tower->type * 64, 0, 64, 64));
		window->draw(tower->sprite);	
	}

	if (displayFakeTower)
	{
		window->draw(fakeTower->sprite);
		fakeTower->showRange();
	}

	for (auto &projectile : projectiles)
	{
		projectile->sprite.setTextureRect(IntRect(projectile->type * 64, projectile->animation->getFrame(isPaused) * 64, 64, 64));
		window->draw(projectile->sprite);
	}

	for (auto &minion : minions)
	{
		minion->sprite.setTextureRect(IntRect(minion->type * 64, minion->animation->getFrame(isPaused) * 64, 64, 64));
		window->draw(minion->sprite);
		minion->showHpBar();
	}

	for (auto &anim : textAnimations)
	{
		window->draw(anim->text);
	}

	// selected tower range visualization
	if (selectedStatus == OccupiedCell)
	{
		for (auto &tower : towers)
		{
			if (tower->sprite.getPosition() == Vector2f((selectedTile.x + 0.5f) * scale * tileSize, (selectedTile.y + 0.5f) * scale * tileSize))
			{
				tower->showRange();
				break;
			}
		}
	}

	window->setView(window->getDefaultView());


	printInterface();

	if (isPaused)
	{
		window->draw(pauseRect);
		window->draw(pauseText);
	}
}

void Game::printInterface()
{
	window->draw(gameInterface);

	Text interfaceText;

	interfaceText.setFont(font);
	interfaceText.setScale(scale, scale);
	interfaceText.setCharacterSize(60);
	interfaceText.setFillColor(Color(56, 56, 56, 255));

	Vector2f startingPoint(0.85f*scale*1920.f + scale * 12.f, 0.1f*scale*1080.f - 0.25f*60.f*scale);
	interfaceText.setPosition(startingPoint);

	Vector2f offset(0.f, 0.1213f*scale*1080.f);

	int value[] = { dollars, wave, livesLeft, 10 + minions.size() - nMinionsSpawned };

	for (int i = 0; i < 4; i++)
	{
		interfaceText.setString(std::to_string(value[i]));
		window->draw(interfaceText);
		interfaceText.move(offset);
	}

	printButtons();
}

void Game::printButtons()
{
	if (selectedStatus == Path)
		return;

	int index = selectedStatus - 1; // FreeCell = 0, OccupiedCell = 1

	int loopCount[2] = { 2, 4 };
	Texture* texture[2] = {&towerTexture, &upgradeTexture};

	RectangleShape towerButtonBase;
	towerButtonBase.setFillColor(Color(200, 200, 20, 150));

	towerButtonBase.setSize(Vector2f(0.1f*1920.f, 0.1f*1080.f));
	towerButtonBase.setScale(scale, scale);

	towerButtonBase.setOutlineThickness(2.f);
	towerButtonBase.setOutlineColor(Color::Black);

	Sprite s(*(texture[index]));
	s.setScale(tileSize / 64.f * scale, tileSize / 64.f * scale);

	if (selectedStatus == OccupiedCell)
	{
		RectangleShape sellButton;

		Vector2f pos = { 0.05f*scale*1920.f, 0.85f*scale*1080.f };

		sellButton.setPosition(pos);
		sellButton.setSize(Vector2f(0.1f*scale*1080.f, 0.1f*scale*1080.f));

		sellButton.setOutlineThickness(2.f*scale);
		sellButton.setFillColor(Color(180, 0, 0, 255));
		sellButton.setOutlineColor(Color::Black);

		window->draw(sellButton);

		Text sellText;
		sellText.setFont(font);
		sellText.setScale(scale, scale);

		sellText.setFillColor(Color::Black);
		sellText.setPosition(pos + Vector2f(0.f, 16.f*scale));

		sellText.setCharacterSize(30);
		Tower* t = nullptr;
		for (auto &tower : towers)
		{
			if (tower->sprite.getPosition() == Vector2f((selectedTile.x + 0.5f) * scale * tileSize, (selectedTile.y + 0.5f) * scale * tileSize))
			{
				t = tower;
				break;
			}
		}
		sellText.setString(" Sell for\n  " + std::to_string(t->getRefund()) + "$");

		window->draw(sellText);

	}

	for (int i = 0; i < loopCount[index]; i++)
	{
		int buyCost[2] = {Tower::TowerCost[i], Tower::TowerUpgradeCost[i]};

		Vector2f pos(Vector2f((0.1f*float(i) + 0.15f)*scale*1920.f, 0.85f*scale*1080.f));

		towerButtonBase.setPosition(pos);
		window->draw(towerButtonBase);

		s.setTextureRect(IntRect(i * 64, 0, 64, 64));
		s.setPosition(pos + Vector2f(5.f*scale, 0.f));
		window->draw(s);

		Text cost;

		cost.setFont(font);
		cost.setScale(scale, scale);
		cost.setCharacterSize(36);
		cost.setFillColor(Color::Black);
		cost.setString(std::to_string(buyCost[index]) + "$");
		cost.setPosition(pos + Vector2f(107.f*scale, 25.f*scale));

		window->draw(cost);

		Text key;

		key.setFont(font);
		key.setScale(scale, scale);
		key.setCharacterSize(30);
		key.setFillColor(Color::Black);

		Tower* t = nullptr;
		for (auto &tower : towers)
		{
			if (tower->sprite.getPosition() == Vector2f((selectedTile.x + 0.5f) * scale * tileSize, (selectedTile.y + 0.5f) * scale * tileSize))
			{
				t = tower;
				break;
			}
		}

		key.setString("[" + std::to_string(i+1) + "]" + (selectedStatus == OccupiedCell ? "         " + std::to_string(int(t->upgrades[i]))+"/"+std::to_string(Tower::TowerUpgradeLimit[t->type][i]) : ""));
		key.setPosition(pos + Vector2f(10.f*scale, 69.f*scale));

		window->draw(key);

		RectangleShape rect;
		rect.setScale(scale, scale);

		rect.setSize(Vector2f(0.1f*1920.f, 0.1f*1080.f));
		rect.setPosition(pos);

		if (selectedStatus == OccupiedCell && t->upgrades[i] == Tower::TowerUpgradeLimit[t->type][i])
		{
			rect.setFillColor(Color(0, 255, 0, 80));
			window->draw(rect);

		}
		else if (dollars < buyCost[index])
		{
			rect.setFillColor(Color(0, 0, 0, 150));
			window->draw(rect);
		}
	}
	
}

void Game::printTiles()
{
	Sprite tile;
	tile.setTexture(tileTexture);
	tile.setScale(tileSize/64.f * scale, tileSize/64.f * scale);
	/*
	float offsetX = tileSize / 2.f, offsetY = tileSize / 2.f;
	Color color[2] = { {165, 132, 33, 255}, {100, 100, 100, 255} };
	float outlineThickness[2] = { 0.f, 1.f };
	
			bool tileType = map->boolGrid[i][j];

			float rectSize = tileSize - 2 * outlineThickness[tileType];
			RectangleShape rect(Vector2f(rectSize, rectSize));
			rect.setScale(scale, scale);

			float posX = scale * (offsetX  + j * tileSize);
			float posY = scale * (offsetY  + i * tileSize);
			
			rect.setPosition(posX, posY);
			rect.setOrigin(rectSize / 2.f, rectSize / 2.f);

			rect.setOutlineColor(Color::Black);
			rect.setOutlineThickness(outlineThickness[tileType]);

			rect.setFillColor(color[tileType]);

			window->setView(view);
			window->draw(rect);
	*/

	for (unsigned int i = 0; i < map->size.y; i++)
	{
		for (unsigned int j = 0; j < map->size.x; j++)
		{
			tile.setTextureRect(IntRect(map->boolGrid[i][j] * 64, 0, 64, 64));

			float posX = j * tileSize;
			float posY = i * tileSize;

			tile.setPosition(Vector2f(posX, posY) * scale);

			window->setView(view);
			window->draw(tile);
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
}

void Game::destroyingObjects()
{
	int vectorSize = minions.size();
	for (int i = 0; i < vectorSize; i++)
	{
		if (minions[i]->lives == false)
		{
			bool killed = !minions[i]->gotToTheEnd; // false when got to the end, true if killed by a tower
			int reward = 20 + wave;

			if (killed)
			{
				dollars += reward;

				Text animText;

				animText.setFont(font);
				animText.setFillColor(Color(10, 100, 10, 255));
				animText.setScale(scale, scale);
				animText.setCharacterSize(48);

				animText.setString("+" + std::to_string(reward) + "$");
				animText.setOrigin(Vector2f(animText.getLocalBounds().width / 2.f, animText.getLocalBounds().height / 2.f));
				animText.setPosition(minions[i]->sprite.getPosition());


				float upDirection = -M_PI / 2.f;
				float rotation = 0.f;
				float velocity = 0.4f;
				float duration = 1.f;

				textAnimations.push_back(new TextAnimation(animText, upDirection, rotation, velocity, scale, duration));
			}
			else	
				livesLeft--;

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

	vectorSize = towers.size();
	for (int i = 0; i < vectorSize; i++)
	{
		if (towers[i]->sold)
		{
			delete towers[i];
			towers.erase(towers.begin() + i);
			i--;
			vectorSize--;
		}
	}

	vectorSize = textAnimations.size();
	for (int i = 0; i < vectorSize; i++)
	{
		if (textAnimations[i]->finished())
		{
			delete textAnimations[i];
			textAnimations.erase(textAnimations.begin() + i);
			i--;
			vectorSize--;
		}
	}
}

void Game::manageTime()
{
	frameTimeQuotient = debugClock.restart().asSeconds() * framerate;

	for (int i = 0; i < 3; i++)
		timeScale[i] = frameTimeQuotient * baseTimeScale[i];
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

	delete fakeTower;

	delete map;
}