#pragma once
#include <SFML\Graphics\Sprite.hpp>
#include <SFML/Graphics/Transform.hpp>
#include "Animation.h"
#include <array>
#include <SFML\Graphics\Rect.hpp>

#define CC_MID 0
#define CC_FRONT 1
#define CC_END 2

namespace sf
{
	class RenderWindow;
	class Texture;
}

class CShip
{
public:
	enum class EWaves
	{
		Small,
		Big,
		Count,
	};
public:
	void Init(sf::Texture& aTexture);
	void Update(float aDT);
	void Render(sf::RenderWindow& aWindow);

	void Respawn();

	void Sink();

	bool GetIsDead() const;
	bool GetIsSinking() const;

	const std::array<sf::Vector2f, 4>& GetCollisionPoints() const;
	sf::Vector2f GetPosition() const;

	void SetHoldsTreasure(bool aDoesIt);
	bool GetHasTreasure() const;

	void SetWhirlwindDrag(const sf::Vector2f& aDrag);

	bool GetIsStill() const;

	void SetPosition(const sf::Vector2f& aPosition);
	
	void SetWavesTextures(sf::Texture& aSmallWaves, sf::Texture& aBigWaves);
private:
	bool myHasTreasure;
	bool myIsSinking;
	bool myIsDead;
	float myCurrentOpacity;
	
	std::array<sf::Vector2f, 4> myCollisionPoints;
	std::array<sf::Vector2f, 4> myTransformedCP;

	std::array<CAnimation, (size_t)EWaves::Count> myWaves;

	sf::Vector2f myWhirlwindDrag;

	sf::Transformable myTransform;
	sf::Sprite mySprite;
	float mySpeed;
	float myMaxSpeed;
	float myAccelration;
	float myRotation;
	float myTurnSpeed;
};