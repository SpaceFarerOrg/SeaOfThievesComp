#include "Whirlwind.h"
#include <SFML\Graphics\RenderWindow.hpp>

void CWhirlwind::Init(const sf::Texture & aWhirlwindTexture)
{
	mySprite.setTexture(aWhirlwindTexture);
	mySprite.setOrigin(mySprite.getGlobalBounds().width / 2.f, mySprite.getGlobalBounds().height / 2.f);
	myCurrentScale = 1.f;
}

void CWhirlwind::Update(float aDT)
{
	float scaleChange = 0.f;

	myHasFadedOut = false;

	if (myIsFadingIn)
	{
		scaleChange = 2.f * aDT;
	}
	else if (myIsFadingOut)
	{
		scaleChange = -2.f * aDT;
	}

	myCurrentScale += scaleChange;

	if (myCurrentScale > 1.f)
	{
		myCurrentScale = 1.f;
		myIsFadingIn = false;
	}
	else if (myCurrentScale < 0.f)
	{
		myCurrentScale = 0.f;
		myIsFadingOut = false;
		myHasFadedOut = true;
	}

	mySprite.setScale(myCurrentScale, myCurrentScale);

	myRotation += 50.f * aDT;

	mySprite.setRotation(myRotation);
}

void CWhirlwind::SetShouldFade(bool aOut)
{
	if (aOut)
	{
		myIsFadingOut = true;
		myIsFadingIn = false;
	}
	else
	{
		myIsFadingIn = true;
		myIsFadingOut = false;
	}

}

bool CWhirlwind::GetCanChangePosition()
{
	return myHasFadedOut;
}

void CWhirlwind::Render(sf::RenderWindow & aWindow)
{
	aWindow.draw(mySprite);
}

void CWhirlwind::SetPosition(const sf::Vector2f & aPosition)
{
	mySprite.setPosition(aPosition);
}

#include "Math.h"
sf::Vector2f CWhirlwind::GetDragTo(const sf::Vector2f & aPosition, bool& aShouldKill)
{
	sf::Vector2f drag;

	float distance = Math::Length(aPosition - mySprite.getPosition());

	if (distance > myCurrentScale * 600.f)
	{
		return std::move(drag);
	}

	drag = mySprite.getPosition() - aPosition;
	Math::Normalize(drag);
	drag *= (600.f) * myCurrentScale - distance;
	drag *= 0.75f;

	if (distance < 1.f)
	{
		aShouldKill = true;
	}

	return std::move(drag);
}
