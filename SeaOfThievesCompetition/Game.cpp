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
	myPlayerCloseToWinning.setFont(myFont);
	myPlayerCloseToWinning.setFillColor(sf::Color::Transparent);

	myWinningPlayer.setFont(myFont);

	myCashSoundBuffer.loadFromFile("audio/earnMoney.ogg");
	myCashSound.setBuffer(myCashSoundBuffer);
	myCashSound.setVolume(45);

	myTreasureThump.loadFromFile("audio/treasureThump.ogg");
	myTreasureSound.setBuffer(myTreasureThump);
	myTreasureSound.setVolume(45);

	myCrash.loadFromFile("audio/shipCrash.ogg");
	myCrashSound.setBuffer(myCrash);
	myCrashSound.setVolume(45);

	myBackgroundSongs[0].openFromFile("audio/song.ogg");
	myBackgroundSongs[0].setLoop(false);

	myBackgroundSongs[1].openFromFile("audio/song2.ogg");
	myBackgroundSongs[0].setLoop(false);

	myBackgroundSound.openFromFile("audio/bgSound.ogg");
	myBackgroundSound.setLoop(true);
	myBackgroundSound.play();

	myStressMusic.openFromFile("audio/stress.ogg");
	myStressMusic.setLoop(true);

	myHasRegisteredPaus = false;



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

	myUIMap.Init(myTextureBank[(size_t)ETexture::Map], myTextureBank[(size_t)ETexture::MapIsland], myTextureBank[(size_t)ETexture::MapIslandTwo], myTextureBank[(size_t)ETexture::MapIslandThree], myTextureBank[(size_t)ETexture::MapGoldIsland], myTextureBank[(size_t)ETexture::Cross], myMap);

	myShipSprite.setTexture(myTextureBank[(size_t)ETexture::Ship]);
	myShipSprite.setOrigin(myShipSprite.getGlobalBounds().width / 2.f, myShipSprite.getGlobalBounds().height / 2.f);

	for (auto& p : myWhirlwindBuffer)
	{
		p.first.Init(myTextureBank[(size_t)ETexture::Whirlwind]);
	}
	myNextAvailableWW = 0;
	mySpawnNewWWTimer = 0.f;

	myShouldSendWinning = true;

	myPlayerHasWon = false;

	myShouldRun = true;

	myBirdSpawner.Init();
}

void CGame::ReInit()
{
	myTreasury.SetGold(0);
	myShouldSendWinning = true;
	myHasRegisteredPaus = false;
	myPlayerHasWon = false;
	myPlayerCanLoot = false;
	myPlayerCanSell = false;
	myHasRegisteredPaus = false;
	myStressMusic.stop();
	myShip.Respawn();

	for (sf::Music& song : myBackgroundSongs)
	{
		song.stop();
	}

	RandomizeSong();
}

