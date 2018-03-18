#include "Game.h"
#include <SFML\Graphics\RenderWindow.hpp>
#include <SFML\Window\Event.hpp>
#include <SFML\System\Vector2.hpp>
#include "Math.h"
#include "Application.h"
#include "Network.h"
#include <iostream>

void CGame::SetWindow(sf::RenderWindow * aWindow)
{
	myWindow = aWindow;
}

void CGame::Init()
{
	myFont.loadFromFile("font/font.ttf");
	myPressSpaceToLoot.setFont(myFont);
	myYouAreOutsideOfMap.setFont(myFont);

	myCashSoundBuffer.loadFromFile("audio/earnMoney.ogg");
	myCashSound.setBuffer(myCashSoundBuffer);
	myCashSound.setVolume(45);

	myBackgroundMusic.openFromFile("audio/song.ogg");
	myBackgroundMusic.setLoop(true);
	//myBackgroundMusic.play();

	myBackgroundSound.openFromFile("audio/bgSound.ogg");
	myBackgroundSound.setLoop(true);
	//myBackgroundSound.play();

		/*
		Map Legend
		1 - Spawn position
		2 - Gold Island
		3 - Island
		*/

	myMap =
	{
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,2,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,1,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0
	};

	for (size_t i = 0; i < myMap.size(); ++i)
	{
		if (myMap[i] == GOLD_ISLAND)
		{
			myGoldIslandIndexInMap = i;
		}
		else if (myMap[i] == SPAWN_POSITION)
		{
			mySpawnPointIndex = i;
		}
	}

	LoadTextures();

	myShip.Init(myTextureBank[(size_t)ETexture::Ship]);
	myShip.SetWavesTextures(myTextureBank[(size_t)ETexture::ShipWavesBig], myTextureBank[(size_t)ETexture::ShipWavesBig]);

	myUIMap.Init(myTextureBank[(size_t)ETexture::Map], myTextureBank[(size_t)ETexture::MapIsland], myTextureBank[(size_t)ETexture::MapIslandTwo], myTextureBank[(size_t)ETexture::MapIslandThree],myTextureBank[(size_t)ETexture::MapGoldIsland], myTextureBank[(size_t)ETexture::Cross], myMap);

	myShipSprite.setTexture(myTextureBank[(size_t)ETexture::Ship]);
	myShipSprite.setOrigin(myShipSprite.getGlobalBounds().width / 2.f, myShipSprite.getGlobalBounds().height / 2.f);

	for (auto& p : myWhirlwindBuffer)
	{
		p.first.Init(myTextureBank[(size_t)ETexture::Whirlwind]);
	}
	myNextAvailibleWW = 0;
	mySpawnNewWWTimer = 0.f;

	myShouldRun = true;

	myBirdSpawner.Init();
}

void CGame::Update()
{
	float dt = myDeltaTimer.getElapsedTime().asSeconds();
	myDeltaTimer.restart();

	dt = dt > 1.f ? 1.f : dt;

	myCamera.setSize(myWindow->getSize().x, myWindow->getSize().y);
	myWindow->setView(myCamera);

	for (CAnimation& wave : myWaves)
	{
		wave.Update(dt);
		wave.Render(*myWindow);
	}

	UpdateWhirlwinds(dt);

	for (CIsland& island : myIslands)
	{
		island.Render(*myWindow);
	}

	myBirdSpawner.Update(dt, myWindow->getView().getCenter());

	CheckShipCollisionVsIslands();

	myShip.Update(dt);
	myShip.Render(*myWindow);

	DisplayOtherShips();

	myTreasury.Render(*myWindow);

	myBirdSpawner.Render(myWindow);

	ShowPressButtonPrompt();
	EnsurePlayerKeepingOnMap(dt);

	float mapAlpha = myUIMap.GetAlpha();

	if (myShip.GetIsControlled())
	{
		mapAlpha = Math::Lerp(mapAlpha, 0.75f, dt * 4.f);
	}
	else
	{
		mapAlpha = Math::Lerp(mapAlpha, 1.0f, dt * 4.f);
	}

	myUIMap.SetAlpha(mapAlpha);

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::M))
	{
		myUIMap.Render(*myWindow);
	}

	myCamera.setCenter(myShip.GetPosition().x, myShip.GetPosition().y);

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape) || myShip.GetIsDead())
	{
		CApplication::EnterMenu();
	}

	if (!CNetworking::GetInstance().GetIsClient())
	{
		mySpawnNewWWTimer += dt;
		if (mySpawnNewWWTimer >= 10.f)
		{
			sf::Vector2f pos = GetWhirlwindSpawnPos();
			PlaceWhirlwind(pos);
			CNetworking::GetInstance().SendWhirlwindSpawn(pos);
			mySpawnNewWWTimer = 0.f;
		}

	}

	if (myTreasury.GetGold() == 1000)
	{
		//Win game
	}
	else if (myTreasury.GetGold() == 800)
	{
		//Push Pirate is Close to winning
	}
}

