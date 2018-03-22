#pragma once
#include <SFML/System/Clock.hpp>
#include <SFML\Graphics\Texture.hpp>
#include <array>
#include "Ship.h"
#include "Animation.h"
#include <vector>
#include "Island.h"
#include "Treasury.h"
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
	void LoadMapFromServer(const std::array<int, MAP_AXIS_SIZE * MAP_AXIS_SIZE>& aMap);

	bool GetShouldRun() const;

	void PlaceWhirlwind(const sf::Vector2f& aPosition);
	void UpdateVolumes();

	void SetWinner(const sf::String& aName);
	void ShowSomeoneCloseToWinningText(const sf::String& aName);

private:
	void SetCloseToWinning(bool aShouldSend = false);

private:
	void RandomizeSong();

	void ShowPressButtonPrompt();
	void CheckShipCollisionVsIslands();
	void EnsurePlayerKeepingOnMap(float aDT);

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

	sf::Text myPressSpaceToLoot;
	sf::Text myPressSpaceToSellTreasure;
	sf::Text myYouAreOutsideOfMap;
	sf::Font myFont;
	CPopupText myPlayerCloseToWinning;
	CPopupText myPlayerWon;

	std::vector<std::pair<CWhirlwind, float>> myWhirlwinds;

	std::array<std::pair<CWhirlwind, float>, WHIRLWIND_BUFFER_SIZE> myWhirlwindBuffer;
	size_t myNextAvailableWW;
	float mySpawnNewWWTimer;

	sf::RenderWindow* myWindow;
	sf::Sprite myShipSprite;

	CTreasury myTreasury;

	sf::View myCamera;

	CShip myShip;

	CUIMap myUIMap;
	
	CWhirlwind testWW;

	size_t myTreasureIsland;
	size_t myGoldIslandIndex;
	size_t myGoldIslandIndexInMap;
	size_t mySpawnPointIndex;

	std::array<sf::Music, 2> myBackgroundSongs;
	size_t myActiveSong;
	sf::Music myBackgroundSound;
	sf::Music myStressMusic;
	sf::Music myWinMusic;
	bool myHasRegisteredPaus;
	float myTimeWithNoMusic;

	bool myPlayerHasWon;
	
	sf::SoundBuffer myCashSoundBuffer;
	sf::Sound myCashSound;
	sf::SoundBuffer myTreasureThump;
	sf::Sound myTreasureSound;
	sf::SoundBuffer myCrash;
	sf::Sound myCrashSound;

	CBirdSpawner myBirdSpawner;

	sf::Clock myDeltaTimer;
};