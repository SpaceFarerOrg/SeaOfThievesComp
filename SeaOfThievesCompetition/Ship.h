#pragma once
#include <SFML\Graphics\Sprite.hpp>
#include <SFML/Graphics/Transform.hpp>
#include "Animation.h"
#include <array>
#include <SFML\Graphics\Rect.hpp>
#include "GameObject.h"

#define CC_MID 0
#define CC_FRONT 1
#define CC_END 2

namespace sf
{
	class RenderWindow;
	class Texture;
}

class CShip : public CGameObject
{
public:
	enum class EWaves
	{
		Small,
		Big,
		Count,
	};
public:
	void Init() override;
	void Update(float aDT) override;
	void Render() override;

	void Respawn();

	void Sink();

	float GetWidth() const;

	bool GetIsDead() const;
	bool GetIsSinking() const;

	const std::array<sf::Vector2f, 4>& GetCollisionPoints() const;
	bool GetIsControlled() const;

	sf::Vector2f GetPosition() const;

	void SetHoldsTreasure(bool aDoesIt);
	bool GetHasTreasure() const;

	void SetWhirlwindDrag(const sf::Vector2f& aDrag);
	void SetShipNudge(const sf::Vector2f& aNudge);

	bool GetIsStill() const;

	void SetPosition(const sf::Vector2f& aPosition);
	
	void SetWavesTextures(const sf::Texture& aSmallWaves, const sf::Texture& aBigWaves);

	bool GetIsInvincible() const;
	bool GetHasRespawned();
private:
	bool myHasRespawned;
	bool myHasTreasure;
	bool myIsSinking;
	bool myIsDead;
	float myCurrentOpacity;
	float myInvisibilityTimer;

	std::array<sf::Vector2f, 4> myCollisionPoints;
	std::array<sf::Vector2f, 4> myTransformedCP;

	std::array<CAnimation, (size_t)EWaves::Count> myWaves;

	sf::Vector2f myWhirlwindDrag;
	sf::Vector2f myShipNudge;

	sf::Sprite myWavesSprite;
	float mySpeed;
	float myMaxSpeed;
	float myAccelration;
	float myRotation;
	float myTurnSpeed;
	bool myIsControlled;
};