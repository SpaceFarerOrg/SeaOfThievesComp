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

	myPlayer.Init();
	myPlayer.GetShip().SetWavesTextures(GET_TEXTURE(ETexture::ShipWavesBig), GET_TEXTURE(ETexture::ShipWavesBig));

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
	myPlayer.FlushTreasury();
	myShouldSendWinning = true;
	myHasRegisteredPaus = false;
	myPlayerHasWon = false;
	myPlayerCanLoot = false;
	myPlayerCanSell = false;
	myHasRegisteredPaus = false;
	myWorld.Generate(myUIMap);
	myPlayer.SetSpawnPosition(myWorld.GetSpawnPosition());
	myPlayer.Respawn();

	CAudioSystem::GetInstance().StopAllMusic();

	CAudioSystem::GetInstance().RandomizeSongBetween(EMusic::BgMusicOne, EMusic::BgMusicTwo);
}

bool CGame::Update()
{
	if (CAudioSystem::GetInstance().GetTimeSilent() > 5.f)
	{
		CAudioSystem::GetInstance().RandomizeSongBetween(EMusic::BgMusicOne, EMusic::BgMusicTwo, true);
	}

	float dt = myDeltaTimer.getElapsedTime().asSeconds();
	myDeltaTimer.restart();

	dt = dt > 1.f ? 1.f : dt;

	myTimeWithNoMusic += dt;


	UpdateWhirlwinds(dt);

	myWorld.Render();

	myBirdSpawner.Update(dt, myWindow->getView().getCenter());

	myPlayer.Update(dt, myWorld);

	myPlayer.Render();

	DisplayOtherShips();


	myBirdSpawner.Render(myWindow);

	CRenderer::GetInstance().SetPosition(myPlayer.GetPosition().x, myPlayer.GetPosition().y);

	CRenderer::GetInstance().SetRotation(myPlayer.GetShip().GetRotation() + 90.f);

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape))
	{
		CNetworking::GetInstance().Disconnect();
		CApplication::EnterMenu();
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

	if (myPlayer.GetGoldBalance() == 700 && myShouldSendWinning)
	{
		myPlayerHasWon = true;

		if (CNetworking::GetInstance().GetIsNetworkEnabled())
		{
			CNetworking::GetInstance().SendWinning(false);
		}

		SetWinner("You ");
		myShouldSendWinning = false;
	}
	else if (myPlayer.GetGoldBalance() == 500)
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

			sf::Vector2f shipToShip = myPlayer.GetShip().GetPosition() - other.myTransform.getPosition();
			float distance = Math::Length(shipToShip);

			if (distance <= myPlayer.GetShip().GetWidth())
			{
				Math::Normalize(shipToShip);
				if (distance == 0.f)
				{
					distance = 0.01f;
				}

				float maxDistance = myPlayer.GetShip().GetWidth();
				float percentage = distance / maxDistance;

				nudge += shipToShip * 600.f * (1.f - percentage);
			}

			myWindow->draw(otherPlayerName);
		}
	}

	myPlayer.GetShip().SetShipNudge(nudge);
}

void CGame::GenerateWorld()
{
	myShouldSendCloseToWinning = true;


	myPlayer.Respawn();
	myPlayer.FlushTreasury();

	myWorld.Generate(myUIMap);
	myWorld.PlaceTreasure(myUIMap);
	CNetworking::GetInstance().SetMap(myWorld.GetRawMap());
}

void CGame::LoadMapFromServer(const SMap& aMap)
{
	myShouldSendCloseToWinning = true;

	myWhirlwinds.clear();

	myWorld.CreateFromGeneratedMap(aMap, myUIMap);
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


		drag += myWhirlwindBuffer[i].first.GetDragTo(myPlayer.GetShip().GetPosition(), shouldKillPlayer);

		myWhirlwindBuffer[i].first.Render();
	}

	if (shouldKillPlayer)
	{
		myPlayer.GetShip().Sink();
	}

	myPlayer.GetShip().SetWhirlwindDrag(drag);
}

bool CGame::GetShouldRun() const
{
	return myShouldRun;
}
