#pragma once
#include "Animation.h"
#include <SFML\Graphics\Sprite.hpp>


class CWhirlwind
{
public:
	void Init(const sf::Texture& aWhirlwindTexture);
	void Update(float aDT);
	void SetShouldFade(bool aOut);
	bool GetCanChangePosition();
	void Render();
	void SetPosition(const sf::Vector2f& aPosition);
	sf::Vector2f GetDragTo(const sf::Vector2f& aPosition, bool& aShouldKill);
private:
	bool myIsFadingIn;
	bool myIsFadingOut;
	bool myHasFadedOut;

	sf::Sprite mySprite;
	float myCurrentScale;
	float myRotation;
};