void CGame::DisplayOtherShips()
{
	const std::vector<SClient> otherShips = CNetworking::GetInstance().GetPlayerList();
	sf::Text otherPlayerName;
	otherPlayerName.setFont(myFont);

	for (const SClient& other : otherShips)
	{
		myShipSprite.setPosition(other.myTransform.getPosition());
		myShipSprite.setRotation(other.myTransform.getRotation());

		myWindow->draw(myShipSprite);

		otherPlayerName.setString(other.myName);
		otherPlayerName.setOrigin(otherPlayerName.getGlobalBounds().width / 2.f, otherPlayerName.getGlobalBounds().height / 2.f);
		otherPlayerName.setPosition(other.myTransform.getPosition().x, other.myTransform.getPosition().y - myShipSprite.getGlobalBounds().height / 2.f);

		myWindow->draw(otherPlayerName);
	}
}

void CGame::GenerateWorld()
{
	ClearMapFromIslands();

	myShip.Respawn();

	myTreasury.SetGold(0);
	myIslands.clear();
	myWaves.clear();
	CreateWorld();
	PlaceTreasure();
	myUIMap.SetMap(myMap);
	CNetworking::GetInstance().SetMap(myMap);
}

void CGame::LoadMapFromServer(const std::array<int, MAP_AXIS_SIZE*MAP_AXIS_SIZE>& aMap)
{
	myIslands.clear();
	myWhirlwinds.clear();
	ClearMapFromIslands();

	myMap = aMap;

	for (size_t i = 0; i < myMap.size(); ++i)
	{
		ETexture islandTexture;
		if (myMap[i] == ISLAND_1)
		{
			islandTexture = ETexture::Island;
			myIslands.push_back(CIsland());
			myIslands.back().Init(myTextureBank[(size_t)islandTexture], TranslateMapPointToWorldPosition(i));
		}
		if (myMap[i] == ISLAND_2)
		{
			islandTexture = ETexture::IslandTwo;
			myIslands.push_back(CIsland());
			myIslands.back().Init(myTextureBank[(size_t)islandTexture], TranslateMapPointToWorldPosition(i));
		}
		if (myMap[i] == ISLAND_3)
		{
			islandTexture = ETexture::IslandThree;
			myIslands.push_back(CIsland());
			myIslands.back().Init(myTextureBank[(size_t)islandTexture], TranslateMapPointToWorldPosition(i));
		}
		if (myMap[i] == GOLD_ISLAND)
		{
			myIslands.push_back(CIsland());
			myIslands.back().Init(myTextureBank[(size_t)ETexture::GoldIsland], TranslateMapPointToWorldPosition(i), true);
			myGoldIslandIndex = i;
			myGoldIslandIndexInMap = i;
		}
		if (i == mySpawnPointIndex)
		{
			myShip.SetPosition(TranslateMapPointToWorldPosition(mySpawnPointIndex));
		}
	}
	CreateWaves();
	myUIMap.SetMap(myMap);
}

void CGame::ShowPressButtonPrompt()
{
	if (myShip.GetIsSinking())
	{
		return;
	}

	if (myPlayerCanLoot)
	{
		myPressSpaceToLoot.setString("Stop & Press Space To Loot");
	}
	else if (myPlayerCanSell)
	{
		myPressSpaceToLoot.setString("Stop & Press Space To Sell");
	}

	if (myPlayerCanLoot || myPlayerCanSell)
	{
		myPressSpaceToLoot.setOutlineColor(sf::Color::Black);
		myPressSpaceToLoot.setOutlineThickness(5.f);
		myPressSpaceToLoot.setCharacterSize(60);
		myPressSpaceToLoot.setOrigin(myPressSpaceToLoot.getGlobalBounds().width / 2.f, myPressSpaceToLoot.getGlobalBounds().height / 2.f);
		myPressSpaceToLoot.setPosition(myCamera.getCenter());

		myWindow->draw(myPressSpaceToLoot);
	}
}

void CGame::CreateWorld()
{
	myMap[myGoldIslandIndexInMap] = GOLD_ISLAND;
	myIslands.push_back(CIsland());
	myIslands.back().Init(myTextureBank[(size_t)ETexture::GoldIsland], TranslateMapPointToWorldPosition(myGoldIslandIndexInMap), true);
	myGoldIslandIndex = myIslands.size() - 1;

	for (size_t i = 0; i < myMap.size(); ++i)
	{
		if (myMap[i] == SPAWN_POSITION)
		{
			myShip.SetPosition(TranslateMapPointToWorldPosition(i));
			mySpawnPointIndex = i;
		}
	}

	CreateIslands();
	CreateWaves();


}

