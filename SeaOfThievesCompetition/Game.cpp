#include "Game.h"
#include <SFML\Graphics\RenderWindow.hpp>
#include <SFML\Window\Event.hpp>
#include <SFML\System\Vector2.hpp>
#include "Math.h"
#include "Application.h"
#include "Network.h"
#include <iostream>
#include "TextureBank.h"

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

	myWinMusic.openFromFile("audio/winSong.ogg");

	myBackgroundSound.openFromFile("audio/bgSound.ogg");
	myBackgroundSound.setLoop(true);
	myBackgroundSound.play();

	myStressMusic.openFromFile("audio/stress.ogg");
	myStressMusic.setLoop(true);

	myHasRegisteredPaus = false;

	myShip.Init();
	myShip.SetWavesTextures(GET_TEXTURE(ETexture::ShipWavesBig), GET_TEXTURE(ETexture::ShipWavesBig));

	myShipSprite.setTexture(GET_TEXTURE(ETexture::Ship));
	myShipSprite.setOrigin(myShipSprite.getGlobalBounds().width / 2.f, myShipSprite.getGlobalBounds().height / 2.f);

	for (auto& p : myWhirlwindBuffer)
	{
		p.first.Init(GET_TEXTURE(ETexture::Whirlwind));
	}

	myUIMap.Init();

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
	myWorld.Generate(myUIMap);
	myShip.SetPosition(myWorld.GetSpawnPosition());
	myWinMusic.stop();

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


	if (myHasRegisteredPaus && myTimeWithNoMusic >= 15.f  && myStressMusic.getStatus() != sf::SoundSource::Status::Playing && myWinMusic.getStatus() != sf::SoundSource::Status::Playing)
	{
		RandomizeSong();
		myHasRegisteredPaus = false;
	}

	if (myShip.GetHasRespawned())
	{
		myTreasury.GiveGold(-100);
		myWorld.PlaceTreasure(myUIMap);
	}

	float dt = myDeltaTimer.getElapsedTime().asSeconds();
	myDeltaTimer.restart();

	dt = dt > 1.f ? 1.f : dt;

	myTimeWithNoMusic += dt;


	myCamera.setSize((float)myWindow->getSize().x, (float)myWindow->getSize().y);
	myWindow->setView(myCamera);

	UpdateWhirlwinds(dt);

	myWorld.Render();

	myBirdSpawner.Update(dt, myWindow->getView().getCenter());

	CheckShipCollisionVsIslands();

	myShip.Update(dt);

	myShip.Render();

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
		CNetworking::GetInstance().Disconnect();
		CApplication::EnterMenu();
	}
	if (myShip.GetIsDead())
	{
		myShip.Respawn();
		myShip.SetPosition(myWorld.GetSpawnPosition());
	}

	if (!CNetworking::GetInstance().GetIsClient())
	{
		mySpawnNewWWTimer += dt;
		if (mySpawnNewWWTimer >= 10.f)
		{
			sf::Vector2f pos = myWorld.GetRandomSeaPosition();
			PlaceWhirlwind(pos);
			CNetworking::GetInstance().SendWhirlwindSpawn(pos);
			mySpawnNewWWTimer = 0.f;
		}
	}

	if (myTreasury.GetGold() == 700 && myShouldSendWinning)
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
	else if (myTreasury.GetGold() == 500)
	{
		SetCloseToWinning(myShouldSendCloseToWinning);
	}


	mySomeoneIsCloseToWinningTimer += dt;
	if (!myPlayerHasWon)
	{

		myPlayerCloseToWinning.Render();

		ShowPressButtonPrompt();
		EnsurePlayerKeepingOnMap(dt);
	}
	else
	{
		if (myWinMusic.getStatus() == sf::Music::Status::Stopped)
		{
			CApplication::EnterMenu();
			CNetworking::GetInstance().Disconnect();
		}

		myPlayerWon.Render();
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

	sf::Vector2f nudge;

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

			sf::Vector2f shipToShip = myShip.GetPosition() - other.myTransform.getPosition();
			float distance = Math::Length(shipToShip);

			if (distance <= myShip.GetWidth())
			{
				Math::Normalize(shipToShip);
				if (distance == 0.f)
				{
					distance = 0.01f;
				}

				float maxDistance = myShip.GetWidth();
				float percentage = distance / maxDistance;

				nudge += shipToShip * 600.f * (1.f - percentage);
			}

			myWindow->draw(otherPlayerName);
		}
	}

	myShip.SetShipNudge(nudge);
}

void CGame::GenerateWorld()
{
	myShouldSendCloseToWinning = true;


	myShip.Respawn();

	myWorld.Generate(myUIMap);
	myTreasury.SetGold(0);
	myWorld.PlaceTreasure(myUIMap);
	CNetworking::GetInstance().SetMap(myWorld.GetRawMap());
}

void CGame::LoadMapFromServer(const std::array<int, MAP_AXIS_SIZE*MAP_AXIS_SIZE>& aMap)
{
	myShouldSendCloseToWinning = true;

	myWhirlwinds.clear();

	myWorld.CreateFormArray(aMap, myUIMap);
}

void CGame::RandomizeSong()
{
	short songToStart = Math::GetRandomInRange(0, (short)myBackgroundSongs.size() - 1);

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

void CGame::CheckShipCollisionVsIslands()
{

	myPlayerCanLoot = false;
	myPlayerCanSell = false;

	const std::array<sf::Vector2f, 4>& cp = myShip.GetCollisionPoints();

	std::vector<CIsland>& myIslands = myWorld.GetIslands();

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
						myWorld.PlaceTreasure(myUIMap);
						myCashSound.play();
					}
				}
			}
		}
	}
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
	myWinMusic.setVolume(volume);
}

void CGame::SetWinner(const sf::String & aName)
{
	if (myWinMusic.getStatus() != sf::Music::Status::Playing)
	{
		myStressMusic.stop();

		for (sf::Music& song : myBackgroundSongs)
		{
			song.stop();
		}

		myWinMusic.play();
	}

	myPlayerWon.SetString(aName + " became a pirate legend first!");
	myPlayerWon.Display();
	myPlayerHasWon = true;
}

void CGame::ShowSomeoneCloseToWinningText(const sf::String & aName)
{
	mySomeoneIsCloseToWinningTimer = 0;

	myPlayerCloseToWinning.Display();

	SetCloseToWinning();
}

void CGame::SetCloseToWinning(bool aShouldSend)
{
	if (myStressMusic.getStatus() != sf::Music::Status::Playing)
	{
		for (sf::Music& song : myBackgroundSongs)
		{
			song.stop();
		}

		myStressMusic.setVolume(100);
		myStressMusic.play();
	}

	if (CNetworking::GetInstance().GetIsNetworkEnabled() && aShouldSend && !myHasSentClosedToWinning)
	{
		CNetworking::GetInstance().SendWinning(true);
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
