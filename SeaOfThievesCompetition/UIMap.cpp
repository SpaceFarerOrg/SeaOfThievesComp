#include "UIMap.h"
#include <sfml\Graphics\RenderWindow.hpp>

void CUIMap::Init(sf::Texture & aMapTexture, sf::Texture & aMapIslandTexture, sf::Texture & aMapGoldIslandTexture, sf::Texture & aCrossTexture, std::array<int, MAP_AXIS_SIZE*MAP_AXIS_SIZE>& aMap)
{
	myMapSprite.setTexture(aMapTexture);
	myIslandSprite.setTexture(aMapIslandTexture);
	myGoldIslandSprite.setTexture(aMapGoldIslandTexture);
	myCrossSprite.setTexture(aCrossTexture);
	myMap = aMap;

	myGeneratedMap.create(850, 850);
	myGeneratedMapSprite.setTexture(myGeneratedMap.getTexture());
}

void CUIMap::SetMap(std::array<int, MAP_AXIS_SIZE*MAP_AXIS_SIZE>& aMap)
{
	myMap = aMap;
}

void CUIMap::SetTreasureIsland(size_t aIndexOfIsland)
{
	myIndexOfTreasureIsland = aIndexOfIsland;
}

void CUIMap::Render(sf::RenderWindow & aWindow)
{
	CreateDrawableMap();

	myGeneratedMapSprite.setOrigin(myGeneratedMapSprite.getGlobalBounds().width / 2.f, myGeneratedMapSprite.getGlobalBounds().height / 2.f);
	myGeneratedMapSprite.setPosition(aWindow.getView().getCenter());

	aWindow.draw(myGeneratedMapSprite);
}

void CUIMap::CreateDrawableMap()
{
	myGeneratedMap.clear({ 0,0,0,0 });
	myGeneratedMap.draw(myMapSprite);



	int chunkSize = 700 / MAP_AXIS_SIZE;
	float offsetFromEdge = (myMapSprite.getGlobalBounds().width - 700.f) / 2.f;

	for (size_t i = 0; i < myMap.size(); ++i)
	{
		float posX = (float)((i % MAP_AXIS_SIZE)*chunkSize) + offsetFromEdge;
		float posY = (float)((i / MAP_AXIS_SIZE)*chunkSize) + offsetFromEdge;
		
		if (myMap[i] == ISLAND)
		{
			myIslandSprite.setPosition(posX, posY);
			myGeneratedMap.draw(myIslandSprite);
		}
		else if (myMap[i] == GOLD_ISLAND)
		{
			myGoldIslandSprite.setPosition(posX, posY);
			myGeneratedMap.draw(myGoldIslandSprite);
		}

		if (i == myIndexOfTreasureIsland)
		{
			myCrossSprite.setPosition(posX, posY);
			myGeneratedMap.draw(myCrossSprite);
		}
	}


	myGeneratedMap.display();


}
