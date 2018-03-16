#pragma once
#include <SFML\Graphics\Texture.hpp>
#include <SFML\Graphics\Sprite.hpp>
#include <array>
#include "Defines.h"
#include <vector>
#include <sfml\Graphics\Transformable.hpp>
#include <sfml\Graphics\RenderTexture.hpp>

namespace sf
{
	class RenderWindow;
}

class CUIMap
{
public:
	void Init(sf::Texture& aMapTexture, sf::Texture& aMapIslandTexture, sf::Texture& aMapGoldIslandTexture, sf::Texture& aCrossTexture, std::array<int, MAP_AXIS_SIZE * MAP_AXIS_SIZE>& aMap);
	void SetMap(std::array<int, MAP_AXIS_SIZE * MAP_AXIS_SIZE>& aMap);
	void SetTreasureIsland(size_t aIndexOfIsland);
	void Render(sf::RenderWindow& aWindow);
private:
	void CreateDrawableMap();
	
	sf::RenderTexture myGeneratedMap;
	sf::Sprite myGeneratedMapSprite;

	std::array<int, MAP_AXIS_SIZE * MAP_AXIS_SIZE> myMap;
	sf::Transformable myTransform;
	size_t myIndexOfTreasureIsland;
	sf::Sprite myMapSprite;
	sf::Sprite myIslandSprite;
	sf::Sprite myGoldIslandSprite;
	sf::Sprite myCrossSprite;
};