#pragma once
#include <SFML\Graphics\Sprite.hpp>
#include <SFML/Graphics/Transform.hpp>
#include "Animation.h"
#include <array>
#include <SFML\Graphics\Rect.hpp>

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

	sf::Rect<float> GetCollider() const;
	void Sink();

	bool GetIsDead() const;
	bool GetIsSinking() const;

	sf::Vector2f GetPosition() const;

	void SetHoldsTreasure(bool aDoesIt);
	bool GetHasTreasure() const;

	bool GetIsStill() const;

	void SetPosition(const sf::Vector2f& aPosition);
	
	void SetWavesTextures(sf::Texture& aSmallWaves, sf::Texture& aBigWaves);
private:
	bool myHasTreasure;
	bool myIsSinking;
	bool myIsDead;
	float myCurrentOpacity;

	std::array<CAnimation, (size_t)EWaves::Count> myWaves;

	sf::Transformable myTransform;
	sf::Sprite mySprite;
	float mySpeed;
	float myMaxSpeed;
	float myAccelration;
	float myRotation;
	float myTurnSpeed;
};