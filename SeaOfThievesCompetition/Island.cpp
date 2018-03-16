#include "Island.h"
#include <SFML\Graphics\RenderWindow.hpp>
#include "Math.h"

#define RANGE_TO_LOOT 95.f
#define RANGE_TO_SINK 64.f

CIsland::CIsland()
{
	myIsGoldIsland = false;
	myHasTreasure = false;
}

void CIsland::Init(sf::Texture & aTexture, const sf::Vector2f & aPosition, bool aIsGoldIsland)
{
	mySprite.setTexture(aTexture);
	mySprite.setPosition(aPosition);

	myIsGoldIsland = aIsGoldIsland;
}

bool CIsland::IsColliding(const sf::Vector2f& aPosition)
{
	sf::Vector2f islandPos = { mySprite.getPosition().x + 64.f, mySprite.getPosition().y + 64.f };
	return Math::Length(islandPos-aPosition) < RANGE_TO_SINK;
}

bool CIsland::IsInLootingRange(const sf::Vector2f & aPosition)
{
	sf::Vector2f positionToCheckFrom = mySprite.getPosition();
	positionToCheckFrom.x += 64.f;
	positionToCheckFrom.y += 64.f;

	if (Math::Length(positionToCheckFrom - aPosition) < RANGE_TO_LOOT)
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
