#pragma once
#include <SFML\Graphics\Texture.hpp>
#include <SFML\Graphics\Sprite.hpp>
#include <array>
#include "Defines.h"
#include <vector>
#include <sfml\Graphics\Transformable.hpp>
#include <sfml\Graphics\RenderTexture.hpp>
#include "MapStruct.h"

namespace sf
{
	class RenderWindow;
}

class CUIMap
{
public:
	void Init();
	void SetMap(const SMap& aMap);
	void SetTreasureIsland(size_t aIndexOfIsland);
	void Render();
	float GetWidth() const;
	void SetAlpha(float aAlpha);
	float GetAlpha();
private:
	void CreateDrawableMap();
	
	sf::RenderTexture myGeneratedMap;
	sf::Sprite myGeneratedMapSprite;

	SMap myMap;
	sf::Transformable myTransform;
	size_t myIndexOfTreasureIsland;

	sf::Sprite myMapSprite;
	
	sf::Sprite myIslandSprite;
	sf::Sprite myIsland2Sprite;
	sf::Sprite myIsland3Sprite;
	
	sf::Sprite myGoldIslandSprite;
	sf::Sprite myCrossSprite;
	

	float myAlpha;
};