bool CGame::Update()
{

	if (myBackgroundSongs[myActiveSong].getStatus() == sf::SoundSource::Stopped && !myHasRegisteredPaus && myStressMusic.getStatus() != sf::SoundSource::Status::Playing)
	{
		myHasRegisteredPaus = true;
		myTimeWithNoMusic = 0.f;
	}


	if (myHasRegisteredPaus && myTimeWithNoMusic >= 15.f  && myStressMusic.getStatus() != sf::SoundSource::Status::Playing)
	{
		RandomizeSong();
		myHasRegisteredPaus = false;
	}

	if (myShip.GetHasRespawned())
	{
		myTreasury.GiveGold(-100);
		PlaceTreasure();
	}

	float dt = myDeltaTimer.getElapsedTime().asSeconds();
	myDeltaTimer.restart();

	dt = dt > 1.f ? 1.f : dt;

	myTimeWithNoMusic += dt;

	myUIMap.SetTreasureIsland(myIslands[myTreasureIsland].GetIndexInMap());

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

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape))
	{
		CApplication::EnterMenu();
	}
	if (myShip.GetIsDead())
	{
		myShip.Respawn();
		myShip.SetPosition(TranslateMapPointToWorldPosition(mySpawnPointIndex));
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

	if (myTreasury.GetGold() == 1000 && myShouldSendWinning)
	{
		myPlayerHasWon = true;

		if (CNetworking::GetInstance().GetIsNetworkEnabled())
		{

			//Win game
			CNetworking::GetInstance().SendWinning(false);

		}

		SetWinner("You ");
		myShouldSendWinning = false;
	}
	else if (myTreasury.GetGold() == 800)
	{
		SetCloseToWinning(myShouldSendCloseToWinning);
	}


	mySomeoneIsCloseToWinningTimer += dt;
	if (!myPlayerHasWon)
	{
		myPlayerCloseToWinning.setPosition(myWindow->getView().getCenter().x, myWindow->getView().getCenter().y - myPlayerCloseToWinning.getGlobalBounds().height * 2);

		myWindow->draw(myPlayerCloseToWinning);

		if (mySomeoneIsCloseToWinningTimer >= 5.f)
		{
			myPlayerCloseToWinning.setFillColor(sf::Color::Transparent);
			myPlayerCloseToWinning.setOutlineColor(sf::Color::Transparent);
		}

		ShowPressButtonPrompt();
		EnsurePlayerKeepingOnMap(dt);
	}
	else
	{
		myTimeWinnerShow += dt;

		if (myTimeWinnerShow >= 6.f)
		{
			CApplication::EnterMenu();
			CNetworking::GetInstance().Disconnect();
		}

		myWinningPlayer.setCharacterSize(50);
		myWinningPlayer.setFillColor(sf::Color::White);
		myWinningPlayer.setOrigin(myWinningPlayer.getGlobalBounds().width / 2.f, myWinningPlayer.getGlobalBounds().height / 2.f);
		myWinningPlayer.setPosition(myCamera.getCenter());
		myWinningPlayer.setOutlineColor(sf::Color::Black);
		myWinningPlayer.setOutlineThickness(2.f);
		myWindow->draw(myWinningPlayer);
	}
	return true;
}

void CGame::DisplayOtherShips()
{
	std::vector<SClient> otherShips = CNetworking::GetInstance().GetPlayerList();

	if (otherShips.size() > 0)
	{
		otherShips.erase(otherShips.begin() + CNetworking::GetInstance().GetSelfInClientList());
	}

	sf::Text otherPlayerName;
	otherPlayerName.setFont(myFont);

	for (const SClient& other : otherShips)
	{
		if (other.myConnected)
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
}

void CGame::GenerateWorld()
{
	myShouldSendCloseToWinning = true;

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
	myShouldSendCloseToWinning = true;

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
			myIslands.back().SetIndexInMap(i);
		}
		if (myMap[i] == ISLAND_2)
		{
			islandTexture = ETexture::IslandTwo;
			myIslands.push_back(CIsland());
			myIslands.back().Init(myTextureBank[(size_t)islandTexture], TranslateMapPointToWorldPosition(i));
			myIslands.back().SetIndexInMap(i);
		}
		if (myMap[i] == ISLAND_3)
		{
			islandTexture = ETexture::IslandThree;
			myIslands.push_back(CIsland());
			myIslands.back().Init(myTextureBank[(size_t)islandTexture], TranslateMapPointToWorldPosition(i));
			myIslands.back().SetIndexInMap(i);
		}
		if (myMap[i] == GOLD_ISLAND)
		{
			myIslands.push_back(CIsland());
			myIslands.back().Init(myTextureBank[(size_t)ETexture::GoldIsland], TranslateMapPointToWorldPosition(i), true);
			myGoldIslandIndex = myIslands.size() - 1;
			myGoldIslandIndexInMap = i;
			myIslands.back().SetIndexInMap(i);
		}
		if (i == mySpawnPointIndex)
		{
			myShip.SetPosition(TranslateMapPointToWorldPosition(mySpawnPointIndex));
		}
	}
	myUIMap.SetMap(myMap);
	CreateWaves();
	PlaceTreasure();

}

