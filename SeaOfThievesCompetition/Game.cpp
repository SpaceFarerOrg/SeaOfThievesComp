#include "Game.h"
#include <SFML\Graphics\RenderWindow.hpp>
#include <SFML\Window\Event.hpp>
#include <SFML\System\Vector2.hpp>
#include "Math.h"
#include "Application.h"

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

	myBackgroundMusic.openFromFile("audio/song.ogg");
	myBackgroundMusic.setLoop(true);
	//	myBackgroundMusic.play();

	myBackgroundSound.openFromFile("audio/bgSound.ogg");
	myBackgroundSound.setLoop(true);
	//	myBackgroundSound.play();

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

	myShouldRun = true;

}

void CGame::Update()
{
	float dt = myDeltaTimer.getElapsedTime().asSeconds();
	myDeltaTimer.restart();

	myCamera.setCenter(myShip.GetPosition());
	myWindow->setView(myCamera);

	HandleWindowEvents();


	for (CAnimation& wave : myWaves)
	{
		wave.Update(dt);
		wave.Render(*myWindow);
	}

	for (CIsland& island : myIslands)
	{
		island.Render(*myWindow);
	}

	CheckShipCollisionVsIslands();

	myShip.Update(dt);
	myShip.Render(*myWindow);

	myAnimation.Update(dt);
	myAnimation.Render(*myWindow);

	myTreasury.Render(*myWindow);

	ShowPressButtonPrompt();
	EnsurePlayerKeepingOnMap(dt);

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::M))
	{
		myUIMap.Render(*myWindow);
	}

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape) || myShip.GetIsDead())
	{
		CApplication::EnterMenu();
	}


}

void CGame::GenerateWorld()
{
	for (size_t i = 0; i < myMap.size(); ++i)
	{
		if (myMap[i] == ISLAND)
		{
			myMap[i] = 0;
		}
	}

	myShip.Respawn();

	myTreasury.SetGold(0);
	myIslands.clear();
	myWaves.clear();
	CreateWorld();
	PlaceTreasure();
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
		//if (myMap[i] == ISLAND)
		//{
		//	myIslands.push_back(CIsland());
		//	myIslands.back().Init(myTextureBank[(size_t)ETexture::Island], myTextureBank[(size_t)ETexture::Cross], TranslateMapPointToWorldPosition(i));
		//}

		if (myMap[i] == GOLD_ISLAND)
		{
			myIslands.push_back(CIsland());
			myIslands.back().Init(myTextureBank[(size_t)ETexture::GoldIsland], myTextureBank[(size_t)ETexture::Cross], TranslateMapPointToWorldPosition(i), true);
			myGoldIslandIndex = myIslands.size() - 1;
		}

		if (myMap[i] == SPAWN_POSITION)
		{
			myShip.SetPosition(TranslateMapPointToWorldPosition(i));
		}

		if (myMap[i] == SEA)
		{
			if (Math::Chance(25))
			{
				myWaves.push_back(CAnimation());
				myWaves.back().Init(myTextureBank[(size_t)ETexture::Waves], 64, 0.5f);
				myWaves.back().RandomizeStartFrame();

				sf::Vector2f position = TranslateMapPointToWorldPosition(i);
				position.x += 32.f;
				position.y += 32.f;

				myWaves.back().SetPosition(position);
			}
			else if (Math::Chance(5))
			{
				myMap[i] = ISLAND;
				myIslands.push_back(CIsland());
				myIslands.back().Init(myTextureBank[(size_t)ETexture::Island], myTextureBank[(size_t)ETexture::Cross], TranslateMapPointToWorldPosition(i));
				myIslands.back().SetIndexInMap(i);
			}
		}
	}
}

void CGame::CheckShipCollisionVsIslands()
{
	sf::FloatRect shipRect = myShip.GetCollider();

	myPlayerCanLoot = false;
	myPlayerCanSell = false;

	for (size_t i = 0; i < myIslands.size(); ++i)
	{
		if (myIslands[i].IsColliding(myShip.GetPosition()))
		{
			myShip.Sink();
		}
		else if (myIslands[i].HasTreasure())
		{
			if (myIslands[i].IsInLootingRange(myShip.GetPosition()))
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
			if (myIslands[i].IsInLootingRange(myShip.GetPosition()))
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
	myTextureBank[(size_t)ETexture::Island].loadFromFile("sprites/island1.png");
	myTextureBank[(size_t)ETexture::GoldIsland].loadFromFile("sprites/goldIsland.png");
	myTextureBank[(size_t)ETexture::Cross].loadFromFile("sprites/cross.png");

	myTextureBank[(size_t)ETexture::Map].loadFromFile("sprites/map.png");
	myTextureBank[(size_t)ETexture::MapIsland].loadFromFile("sprites/mapIsland.png");
	myTextureBank[(size_t)ETexture::MapGoldIsland].loadFromFile("sprites/mapGoldIsland.png");
}

void CGame::HandleWindowEvents()
{
	sf::Event e;

	while (myWindow->pollEvent(e))
	{
		if (e.type == sf::Event::Closed)
		{
			myShouldRun = false;
		}
	}
}

bool CGame::GetShouldRun() const
{
	return myShouldRun;
}
