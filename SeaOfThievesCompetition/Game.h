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
		GoldIsland,
		Waves,
		ShipWavesBig,
		Cross,
		Map,
		MapIsland,
		MapGoldIsland,
		Count,
	};

private:
	void ShowPressButtonPrompt();
	void CreateWorld();
	void CheckShipCollisionVsIslands();
	void PlaceTreasure();
	void EnsurePlayerKeepingOnMap(float aDT);
	bool myPlayerCanLoot;
	bool myPlayerCanSell;

	sf::Text myPressSpaceToLoot;
	sf::Text myPressSpaceToSellTreasure;
	sf::Text myYouAreOutsideOfMap;
	sf::Font myFont;

	bool myIsOutsideOfMap;
	float myIsOutsideOfMapTimer;

	sf::Vector2f TranslateMapPointToWorldPosition(size_t aMapIndex);
	std::vector<CIsland> myIslands;
	std::vector<CAnimation> myWaves;

	sf::RenderWindow* myWindow;

	CTreasury myTreasury;

	std::array<int, MAP_AXIS_SIZE * MAP_AXIS_SIZE> myMap;

	sf::View myCamera;

	void LoadTextures();

	std::array<sf::Texture, (size_t)ETexture::Count> myTextureBank;
	CShip myShip;

	CUIMap myUIMap;

	size_t myGoldIslandIndex;

	CAnimation myAnimation;

	bool myShouldRun;

	sf::Music myBackgroundMusic;
	sf::Music myBackgroundSound;
	sf::Sound myCashSound;
	sf::SoundBuffer myCashSoundBuffer;

	sf::Clock myDeltaTimer;
};

