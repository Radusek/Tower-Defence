#include "Game.h"
#include <iostream>
#include <iomanip>

#include "Tower.h"
#include "Minion.h"
#include "Projectile.h"
#include "TextAnimation.h"
#include "Animation.h"

#include "Map.h"
#include "App.h"

Game::Game(App& app0) : RenderingScene(app0), wave(1), dollars(120), livesLeft(20), nMinionsSpawned(0), minionId(0), mapScrolling{}, selectedTile(0, 0), selectedStatus(Path), displayFakeTower(false), zoom(1.f), isPaused(false), timeIndex(0), timeScale{ 1.f, 1.5f, 2.25f }, spawnLimit(10), level{}, infoIndex(UNSET)
{
	std::srand(std::time(NULL));

	setView();

	map = new Map();
	int mapNumber = rand() % MAPS;
	level[mapNumber] = true;
	map->load(mapNumber);

	tGameInterface.loadFromFile("img/gameInterface.png");
	gameInterface.setTexture(tGameInterface);
	gameInterface.setScale(scale, scale);

	towerTexture.loadFromFile("img/towerTexture.png");
	upgradeTexture.loadFromFile("img/upgradeTexture.png");
	minionTexture.loadFromFile("img/minionTexture.png");
	projectileTexture.loadFromFile("img/projectileTexture.png");

	fakeTower = new Tower(this, 0);

	fakeTower->sprite.setColor(FAKE_TOWER_COLOR);

	pauseRect.setSize(Vector2f(BASE_WIDTH, BASE_HEIGHT) * VIEW_PART * scale);
	pauseRect.setFillColor(Color(0, 0, 0, 140));

	pauseText = Text("PAUSE", font, 72);
	pauseText.setScale(scale, scale);
	pauseText.setPosition(Vector2f(BASE_WIDTH, BASE_HEIGHT) * VIEW_PART/2.f * scale);

	pauseText.setOrigin(pauseText.getLocalBounds().width / 2, pauseText.getLocalBounds().height / 2 + 30);

	clock.restart();
}

void Game::setView()
{
	Vector2f viewSize(Vector2f(window->getSize()) * VIEW_PART);

	view = View(FloatRect(0.f, 0.f, viewSize.x, viewSize.y));
	view.setViewport(sf::FloatRect(0.f, 0.f, VIEW_PART, VIEW_PART));
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
			if (map->size.x * tileSize >= BASE_WIDTH*VIEW_PART*zoom*1.25f - 0.1f && map->size.y * tileSize >= BASE_HEIGHT* VIEW_PART*zoom*1.25f - 0.1f)
			{
				view.zoom(1.25f);
				zoom *= 1.25f;

				Vector2f centerPos = view.getCenter();
				
				if (view.getCenter().x + view.getSize().x / 2.f > scale*float(map->size.x)*tileSize)
					centerPos.x = scale * (float(map->size.x)*tileSize) - view.getSize().x / 2.f;
	
				else if (view.getCenter().x - view.getSize().x / 2.f < 0.f)
					centerPos.x = view.getSize().x / 2.f;

				
				if (view.getCenter().y + view.getSize().y / 2.f > scale*float(map->size.y)*tileSize)
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
		
		if (selectedStatus > Path && event.key.code >= Keyboard::Num1 && event.key.code <= Keyboard::Num4)
		{
			int purchaseChoice = event.key.code - Keyboard::Num1;

			if (purchaseChoice > 1 && selectedStatus == FreeCell)
				return;

			spendMoney(purchaseChoice);
		}

		if (event.key.code == Keyboard::P)
			isPaused = !isPaused;

		if (event.key.code == Keyboard::T)
			++timeIndex %= TIME_MODES;
	}

	if (event.type == sf::Event::KeyReleased)
	{
		if (event.key.code >= sf::Keyboard::Left && event.key.code <= sf::Keyboard::Down)
			mapScrolling[EKeyboard][event.key.code - sf::Keyboard::Left] = false;
	}
}

