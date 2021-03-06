#pragma once
#include <SFML/System/Clock.hpp>
#include <SFML\Graphics\Texture.hpp>
#include <array>
#include "Animation.h"
#include <vector>
#include "Island.h"
#include <SFML\Audio\Music.hpp>
#include <SFML\Audio\Sound.hpp>
#include <SFML\Audio\SoundBuffer.hpp>
#include <SFML\Graphics\View.hpp>
#include "Defines.h"
#include "UIMap.h"
#include "Whirlwind.h"
#include "BirdSpawner.h"
#include "PopupText.h"
#include "World.h"
#include "MapStruct.h"
#include "Player.h"

namespace sf
{
	class RenderWindow;
}

class CGame
{
public:
	void SetWindow(sf::RenderWindow* aWindow);
	void Init();
	void ReInit();
	bool Update();
	void DisplayOtherShips();

	void GenerateWorld();
	void LoadMapFromServer(const SMap& aMap);

	bool GetShouldRun() const;

	void PlaceWhirlwind(const sf::Vector2f& aPosition);

	void SetWinner(const sf::String& aName);
	void ShowSomeoneCloseToWinningText(const sf::String& aName);

private:
	void SetCloseToWinning(bool aShouldSend = false);

private:
	void UpdateWhirlwinds(float aDT);

	bool myShouldSendCloseToWinning;
	bool myPlayerCanLoot;
	bool myPlayerCanSell;
	bool myIsOutsideOfMap;
	bool myShouldRun;
	bool myHasSentClosedToWinning;
	bool myShouldSendWinning;

	float myIsOutsideOfMapTimer;
	float mySomeoneIsCloseToWinningTimer;
	
	CWorld myWorld;

	CPopupText myPlayerActionPrompt;
	CPopupText myPlayerCloseToWinning;
	CPopupText myPlayerWon;

	std::vector<std::pair<CWhirlwind, float>> myWhirlwinds;

	CPlayer myPlayer;

	std::array<std::pair<CWhirlwind, float>, WHIRLWIND_BUFFER_SIZE> myWhirlwindBuffer;
	size_t myNextAvailableWW;
	float mySpawnNewWWTimer;

	sf::RenderWindow* myWindow;
	sf::Sprite myShipSprite;

	CUIMap myUIMap;
	
	CWhirlwind testWW;

	size_t myTreasureIsland;
	size_t myGoldIslandIndex;
	size_t myGoldIslandIndexInMap;
	size_t mySpawnPointIndex;

	bool myHasRegisteredPaus;
	float myTimeWithNoMusic;

	bool myPlayerHasWon;

	CBirdSpawner myBirdSpawner;

	sf::Clock myDeltaTimer;
};