void CGame::CheckShipCollisionVsIslands()
{

	myPlayerCanLoot = false;
	myPlayerCanSell = false;

	const std::array<sf::Vector2f, 4>& cp = myShip.GetCollisionPoints();

	for (size_t i = 0; i < myIslands.size(); ++i)
	{
		for (const sf::Vector2f& pos : cp)
		{
			if (myIslands[i].IsColliding(pos))
			{
				myShip.Sink();
			}
			else if (myIslands[i].HasTreasure())
			{
				if (myIslands[i].IsInLootingRange(pos))
				{
					myPlayerCanLoot = true;
					if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space) && myShip.GetIsStill())
					{
						myIslands[i].Loot();
						myShip.SetHoldsTreasure(true);
					}
				}
			}
			else if (myIslands[i].IsGoldIsland() && myShip.GetHasTreasure())
			{
				if (myIslands[i].IsInLootingRange(pos))
				{
					myPlayerCanSell = true;
					if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space) && myShip.GetIsStill())
					{
						myShip.SetHoldsTreasure(false);
						myTreasury.GiveGold(100);
						PlaceTreasure();
						myCashSound.play();
					}
				}
			}
		}
	}
}

void CGame::PlaceTreasure()
{
	short islandToGetTreasure = myGoldIslandIndex;

	while (islandToGetTreasure == myGoldIslandIndex)
	{
		islandToGetTreasure = Math::GetRandomInRange(0, myIslands.size() - 1);
	}

	myUIMap.SetTreasureIsland(myIslands[islandToGetTreasure].GetIndexInMap());
	myIslands[islandToGetTreasure].SetHasTreasure();
}

void CGame::EnsurePlayerKeepingOnMap(float aDT)
{
	float mapMin = 0.f;
	float mapMax = MAP_CHUNK_SIZE * MAP_AXIS_SIZE;
	float forgiveness = 200.f;
	myIsOutsideOfMap = false;

	sf::Vector2f shipPos = myShip.GetPosition();

	if (myShip.GetPosition().x > mapMax +forgiveness || myShip.GetPosition().y > mapMax +forgiveness || myShip.GetPosition().x < mapMin - forgiveness || myShip.GetPosition().y < mapMin - forgiveness || myShip.GetIsSinking())
	{
		myIsOutsideOfMap = true;

		myYouAreOutsideOfMap.setCharacterSize(60);

		if (myShip.GetIsSinking())
		{
			myYouAreOutsideOfMap.setString("You rest with Davy Jones!");
		}
		else
		{
			myYouAreOutsideOfMap.setString("You are entering no Pirate's Sea. Turn back!");
		}

		myYouAreOutsideOfMap.setOutlineThickness(5.f);
		myYouAreOutsideOfMap.setOutlineColor(sf::Color::Black);
		myYouAreOutsideOfMap.setOrigin(myYouAreOutsideOfMap.getGlobalBounds().width / 2.f, myYouAreOutsideOfMap.getGlobalBounds().height / 2.f);
		myYouAreOutsideOfMap.setPosition(myCamera.getCenter());
		myWindow->draw(myYouAreOutsideOfMap);
	}

	if (myIsOutsideOfMap)
	{
		myIsOutsideOfMapTimer += aDT;

		if (myIsOutsideOfMapTimer >= 5.0f)
		{
			myShip.Sink();
		}
	}
	else
	{
		myIsOutsideOfMapTimer = 0.f;
	}
}

sf::Vector2f CGame::GetWhirlwindSpawnPos()
{
	while (true)
	{
		size_t indexInMap = Math::GetRandomInRange(0, myMap.size() - 1);

		if (myMap[indexInMap] == SEA)
		{
			sf::Vector2f pos = TranslateMapPointToWorldPosition(indexInMap);

			return std::move(pos);
		}
	}
}

sf::Vector2f CGame::TranslateMapPointToWorldPosition(size_t aMapIndex)
{
	int x = aMapIndex % MAP_AXIS_SIZE;
	int y = aMapIndex / MAP_AXIS_SIZE;

	float worldX = (float)x * MAP_CHUNK_SIZE;
	float worldY = (float)y * MAP_CHUNK_SIZE;

	sf::Vector2f returnPos = { worldX, worldY };

	return std::move(returnPos);
}

