#pragma once
#include <SFML\Graphics\Sprite.hpp>
#include <SFML\System\Vector2.hpp>
#include "Defines.h"
#include <sfml\Graphics\CircleShape.hpp>
#include "GameObject.h"

namespace sf
{
	class RenderWindow;
}

enum class EIslandType
{
	IslandOne,
	IslandTwo,
	IslandThree,
	GoldIsland,
};

class CIsland : public CGameObject
{
public:
	CIsland();
	void SetIslandData(EIslandType aIslandType,const sf::Vector2f& aPosition);
	void Init() override;
	
	bool IsColliding(const sf::Vector2f& aOtherCollider);
	bool IsInLootingRange(const sf::Vector2f& aPosition);

	void SetIndexInMap(size_t aIndex);
	size_t GetIndexInMap() const;

	bool IsGoldIsland() const;
	bool HasTreasure() const;
	void SetHasTreasure(bool aHas = true);
	void Loot();

private:
	EIslandType myIslandType;

	size_t myIndexInMap;
	bool myIsGoldIsland;
	bool myHasTreasure;

	float myRangeToLoot;
	float myRangeToDie;

};