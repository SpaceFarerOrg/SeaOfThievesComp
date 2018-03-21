#include "Island.h"
#include <SFML\Graphics\RenderWindow.hpp>
#include "Math.h"
#include "TextureBank.h"

CIsland::CIsland()
{
	myIsGoldIsland = false;
	myHasTreasure = false;
}

void CIsland::SetIslandData(EIslandType aIslandType, const sf::Vector2f & aPosition)
{
	SetPosition(aPosition);
	myIslandType = aIslandType;
	if (aIslandType == EIslandType::GoldIsland)
	{
		myIsGoldIsland = true;
	}
}

void CIsland::Init()
{
	if (myIslandType == EIslandType::IslandOne)
	{
		mySprite.setTexture(GET_TEXTURE(ETexture::Island));
	}
	else if (myIslandType == EIslandType::IslandOne)
	{
		mySprite.setTexture(GET_TEXTURE(ETexture::IslandTwo));
	}
	else if(myIslandType == EIslandType::IslandThree)
	{
		mySprite.setTexture(GET_TEXTURE(ETexture::IslandThree));
	}
	else if (myIslandType == EIslandType::GoldIsland)
	{
		mySprite.setTexture(GET_TEXTURE(ETexture::GoldIsland));
	}

	SetOriginMiddle();

	myRangeToLoot = (mySprite.getGlobalBounds().width / 2.f) + 70.f;
	myRangeToDie = (mySprite.getGlobalBounds().width / 3.5f);
}

bool CIsland::IsColliding(const sf::Vector2f& aPosition)
{
	sf::Vector2f islandPos = mySprite.getPosition();
	return Math::Length(islandPos-aPosition) < myRangeToDie;
}

bool CIsland::IsInLootingRange(const sf::Vector2f & aPosition)
{
	sf::Vector2f positionToCheckFrom = mySprite.getPosition();

	if (Math::Length(positionToCheckFrom - aPosition) < myRangeToLoot)
	{
		return true;
	}

	return false;
}

void CIsland::SetIndexInMap(size_t aIndex)
{
	myIndexInMap = aIndex;
}

size_t CIsland::GetIndexInMap() const
{
	return myIndexInMap;
}

bool CIsland::IsGoldIsland() const
{
	return myIsGoldIsland;
}

bool CIsland::HasTreasure() const
{
	return myHasTreasure;
}

void CIsland::SetHasTreasure(bool aHas)
{
	myHasTreasure = aHas;
}

void CIsland::Loot()
{
	myHasTreasure = false;
}

