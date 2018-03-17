#include "Island.h"
#include <SFML\Graphics\RenderWindow.hpp>
#include "Math.h"

CIsland::CIsland()
{
	myIsGoldIsland = false;
	myHasTreasure = false;
}

void CIsland::Init(sf::Texture & aTexture, const sf::Vector2f & aPosition, bool aIsGoldIsland)
{
	mySprite.setTexture(aTexture);
	mySprite.setPosition(aPosition);
	mySprite.setOrigin(mySprite.getGlobalBounds().width / 2.f, mySprite.getGlobalBounds().height / 2.f);

	myRangeToLoot = (mySprite.getGlobalBounds().width / 2.f) + 70.f;
	myRangeToDie = (mySprite.getGlobalBounds().width / 3.5f);

	myIsGoldIsland = aIsGoldIsland;
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

void CIsland::SetHasTreasure()
{
	myHasTreasure = true;
}

void CIsland::Loot()
{
	myHasTreasure = false;
}

void CIsland::Render(sf::RenderWindow & aWindow)
{
	aWindow.draw(mySprite);
}