void Game::leftMouseClick(Vector2i pos)
{
	if (pos.x < VIEW_PART * scale * BASE_WIDTH && pos.y < VIEW_PART * scale * BASE_HEIGHT) // klikniêto w obszar planszy
	{
		Vector2f clickedPoint = window->mapPixelToCoords(pos, view);
		
		selectedTile = Vector2i(clickedPoint / (scale*tileSize));

		if (map->boolGrid[selectedTile.y][selectedTile.x] == 0)
		{
			selectedStatus = Path;
			return;
		}
		else
			selectedStatus = OccupiedCell;

		Vector2f newPosition((Vector2f(selectedTile) + Vector2f(0.5f, 0.5f)) * scale * float(tileSize));
		for (auto &tower : towers)
			if (tower->sprite.getPosition() == newPosition)
				return;

		selectedStatus = FreeCell;
	}
	else if (selectedStatus != Path && pos.y >= BUTTON_TOP * scale && pos.y < BUTTON_DOWN * scale
			&& pos.x < BUTTON_N_X * scale * BASE_WIDTH && pos.x >= BUTTON_0_X * scale * BASE_WIDTH) // wybor towera
	{
		int purchaseChoice = pos.x - BUTTON_0_X * scale * BASE_WIDTH;
		purchaseChoice /= BUTTON_SIZE * scale * BASE_WIDTH;

		if (purchaseChoice > 1 && selectedStatus == FreeCell)
			return;

		spendMoney(purchaseChoice);
	}

	if (selectedStatus == OccupiedCell && pos.y >= scale * SELL_BUTTON_Y && pos.y < scale * (SELL_BUTTON_Y + SELL_SIZE)
		&& pos.x < scale * (SELL_BUTTON_X + SELL_SIZE) && pos.x >= scale * SELL_BUTTON_X)
	{
		Tower* t = nullptr;
		t = getSelectedTower();
		t->sellTower();
	}
}

int Game::checkCursorPosition(Vector2i pos)
{
	mapScrolling[EMouse][Left] = pos.x < SCREEN_EDGE * scale * BASE_WIDTH;
	mapScrolling[EMouse][Right] = pos.x >= (1.f - SCREEN_EDGE) * scale * BASE_WIDTH;

	mapScrolling[EMouse][Up] = pos.y < SCREEN_EDGE * scale * BASE_HEIGHT;
	mapScrolling[EMouse][Down] = pos.y >= (1.f - SCREEN_EDGE) * scale * BASE_HEIGHT;

	displayFakeTower = false;
	infoIndex = UNSET;

	if (selectedStatus > Path && pos.y >= BUTTON_TOP * scale && pos.y < BUTTON_DOWN * scale
		&& pos.x < scale * (BUTTON_0_X + UpgradesCount * BUTTON_SIZE) * BASE_WIDTH && pos.x >= scale * BUTTON_0_X * BASE_WIDTH)
	{
		int buttonNumber = pos.x - BUTTON_0_X * scale * BASE_WIDTH;
		buttonNumber /= BUTTON_SIZE * scale * BASE_WIDTH;

		if (selectedStatus == FreeCell && buttonNumber > 1)
			return -1;

		if (selectedStatus == FreeCell)
		{
			fakeTower->type = buttonNumber;
			fakeTower->fireRange = Tower::TowerFireRange[buttonNumber];

			fakeTower->sprite.setTextureRect(IntRect(buttonNumber * TSIZE, 0, TSIZE, TSIZE));
			fakeTower->sprite.setPosition((Vector2f(selectedTile) + Vector2f(0.5f, 0.5f)) * scale * float(tileSize));

			displayFakeTower = true;
		}

		infoIndex = buttonNumber;
	}
	
	return 0;
}

