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

namespace sf
{
	class RenderWindow;
}

class CGame
{
public:
	void SetWindow(sf::RenderWindow* aWindow);
	void Init();
	void Update();

	void GenerateWorld();

	bool GetShouldRun() const;
private:
	enum class ETexture
	{
		Ship,
		Island,
		IslandTwo,
		IslandThree,
		GoldIsland,
		Waves,
		ShipWavesBig,
		Cross,
		Map,
		MapIsland,
		MapGoldIsland,
		Whirlwind,
		Count,
	};

private:
	void ShowPressButtonPrompt();
	void CreateWorld();
	void CheckShipCollisionVsIslands();
	void PlaceTreasure();
	void EnsurePlayerKeepingOnMap(float aDT);
	void LoadTextures();

	void CreateIslands();
	void CreateWaves();

	void PlaceWhirlwind(int aIndex = -1);
	void UpdateWhirlwinds(float aDT);

	sf::Vector2f TranslateMapPointToWorldPosition(size_t aMapIndex);

	bool myPlayerCanLoot;
	bool myPlayerCanSell;
	bool myIsOutsideOfMap;
	bool myShouldRun;

	float myIsOutsideOfMapTimer;
	
	sf::Text myPressSpaceToLoot;
	sf::Text myPressSpaceToSellTreasure;
	sf::Text myYouAreOutsideOfMap;
	sf::Font myFont;

	std::vector<CIsland> myIslands;
	std::vector<CAnimation> myWaves;

	std::array<int, MAP_AXIS_SIZE * MAP_AXIS_SIZE> myMap;
	std::array<sf::Texture, (size_t)ETexture::Count> myTextureBank;
	std::vector<std::pair<CWhirlwind, float>> myWhirlwinds;

	sf::RenderWindow* myWindow;

	CTreasury myTreasury;

	sf::View myCamera;

	CShip myShip;

	CUIMap myUIMap;
	
	CWhirlwind testWW;

	size_t myGoldIslandIndex;
	size_t mySpawnPointIndex;

	float myTargetRightOffset;
	float myCurrentRightOffset;

	sf::Music myBackgroundMusic;
	sf::Music myBackgroundSound;
	sf::SoundBuffer myCashSoundBuffer;
	sf::Sound myCashSound;

	CBirdSpawner myBirdSpawner;

	sf::Clock myDeltaTimer;
};

