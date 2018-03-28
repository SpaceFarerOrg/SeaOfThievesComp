#include "Player.h"
#include <SFML\Window\Keyboard.hpp>
#include "AudioSystem.h"
#include "TextureBank.h"
#include "Math.h"

CPlayer::CPlayer()
	:myWasUsingMap(false)
{
}

void CPlayer::SetSpawnPosition(const sf::Vector2f & aSpawnPosition)
{
	mySpawnPosition = aSpawnPosition;
}

void CPlayer::Init()
{
	myShip.Init();
	myCompass.Init();
	myShip.SetWavesTextures(CTextureBank::GetInstance().GetTexture(ETexture::ShipWavesBig), CTextureBank::GetInstance().GetTexture(ETexture::ShipWavesBig));
	myMap.Init();
	myPlayerActionPrompt.Init("ERROR", 0.1f);
}

void CPlayer::Update(float aDT, CWorld& aWorld)
{
	myMap.SetMap(aWorld.GetRawMap());
	myMap.SetTreasureIsland(aWorld.GetIndexOfTreasureIsland());

	if (myShip.GetIsDead())
	{
		myTreasury.GiveGold(-100);
		Respawn();
	}
	
	EPlayerAction action = aWorld.CheckPlayerWorldInteraction(myShip);

	RespondToAction(action, aWorld);

	myShip.Update(aDT);
	
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::M))
	{
		UseMap(aDT);
	}
	else
	{
		myWasUsingMap = false;
	}

	myCompass.Update(aDT, myShip.GetTransform().getRotation());

}

void CPlayer::Render()
{
	myShip.Render();

	if(myWasUsingMap)
		myMap.Render();

	myPlayerActionPrompt.Render();

	myTreasury.Render();

	myCompass.Render();
}

void CPlayer::Respawn()
{
	myShip.SetPosition(mySpawnPosition);
	myShip.Respawn();
}

void CPlayer::FlushTreasury()
{
	myTreasury.SetGold(0);
}

sf::Vector2f CPlayer::GetPosition() const
{
	sf::Vector2f pos = myShip.GetPosition();

	return std::move(pos);
}

short CPlayer::GetGoldBalance() const
{
	return myTreasury.GetGold();
}

CShip & CPlayer::GetShip()
{
	return myShip;
}

void CPlayer::UseMap(float aDT)
{
	if (!myWasUsingMap)
	{
		CAudioSystem::GetInstance().PlaySound(ESound::MapRussle);
		myWasUsingMap = true;
	}

	float mapAlpha = myMap.GetAlpha();

	if (myShip.GetIsControlled())
	{
		mapAlpha = Math::Lerp(mapAlpha, 0.75f, aDT * 4.f);
	}
	else
	{
		mapAlpha = Math::Lerp(mapAlpha, 1.0f, aDT * 4.f);
	}

	myMap.SetAlpha(mapAlpha);
}

void CPlayer::RespondToAction(EPlayerAction aAction, CWorld& aWorld)
{
	switch (aAction)
	{
	case EPlayerAction::None:
		myPlayerActionPrompt.Hide();
		break;
	case EPlayerAction::Loot:
		myPlayerActionPrompt.SetString("Press space to loot");
		myPlayerActionPrompt.Display();
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space))
		{
			aWorld.LootTreasure();
			myShip.SetHoldsTreasure(true);
			CAudioSystem::GetInstance().PlaySound(ESound::Loot);
		}

		break;
	case EPlayerAction::Sell:
		myPlayerActionPrompt.SetString("Press space to sell");
		myPlayerActionPrompt.Display();

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space))
		{
			aWorld.PlaceTreasure(myMap);
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
