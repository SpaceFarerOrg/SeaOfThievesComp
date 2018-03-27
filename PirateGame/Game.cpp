#include "Game.h"
#include <SFML\Graphics\RenderWindow.hpp>
#include <SFML\Window\Event.hpp>
#include <SFML\System\Vector2.hpp>
#include "Math.h"
#include "Application.h"
#include "Network.h"
#include <iostream>
#include "TextureBank.h"
#include "AudioSystem.h"

void CGame::SetWindow(sf::RenderWindow * aWindow)
{
	myWindow = aWindow;
}

void CGame::Init()
{
	myHasRegisteredPaus = false;

	myPlayerActionPrompt.Init("Error", 0.5f);

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
	myShip.Respawn();
	myWorld.Generate(myUIMap);
	myShip.SetPosition(myWorld.GetSpawnPosition());

	CAudioSystem::GetInstance().StopAllMusic();

	CAudioSystem::GetInstance().RandomizeSongBetween(EMusic::BgMusicOne, EMusic::BgMusicTwo);
}

bool CGame::Update()
{
	if (CAudioSystem::GetInstance().GetTimeSilent() > 5.f)
	{
		CAudioSystem::GetInstance().RandomizeSongBetween(EMusic::BgMusicOne, EMusic::BgMusicTwo, true);
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

	myShip.Update(dt);

	EPlayerAction possibleAction = myWorld.CheckPlayerWorldInteraction(myShip);
	RespondToPlayerAction(possibleAction);

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
		myPlayerActionPrompt.Render();
	}
	else
	{
		if (CAudioSystem::GetInstance().NoMusicPlaying())
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
	otherPlayerName.setFont(CTextureBank::GetInstance().GetFont());

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

void CGame::LoadMapFromServer(const SMap& aMap)
{
	myShouldSendCloseToWinning = true;

	myWhirlwinds.clear();

	myWorld.CreateFromGeneratedMap(aMap, myUIMap);
}

void CGame::RespondToPlayerAction(EPlayerAction aPlayerAction)
{
	switch (aPlayerAction)
	{
	case EPlayerAction::None:
		myPlayerActionPrompt.Hide();
		break;
	case EPlayerAction::Loot:
		myPlayerActionPrompt.SetString("Press space to loot");
		myPlayerActionPrompt.Display();
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space))
		{
			myWorld.LootTreasure();
			myShip.SetHoldsTreasure(true);
			CAudioSystem::GetInstance().PlaySound(ESound::Loot);
		}

		break;
	case EPlayerAction::Sell:
		myPlayerActionPrompt.SetString("Press space to sell");
		myPlayerActionPrompt.Display();

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space))
		{
			myWorld.PlaceTreasure(myUIMap);
			myShip.SetHoldsTreasure(false);
			myTreasury.GiveGold(100);
			CAudioSystem::GetInstance().PlaySound(ESound::Sell);
		}

		break;
	case EPlayerAction::Crash:
		if (!myShip.GetIsSinking())
		{
			CAudioSystem::GetInstance().PlaySound(ESound::ShipCrash);
			myShip.Sink();
		}
		break;
	case EPlayerAction::OutsideWorldBounds:
		myPlayerActionPrompt.SetString("You are leaving chartered waters, turn back!");
		myPlayerActionPrompt.Display();
		break;
	default:
		break;
	}
}

void CGame::SetWinner(const sf::String & aName)
{
	CAudioSystem::GetInstance().PlayMusic(EMusic::WinMusic);

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
	CAudioSystem::GetInstance().PlayMusic(EMusic::StressMusic);

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
