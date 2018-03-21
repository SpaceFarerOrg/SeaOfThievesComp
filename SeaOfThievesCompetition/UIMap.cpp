#include "UIMap.h"
#include <sfml\Graphics\RenderWindow.hpp>
#include "TextureBank.h"

void CUIMap::Init(std::array<int, MAP_AXIS_SIZE*MAP_AXIS_SIZE>& aMap)
{
	myMapSprite.setTexture(GET_TEXTURE(ETexture::Map));
	myIslandSprite.setTexture(GET_TEXTURE(ETexture::MapIsland));
	myIsland2Sprite.setTexture(GET_TEXTURE(ETexture::MapIslandTwo));
	myIsland3Sprite.setTexture(GET_TEXTURE(ETexture::MapIslandThree));
	myGoldIslandSprite.setTexture(GET_TEXTURE(ETexture::MapGoldIsland));
	myCrossSprite.setTexture(GET_TEXTURE(ETexture::Cross));
	myMap = aMap;

	myIslandSprite.setScale(0.65f, 0.65f);
	myIsland2Sprite.setScale(0.65f, 0.65f);
	myIsland3Sprite.setScale(0.65f, 0.65f);
	myGoldIslandSprite.setScale(0.65f, 0.65f);

	myGoldIslandSprite.setOrigin(myGoldIslandSprite.getGlobalBounds().width / 2.f, myGoldIslandSprite.getGlobalBounds().height / 2.f);
	myIslandSprite.setOrigin(myIslandSprite.getGlobalBounds().width / 2.f, myIslandSprite.getGlobalBounds().height / 2.f);
	myIsland2Sprite.setOrigin(myIsland2Sprite.getGlobalBounds().width / 2.f, myIsland2Sprite.getGlobalBounds().height / 2.f);
	myIsland3Sprite.setOrigin(myIsland3Sprite.getGlobalBounds().width / 2.f, myIsland3Sprite.getGlobalBounds().height / 2.f);

	myMapSprite.setScale(1.f, 1.f);


	myGeneratedMap.create(myMapSprite.getGlobalBounds().width, myMapSprite.getGlobalBounds().height);
	myGeneratedMapSprite.setTexture(myGeneratedMap.getTexture());

	myAlpha = 1.0f;
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

	myGeneratedMapSprite.setColor(sf::Color(255,255,255, 255.f * myAlpha));
	myGeneratedMapSprite.setOrigin(myGeneratedMapSprite.getGlobalBounds().width / 2, myGeneratedMapSprite.getGlobalBounds().height / 2.f);
	myGeneratedMapSprite.setPosition(aWindow.getView().getCenter());

	aWindow.draw(myGeneratedMapSprite);
}

float CUIMap::GetWidth() const
{
	return myGeneratedMapSprite.getGlobalBounds().width;
}

void CUIMap::SetAlpha(float aAlpha)
{
	myAlpha = aAlpha;
}

float CUIMap::GetAlpha()
{
	return myAlpha;
}

void CUIMap::CreateDrawableMap()
{
	myGeneratedMap.clear({ 0,0,0,0 });
	myGeneratedMap.draw(myMapSprite);

	int chunkSize = 600 / MAP_AXIS_SIZE;
	float offsetFromEdge = 180.f;//(myMapSprite.getGlobalBounds().width / 2) / 2.f;

	for (size_t i = 0; i < myMap.size(); ++i)
	{
		float posX = (float)((i % MAP_AXIS_SIZE)*chunkSize) + offsetFromEdge;
		float posY = (float)((i / MAP_AXIS_SIZE)*chunkSize) + offsetFromEdge;
		
		bool isIsland = myMap[i] == ISLAND_1 || myMap[i] == ISLAND_2 || myMap[i] == ISLAND_3;

		if (isIsland)
		{
			myIslandSprite.setPosition(posX, posY);
			myIsland2Sprite.setPosition(posX, posY);
			myIsland3Sprite.setPosition(posX, posY);

			if (myMap[i] == ISLAND_1)
			{
				myGeneratedMap.draw(myIslandSprite);
			}
			if (myMap[i] == ISLAND_2)
			{
				myGeneratedMap.draw(myIsland2Sprite);
			}
			if (myMap[i] == ISLAND_3)
			{
				myGeneratedMap.draw(myIsland3Sprite);
			}
		}
		if (myMap[i] == GOLD_ISLAND)
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
