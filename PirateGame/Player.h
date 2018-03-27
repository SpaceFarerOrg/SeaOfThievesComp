#pragma once
#include "Ship.h"
#include "Treasury.h"
#include "World.h"
#include "UIMap.h"
#include "PopupText.h"

class CPlayer
{
public:
	CPlayer();

	void SetSpawnPosition(const sf::Vector2f& aSpawnPosition);
	void Init();
	void Update(float aDT, CWorld& aWorld);
	void Render();

	void Respawn();
	void FlushTreasury();

	sf::Vector2f GetPosition() const;

	short GetGoldBalance() const;

	CShip& GetShip();
private:
	void UseMap(float aDT);

	void RespondToAction(EPlayerAction aAction, CWorld& aWorld);
	
	sf::Vector2f mySpawnPosition;

	bool myWasUsingMap;
	CUIMap myMap;
	CPopupText myPlayerActionPrompt;
	CTreasury myTreasury;
	CShip myShip;
};