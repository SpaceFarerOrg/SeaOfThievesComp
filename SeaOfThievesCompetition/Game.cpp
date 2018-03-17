#include "Game.h"
#include <SFML\Graphics\RenderWindow.hpp>
#include <SFML\Window\Event.hpp>
#include <SFML\System\Vector2.hpp>
#include "Math.h"
#include "Application.h"
#include "Network.h"

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

	LoadTextures();

	GenerateWorld();

	myShip.Init(myTextureBank[(size_t)ETexture::Ship]);
	myShip.SetWavesTextures(myTextureBank[(size_t)ETexture::ShipWavesBig], myTextureBank[(size_t)ETexture::ShipWavesBig]);

	myUIMap.Init(myTextureBank[(size_t)ETexture::Map], myTextureBank[(size_t)ETexture::MapIsland], myTextureBank[(size_t)ETexture::MapGoldIsland], myTextureBank[(size_t)ETexture::Cross], myMap);

	myShipSprite.setTexture(myTextureBank[(size_t)ETexture::Ship]);
	myShipSprite.setOrigin(myShipSprite.getGlobalBounds().width / 2.f, myShipSprite.getGlobalBounds().height / 2.f);

	myShouldRun = true;

	testWW.Init(myTextureBank[(size_t)ETexture::Whirlwind]);

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

	myTargetRightOffset = 0.f;

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::M))
	{
		myUIMap.Render(*myWindow);
		myTargetRightOffset = myUIMap.GetWidth() / 2.f;
	}

	myCurrentRightOffset = Math::Lerp(myCurrentRightOffset, myTargetRightOffset, 10.f * dt);

	myCamera.setCenter(myShip.GetPosition().x - myCurrentRightOffset, myShip.GetPosition().y);

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape) || myShip.GetIsDead())
	{
		CApplication::EnterMenu();
	}
}

void CGame::DisplayOtherShips()
{
	const std::vector<SClient> otherShips = CNetworking::GetInstance().GetPlayerList();

	for (const SClient& other : otherShips)
	{
		myShipSprite.setPosition(other.myTransform.getPosition());
		myShipSprite.setRotation(other.myTransform.getRotation());

		myWindow->draw(myShipSprite);
	}
}

void CGame::GenerateWorld()
{
	myCurrentRightOffset = 0.f;

	ClearMapFromIslands();

	myShip.Respawn();

	myWhirlwinds.clear();

	myTreasury.SetGold(0);
	myIslands.clear();
	myWaves.clear();
	CreateWorld();
	PlaceTreasure();
	myUIMap.SetMap(myMap);
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
	}

	myUIMap.SetMap(myMap);
}

void CGame::ShowPressButtonPrompt()
{
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
	for (size_t i = 0; i < myMap.size(); ++i)
	{
		if (myMap[i] == GOLD_ISLAND)
		{
			myIslands.push_back(CIsland());
			myIslands.back().Init(myTextureBank[(size_t)ETexture::GoldIsland], TranslateMapPointToWorldPosition(i), true);
			myGoldIslandIndex = myIslands.size() - 1;
		}

		if (myMap[i] == SPAWN_POSITION)
		{
			myShip.SetPosition(TranslateMapPointToWorldPosition(i));
			mySpawnPointIndex = i;
		}

		if (myMap[i] == SEA)
		{
			if (Math::Chance(1))
			{
				PlaceWhirlwind();
			}
		}
	}

	CreateIslands();
	CreateWaves();

	CNetworking::GetInstance().SetMap(myMap);
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
	myIsOutsideOfMap = false;

	sf::Vector2f shipPos = myShip.GetPosition();

	if (myShip.GetPosition().x > mapMax || myShip.GetPosition().y > mapMax || myShip.GetPosition().x < mapMin || myShip.GetPosition().y < mapMin || myShip.GetIsSinking())
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
		if (pointInMap != SPAWN_POSITION && pointInMap != GOLD_ISLAND)
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
			myWaves.back().Init(myTextureBank[(size_t)ETexture::Waves], 600, 0.5f);
			myWaves.back().RandomizeStartFrame();
			myWaves.back().SetPosition(TranslateMapPointToWorldPosition(i));
		}
	}
}

void CGame::PlaceWhirlwind(int aIndex)
{
	if (aIndex == -1)
	{
		myWhirlwinds.push_back(std::pair<CWhirlwind, float>(CWhirlwind(), 0.f));
		myWhirlwinds.back().first.Init(myTextureBank[(size_t)ETexture::Whirlwind]);
	}

	auto& ww = aIndex == -1 ? myWhirlwinds.back() : myWhirlwinds[aIndex];
	ww.second = 0.f;


	bool placed = false;
	while (!placed)
	{
		int indexInMap = Math::GetRandomInRange(0, myMap.size() - 1);

		if (myMap[indexInMap] == SEA)
		{
			if (Math::Length(TranslateMapPointToWorldPosition(indexInMap) - TranslateMapPointToWorldPosition(mySpawnPointIndex)) > 700.f)
			{
				placed = true;
				ww.first.SetShouldFade(false);
				ww.first.SetPosition(TranslateMapPointToWorldPosition(indexInMap));
			}
		}

	}
}

void CGame::UpdateWhirlwinds(float aDT)
{
	sf::Vector2f drag;

	bool shouldKillPlayer = false;

	for (size_t i = 0; i < myWhirlwinds.size(); ++i)
	{
		myWhirlwinds[i].first.Update(aDT);
		myWhirlwinds[i].second += aDT;

		if (myWhirlwinds[i].second >= WHIRLWIND_STAY_TIME)
		{
			myWhirlwinds[i].first.SetShouldFade(true);

			if (myWhirlwinds[i].first.GetCanChangePosition())
			{
				PlaceWhirlwind(i);
			}

		}

		drag += myWhirlwinds[i].first.GetDragTo(myShip.GetPosition(), shouldKillPlayer);

		myWhirlwinds[i].first.Render(*myWindow);
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