void CGame::LoadTextures()
{
	myTextureBank[(size_t)ETexture::Ship].loadFromFile("sprites/playerShip.png");
	myTextureBank[(size_t)ETexture::Waves].loadFromFile("sprites/waterWaves.png");
	myTextureBank[(size_t)ETexture::ShipWavesBig].loadFromFile("sprites/shipWavesBig.png");
	myTextureBank[(size_t)ETexture::GoldIsland].loadFromFile("sprites/goldIsland.png");
	myTextureBank[(size_t)ETexture::Cross].loadFromFile("sprites/cross.png");

	myTextureBank[(size_t)ETexture::Island].loadFromFile("sprites/island1.png");
	myTextureBank[(size_t)ETexture::IslandTwo].loadFromFile("sprites/island2.png");
	myTextureBank[(size_t)ETexture::IslandThree].loadFromFile("sprites/island3.png");

	myTextureBank[(size_t)ETexture::Whirlwind].loadFromFile("sprites/whirlwind.png");

	myTextureBank[(size_t)ETexture::Map].loadFromFile("sprites/map.png");
	myTextureBank[(size_t)ETexture::MapIsland].loadFromFile("sprites/mapIsland.png");
	myTextureBank[(size_t)ETexture::MapIslandTwo].loadFromFile("sprites/mapIsland2.png");
	myTextureBank[(size_t)ETexture::MapIslandThree].loadFromFile("sprites/mapIsland3.png");
	myTextureBank[(size_t)ETexture::MapGoldIsland].loadFromFile("sprites/mapGoldIsland.png");

	for (size_t i = 0; i < myTextureBank.size(); ++i)
	{
		myTextureBank[i].setSmooth(true);
	}
}

void CGame::ClearMapFromIslands()
{
	for (int& pointInMap : myMap)
	{
		if (pointInMap != SPAWN_POSITION && pointInMap != SEA)
		{
			pointInMap = 0;
		}
	}
}

void CGame::CreateIslands()
{
	size_t numberOfIsles = Math::GetRandomInRange(MIN_ISLAND_COUNT, MAX_ISLAND_COUNT);

	for (size_t i = 0; i < numberOfIsles; ++i)
	{
		bool placedIsland = false;

		while (!placedIsland)
		{
			size_t indexInMap = Math::GetRandomInRange(0, myMap.size() - 1);

			if (indexInMap != mySpawnPointIndex && indexInMap != myGoldIslandIndex && myMap[indexInMap] < ISLAND_1)
			{
				size_t texture = Math::GetRandomInRange(0, 2);

				size_t islandType = ISLAND_1 + texture;

				size_t firstIsle = (size_t)ETexture::Island;

				firstIsle += texture;

				myMap[indexInMap] = islandType;
				myIslands.push_back(CIsland());
				myIslands.back().Init(myTextureBank[firstIsle], TranslateMapPointToWorldPosition(indexInMap));
				myIslands.back().SetIndexInMap(indexInMap);

				placedIsland = true;
			}

		}

	}

}

void CGame::CreateWaves()
{
	for (size_t i = 0; i < myMap.size(); ++i)
	{
		if (myMap[i] != ISLAND && i != myGoldIslandIndex)
		{
			myWaves.push_back(CAnimation());
			myWaves.back().Init(myTextureBank[(size_t)ETexture::Waves], 600, 0.25f);
			myWaves.back().RandomizeStartFrame();
			myWaves.back().SetPosition(TranslateMapPointToWorldPosition(i));
		}
	}
}

void CGame::PlaceWhirlwind(const sf::Vector2f& aPosition)
{
	auto& wwToPlace = myWhirlwindBuffer[myNextAvailibleWW];

	wwToPlace.first.SetShouldFade(false);
	wwToPlace.first.SetPosition(aPosition);
	wwToPlace.second = 0.f;

	++myNextAvailibleWW;

	myNextAvailibleWW %= myWhirlwindBuffer.size();
}

void CGame::UpdateWhirlwinds(float aDT)
{
	sf::Vector2f drag;

	bool shouldKillPlayer = false;

	for (size_t i = 0; i < myWhirlwindBuffer.size(); ++i)
	{
		myWhirlwindBuffer[i].first.Update(aDT);
		myWhirlwindBuffer[i].second += aDT;

		if (myWhirlwindBuffer[i].second >= WHIRLWIND_STAY_TIME)
		{
			myWhirlwindBuffer[i].first.SetShouldFade(true);
			
			if (myWhirlwindBuffer[i].first.GetCanChangePosition())
			{
				myWhirlwindBuffer[i].first.SetPosition(TRASH_PILE);
			}
		}


		drag += myWhirlwindBuffer[i].first.GetDragTo(myShip.GetPosition(), shouldKillPlayer);

		myWhirlwindBuffer[i].first.Render(*myWindow);
	}

	if (shouldKillPlayer)
	{
		myShip.Sink();
	}

	myShip.SetWhirlwindDrag(drag);
}

bool CGame::GetShouldRun() const
{
	return myShouldRun;
}