void CGame::RandomizeSong()
{
	short songToStart = Math::GetRandomInRange(0, myBackgroundSongs.size() - 1);

	myBackgroundSongs[songToStart].play();

	myActiveSong = songToStart;
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
				if (!myShip.GetIsSinking() && !myShip.GetIsInvincible())
				{
					myCrashSound.play();
				}

				myShip.Sink();
			}
			else if (myIslands[i].HasTreasure())
			{
				if (myIslands[i].IsInLootingRange(pos))
				{
					myPlayerCanLoot = true;
					if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space) && myShip.GetIsStill())
					{
						myTreasureSound.play();
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
	bool placedTreasure = false;

	short islandToGetTreasure = 0;

	while (!placedTreasure)
	{
		islandToGetTreasure = Math::GetRandomInRange(0, myIslands.size() - 1);

		if (islandToGetTreasure != myTreasureIsland && islandToGetTreasure != myGoldIslandIndex)
		{
			placedTreasure = true;
		}
	}

	myTreasureIsland = islandToGetTreasure;

	myIslands[islandToGetTreasure].SetHasTreasure();
}

void CGame::EnsurePlayerKeepingOnMap(float aDT)
{
	float mapMin = 0.f;
	float mapMax = MAP_CHUNK_SIZE * MAP_AXIS_SIZE;
	float forgiveness = 200.f;
	myIsOutsideOfMap = false;

	sf::Vector2f shipPos = myShip.GetPosition();

	if (myShip.GetPosition().x > mapMax + forgiveness || myShip.GetPosition().y > mapMax + forgiveness || myShip.GetPosition().x < mapMin - forgiveness || myShip.GetPosition().y < mapMin - forgiveness || myShip.GetIsSinking())
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

void CGame::UpdateVolumes()
{
	float volume = CApplication::GetVolume();

	for (sf::Music& song : myBackgroundSongs)
	{
		song.setVolume(volume);
	}
	myStressMusic.setVolume(volume);
	myBackgroundSound.setVolume(volume);
	myCashSound.setVolume(volume);
	myTreasureSound.setVolume(volume);
	myCrashSound.setVolume(volume);
}

void CGame::SetWinner(const sf::String & aName)
{
	myWinningPlayer.setString(aName + " became a pirate legend first!");
	myPlayerHasWon = true;
}

void CGame::ShowSomeoneCloseToWinningText(const sf::String & aName)
{
	mySomeoneIsCloseToWinningTimer = 0;

	myPlayerCloseToWinning.setCharacterSize(60);
	myPlayerCloseToWinning.setOutlineThickness(2.f);
	myPlayerCloseToWinning.setString(aName + " is close to winning!");
	myPlayerCloseToWinning.setOrigin(myPlayerCloseToWinning.getGlobalBounds().width / 2.f, myPlayerCloseToWinning.getGlobalBounds().height / 2.f);
	myPlayerCloseToWinning.setFillColor(sf::Color::White);
	myPlayerCloseToWinning.setPosition(myWindow->getView().getCenter().x, myWindow->getView().getCenter().y - myPlayerCloseToWinning.getGlobalBounds().height * 2);
	myPlayerCloseToWinning.setOutlineColor(sf::Color::Black);

	SetCloseToWinning();
}

void CGame::SetCloseToWinning(bool aShouldSend)
{
	//if (myBackgroundMusic.getVolume() > 0)
	//{
	//	myBackgroundMusic.setVolume(myBackgroundMusic.getVolume() - 1);
	//}
	//else
	//{

	if (myStressMusic.getStatus() != sf::Music::Status::Playing)
	{
		for (sf::Music& song : myBackgroundSongs)
		{
			song.stop();
		}

		myStressMusic.setVolume(100);
		myStressMusic.play();
	}
	//}


	if (CNetworking::GetInstance().GetIsNetworkEnabled() && aShouldSend && !myHasSentClosedToWinning)
	{
		myHasSentClosedToWinning = true;
		CNetworking::GetInstance().SendWinning(true);
	}
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
			myWaves.back().Init(myTextureBank[(size_t)ETexture::Waves], 600, 600, 0.25f);
			myWaves.back().RandomizeStartFrame();
			myWaves.back().SetPosition(TranslateMapPointToWorldPosition(i));
		}
	}
}

void CGame::PlaceWhirlwind(const sf::Vector2f& aPosition)
{
	auto& wwToPlace = myWhirlwindBuffer[myNextAvailableWW];

	wwToPlace.first.SetShouldFade(false);
	wwToPlace.first.SetPosition(aPosition);
	wwToPlace.second = 0.f;

	++myNextAvailableWW;

	myNextAvailableWW %= myWhirlwindBuffer.size();
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