void Game::changeMap()
{
	map->free();

	while(projectiles.size())
	{
		delete projectiles[0];
		projectiles.erase(projectiles.begin());
	}

	while (towers.size())
	{
		delete towers[0];
		towers.erase(towers.begin());
	}

	while (textAnimations.size())
	{
		delete textAnimations[0];
		textAnimations.erase(textAnimations.begin());
	}

	wave = 1;
	dollars = 120;
	livesLeft += 5;
	selectedStatus = Path;

	view.setCenter(view.getSize() / 2.f);

	int unusedMaps = 0;
	for (int i = 0; i < MAPS; i++)
		unusedMaps += 1 - level[i];
	

	if (unusedMaps == 0)
	{
		for (int i = 0; i < MAPS; i++)
			level[i] = false;

		unusedMaps = MAPS;
	}
	
	int nextMap = rand() % unusedMaps;

	for (int i = 0; i < MAPS; i++)
	{
		if (nextMap == 0 && level[i] == false)
		{
			map->load(i);
			level[i] = true;
			break;
		}
		
		if(level[i] == false)
			nextMap -= 1 - level[i];
	}
}

void Game::updateLogic()
{
	manageTime();

	cameraMoving();
	
	spawningObjects();

	movingObjects();

	staticCollision();
	
	destroyingObjects();
}

