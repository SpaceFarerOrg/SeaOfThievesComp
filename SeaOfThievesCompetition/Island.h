#pragma once
#include <SFML\Graphics\Sprite.hpp>
#include <SFML\System\Vector2.hpp>

namespace sf
{
	class RenderWindow;
}

class CIsland
{
public:
	CIsland();
	void Init(sf::Texture& aTexture, const sf::Vector2f& aPosition, bool aIsGoldIsland = false);
	
	bool IsColliding(const sf::Vector2f& aOtherCollider);
	bool IsInLootingRange(const sf::Vector2f& aPosition);

	void SetIndexInMap(size_t aIndex);
	size_t GetIndexInMap() const;

	bool IsGoldIsland() const;
	bool HasTreasure() const;
	void SetHasTreasure();
	void Loot();

	void Render(sf::RenderWindow& aWindow);
private:
	size_t myIndexInMap;
	bool myIsGoldIsland;
	bool myHasTreasure;
	sf::Sprite mySprite;
	sf::Transformable myTransform;

};