void Game::cameraMoving()
{
	int dx = mapScrolling[EKeyboard][Right] + mapScrolling[EMouse][Right] - mapScrolling[EKeyboard][Left] - mapScrolling[EMouse][Left];
	if (std::abs(dx) > 1) 
		dx /= std::abs(dx);

	int dy = mapScrolling[EKeyboard][Down] + mapScrolling[EMouse][Down] - mapScrolling[EKeyboard][Up] - mapScrolling[EMouse][Up];
	if (std::abs(dy) > 1)
		dy /= std::abs(dy);

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
	if (isPaused)
		return;

	if (nMinionsSpawned < spawnLimit)
	{
		spawnTime += seconds(timeScale[timeIndex] * frameTime.asSeconds());
		float spawnInterval = 2.f - 0.00005f * float(wave) * float(wave); // 2 seconds spawn interval and decreasing each wave
		if (spawnTime.asSeconds() >= spawnInterval || minions.size() == 0)
		{
			minions.push_back(new Minion(this));
			nMinionsSpawned++;
			spawnTime = seconds(0.f);
		}
	}
	else if (minions.size() == 0)
	{
		nMinionsSpawned = 0;
		wave++;

		if (wave == 31)
			changeMap();

		Text animText("NOICE! NEXT WAVE!!!", font, 96);

		animText.setFillColor(Color(100, 10, 100, 255));
		animText.setScale(scale, scale);

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
			towers.push_back(new Tower(this, type));
		else
		{		
			Tower* t = nullptr;
			t = getSelectedTower();

			maxUpgraded = t->upgrades[type] < Tower::TowerUpgradeLimit[t->type][type] ? false : true;

			if (maxUpgraded == false)
			{
				t->upgrades[type]++;

				switch (type)
				{
				case Damage:
					t->damage += Tower::TowerDamageIncrease[t->type];
					break;
				case ArmorPenetration:
					t->armorPenetration += Tower::TowerArmorPenetrationIncrease[t->type];
					break;
				case FireRate:
					t->fireRate -= Tower::TowerFireRateIncrease[t->type];
					break;
				case FireRange:
					t->fireRange += Tower::TowerFireRangeIncrease[t->type];
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
	if (isPaused)
		return;

	for (auto &minion : minions)
		minion->move();

	for (auto &projectile : projectiles)
		projectile->move();

	for (auto &anim : textAnimations)
		anim->update(this);
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
					float overlap = 0.5f * (distance - tileSize / TSIZE_F * scale * (m1->radius + m2->radius));

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
	float twoTimesRadius = tileSize/TSIZE_F * scale * (m1->radius + m2->radius);

	return distance < twoTimesRadius;
}

void Game::loadGraphicsToWindow()
{
	window->setView(view);

	printTiles();

	for (auto &tower : towers)
	{
		tower->sprite.setTextureRect(IntRect(tower->type * TSIZE, 0, TSIZE, TSIZE));
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

	if (selectedStatus == OccupiedCell)
	{
		Tower* t = nullptr;
		t = getSelectedTower();
		t->showRange();
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

	int fontSize = 60;
	Text interfaceText("", font, fontSize);

	interfaceText.setScale(scale, scale);
	interfaceText.setFillColor(INTERFACE_TEXT_COLOR);

	Vector2f startPoint(Vector2f(0.85f*BASE_WIDTH +  12.f, 0.1f*BASE_HEIGHT - 0.25f*fontSize) * scale);
	interfaceText.setPosition(startPoint);

	Vector2f offset(0.f, TEXT_Y_OFFSET * scale * BASE_HEIGHT);

	int value[] = { dollars, wave, livesLeft, 10 + minions.size() - nMinionsSpawned };

	for (auto val : value)
	{
		interfaceText.setString(std::to_string(val));
		window->draw(interfaceText);
		interfaceText.move(offset);
	}

	printButtons();
	printInfo();
}

void Game::printButtons()
{
	if (selectedStatus == Path)
		return;

	int index = selectedStatus - 1; // FreeCell = 0, OccupiedCell = 1

	int loopCount[2] = { 2, 4 };
	Texture* texture[2] = {&towerTexture, &upgradeTexture};

	RectangleShape towerButtonBase(Vector2f(BASE_WIDTH, BASE_HEIGHT) * 0.1f * scale);
	towerButtonBase.setOutlineThickness(2.f * scale);

	towerButtonBase.setFillColor(BUTTON_COLOR);
	towerButtonBase.setOutlineColor(Color::Black);

	Sprite s(*(texture[index]));
	s.setScale(Vector2f(1.f, 1.f) * float(tileSize) / TSIZE_F * scale);

	if (selectedStatus == OccupiedCell)
	{
		RectangleShape sellButton(Vector2f(BASE_HEIGHT, BASE_HEIGHT) * 0.1f * scale);
		sellButton.setOutlineThickness(2.f * scale);

		Vector2f pos(Vector2f(0.05f * BASE_WIDTH, 0.85f * BASE_HEIGHT) * scale);
		sellButton.setPosition(pos);

		sellButton.setFillColor(SELL_BUTTON_COLOR);
		sellButton.setOutlineColor(Color::Black);

		window->draw(sellButton);

		Tower* t = nullptr;
		t = getSelectedTower();

		Text sellText(" Sell for\n  " + std::to_string(t->getRefund()) + "$", font, 24);
		sellText.setScale(scale, scale);

		sellText.setFillColor(Color::Black);
		sellText.setPosition(pos + Vector2f(0.f, 16.f*scale));

		window->draw(sellText);
	}

	for (int i = 0; i < loopCount[index]; i++)
	{
		int buyCost[2] = {Tower::TowerCost[i], Tower::TowerUpgradeCost[i]};

		Vector2f pos(Vector2f((0.1f*i + 0.15f)*BASE_WIDTH, 0.85f*BASE_HEIGHT) * scale);
		towerButtonBase.setPosition(pos);

		window->draw(towerButtonBase);

		s.setTextureRect(IntRect(i * TSIZE, 0, TSIZE, TSIZE));
		s.setPosition(pos + Vector2f(5.f*scale, 0.f));

		window->draw(s);

		Text cost(std::to_string(buyCost[index]) + "$", font, 32);

		cost.setScale(scale, scale);
		cost.setFillColor(Color::Black);
		cost.setPosition(pos + Vector2f(102.f, 25.f) * scale);

		window->draw(cost);

		Tower* t = nullptr;
		t = getSelectedTower();

		String str = "[" + std::to_string(i + 1) + "]" + (selectedStatus == OccupiedCell ? "     " + std::to_string(int(t->upgrades[i])) + "/" + std::to_string(Tower::TowerUpgradeLimit[t->type][i]) : ""); // XD

		Text key(str, font, 24);

		key.setScale(scale, scale);
		key.setFillColor(Color::Black);

		key.setPosition(pos + Vector2f(3.f, 75.f) * scale);

		window->draw(key);

		RectangleShape rect(Vector2f(BASE_WIDTH, BASE_HEIGHT) * 0.1f * scale);
		rect.setPosition(pos);

		if (selectedStatus == OccupiedCell && t->upgrades[i] == Tower::TowerUpgradeLimit[t->type][i])
		{
			rect.setFillColor(FULL_UPGRADE_COLOR);
			window->draw(rect);
		}
		else if (dollars < buyCost[index])
		{
			rect.setFillColor(NO_MONEY_COLOR);
			window->draw(rect);
		}
	}
}

void Game::printInfo()
{
	Vector2f pos0(Vector2f(0.83f * BASE_WIDTH, 0.6f * BASE_HEIGHT) * scale);

	switch (selectedStatus)
	{
	case Path:
		printPathInfo(pos0);
		break;

	case FreeCell:
		printFreeCellInfo(pos0);
		break;

	case OccupiedCell:
		printOccupiedCellInfo(pos0);
		break;
	}
}

void Game::printPathInfo(Vector2f pos0)
{
	Sprite s(minionTexture);
	s.setScale(Vector2f(1.f, 1.f) * scale * float(tileSize) / TSIZE_F);
	s.setPosition(pos0);

	fakeMinionTime += frameTime;
	int frame = int(fakeMinionTime.asSeconds()) % 2;

	s.setTextureRect(IntRect(0, frame * TSIZE, TSIZE, TSIZE));

	window->draw(s);

	int valCount = 2;
	int values[] = {Minion::getWaveHp(wave), Minion::getWaveArmor(wave)};
	String valueDescription[] = { "HP: ", "Armor: " };

	Text text("", font, 20);
	text.setScale(scale, scale);
	text.setPosition(pos0 + Vector2f(0.06f * BASE_WIDTH, 0.01f * BASE_HEIGHT) * scale);
	text.setFillColor(Color::Black);

	for (int i = 0; i < valCount; i++)
	{
		text.setString(valueDescription[i] + std::to_string(values[i]));
		window->draw(text);
		text.move(Vector2f(0.f, 0.04f * BASE_HEIGHT) * scale);
	}
}

void Game::printFreeCellInfo(Vector2f pos0)
{
	if (infoIndex == UNSET)
		return;

	int valCount = 4;
	int values[] = { Tower::TowerDamage[infoIndex], Tower::TowerArmorPenetration[infoIndex], 1000 * Tower::TowerFireRate[infoIndex], Tower::TowerFireRange[infoIndex] };
	String valueDescription[] = { "Damage: ", "Penetration: ", "Reload time: ", "Range: " };

	Text text("", font, 20);
	text.setScale(scale, scale);
	text.setPosition(pos0);
	text.setFillColor(Color::Black);

	for (int i = 0; i < valCount; i++)
	{
		text.setString(valueDescription[i] + std::to_string(values[i]) + (i == 2 ? " ms" : ""));
		window->draw(text);
		text.move(Vector2f(0.f, 0.04f * BASE_HEIGHT) * scale);
	}
}

void Game::printOccupiedCellInfo(Vector2f pos0)
{
	Tower* t = nullptr;
	t = getSelectedTower();

	int valCount = 4;
	int values[] = { t->damage, t->armorPenetration, 1000 * t->fireRate, t->fireRange };
	String valueDescription[] = { "Damage: ", "Penetration: ", "Reload time: ", "Range: " };

	Text text("", font, 20);
	text.setScale(scale, scale);
	text.setPosition(pos0);
	text.setFillColor(Color::Black);

	bool displayChangeInfo = false;

	int newValues[] = { values[0] + Tower::TowerDamageIncrease[t->type],
						values[1] + Tower::TowerArmorPenetrationIncrease[t->type],
						values[2] - 1000 * Tower::TowerFireRateIncrease[t->type],
						values[3] + Tower::TowerFireRangeIncrease[t->type] };

	int newValue = 0;

	if (infoIndex != UNSET && int(Tower::TowerUpgradeLimit[t->type][infoIndex]) != int(t->upgrades[infoIndex]))
	{
		displayChangeInfo = true;
		newValue = newValues[infoIndex];
	}

	for (int i = 0; i < valCount; i++)
	{
		text.setString(valueDescription[i] + std::to_string(values[i]) + (displayChangeInfo && i == infoIndex ? "->" + std::to_string(newValue) : "") + (i == 2 ? " ms" : ""));
		window->draw(text);
		text.move(Vector2f(0.f, 0.04f * BASE_HEIGHT) * scale);
	}

	String dpsVal = std::to_string(int(10 * t->damage / t->fireRate));
	String digit = dpsVal[dpsVal.getSize() - 1];
	dpsVal.erase(dpsVal.getSize() - 1);
	dpsVal += "." + digit;

	float newDps = 0.f;
	if (infoIndex == 0)
		newDps = (float(t->damage) + Tower::TowerDamageIncrease[t->type]) / t->fireRate;
	else if (infoIndex == 2)
		newDps = t->damage / (t->fireRate - Tower::TowerFireRateIncrease[t->type]);

	String newDpsString = std::to_string(int(10 * (newDps - t->damage / t->fireRate)));
	digit = newDpsString[newDpsString.getSize() - 1];
	newDpsString.erase(newDpsString.getSize() - 1);
	newDpsString += "." + digit;

	text.setString("DPS: " + dpsVal + (displayChangeInfo && (infoIndex == 0 || infoIndex == 2) ? " (+"+ newDpsString + ")" : ""));
	window->draw(text);	
}

void Game::printTiles()
{
	Sprite tile(tileTexture);
	tile.setScale(Vector2f(1.f, 1.f) * float(tileSize) / TSIZE_F * scale);

	for (unsigned int i = 0; i < map->size.y; i++)
	{
		for (unsigned int j = 0; j < map->size.x; j++)
		{
			tile.setTextureRect(IntRect(map->boolGrid[i][j] * TSIZE, 0, TSIZE, TSIZE));

			Vector2f pos(Vector2f(j, i) * float(tileSize) * scale);
			tile.setPosition(pos);

			window->draw(tile);
		}
	}

	if (selectedStatus == Path) 
		return;

	// highlight selected tile
	RectangleShape selection(Vector2f(1.f, 1.f) * scale * float(tileSize));
	selection.setPosition(Vector2f(selectedTile) * scale * float(tileSize));

	selection.setFillColor(SELECTION_COLOR);

	window->draw(selection);
}

void Game::destroyingObjects()
{
	for (int i = 0; i < minions.size(); i++)
	{
		if (minions[i]->lives == false)
		{
			bool killed = !minions[i]->gotToTheEnd; // false when got to the end, true if killed by a tower
			int reward = 10 + wave*0.8f;

			if (killed)
			{
				dollars += reward;

				Text animText("+" + std::to_string(reward) + "$", font, REWARD_TEXT_SIZE);

				animText.setFillColor(REWARD_TEXT_COLOR);
				animText.setScale(scale, scale);

				animText.setOrigin(animText.getLocalBounds().width / 2.f, animText.getLocalBounds().height / 2.f);
				animText.setPosition(minions[i]->sprite.getPosition());
				
				float upDirection = -M_PI / 2.f;
				float rotation = 0.f;
				float velocity = 0.4f;
				float duration = 1.f;

				textAnimations.push_back(new TextAnimation(animText, upDirection, rotation, velocity, scale, duration));
			}
			else if (--livesLeft <= 0)
				nextScene = EMenu;

			

			delete minions[i];
			minions.erase(minions.begin() + i);
			i--;
		}
	}

	for (int i = 0; i < projectiles.size(); i++)
	{
		if (projectiles[i]->hit)
		{
			delete projectiles[i];
			projectiles.erase(projectiles.begin() + i);
			i--;
		}
	}

	for (int i = 0; i < towers.size(); i++)
	{
		if (towers[i]->sold)
		{
			delete towers[i];
			towers.erase(towers.begin() + i);
			i--;
		}
	}

	for (int i = 0; i < textAnimations.size(); i++)
	{
		if (textAnimations[i]->finished())
		{
			delete textAnimations[i];
			textAnimations.erase(textAnimations.begin() + i);
			i--;
		}
	}
}

void Game::manageTime()
{
	frameTime = clock.restart();
}

Tower* Game::getSelectedTower()
{
	for (auto &tower : towers)
	{
		Vector2f pos((Vector2f(selectedTile) + Vector2f(0.5f, 0.5f)) * scale * float(tileSize));
		if (tower->sprite.getPosition() == pos)		
			return tower;
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

	delete fakeTower;

	delete map;
}