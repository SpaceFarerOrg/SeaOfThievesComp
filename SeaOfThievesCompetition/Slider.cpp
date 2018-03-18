#include "Slider.h"
#include "SFML\Graphics\RenderWindow.hpp"
#include "SFML\Window\Mouse.hpp"
#include "Math.h"

CSlider::CSlider()
{
}


CSlider::~CSlider()
{
}

sf::Texture CSlider::ourSliderTexture;
sf::Texture CSlider::ourButtonTexture;

void CSlider::Init(const sf::Vector2f & aPosition)
{
	static bool firstInit = true;
	if (firstInit)
	{
		ourSliderTexture.loadFromFile("sprites/slider.png");
		ourButtonTexture.loadFromFile("sprites/handle.png");

		firstInit = false;
	}

	mySliderSprite.setTexture(ourSliderTexture);
	myButtonSprite.setTexture(ourButtonTexture);

	myButtonSprite.setOrigin(ourButtonTexture.getSize().x / 2, 0.f);

	myValue = 50.f;
	myPosition = aPosition;
}

float CSlider::GetValue()
{
	return myValue;
}

void CSlider::Update(float aDT)
{
	sf::Vector2i mPos = sf::Mouse::getPosition(*ourWindow);

	if (mySliderSprite.getGlobalBounds().contains(mPos.x, mPos.y))
	{
		if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
		{
			myValue = 100.f * (mPos.x - mySliderSprite.getPosition().x) / ourSliderTexture.getSize().x;
		}
	}
}

void CSlider::Render(sf::RenderWindow * aRenderWindow)
{
	sf::Vector2f valueOffset = myValue / 100.f * sf::Vector2f((float)ourSliderTexture.getSize().x, 0);

	valueOffset.x = Math::Clamp(valueOffset.x, (float)ourButtonTexture.getSize().x / 2, (float)ourSliderTexture.getSize().x - ourButtonTexture.getSize().x / 2);

	mySliderSprite.setPosition(myPosition);
	myButtonSprite.setPosition(myPosition + valueOffset);

	aRenderWindow->draw(mySliderSprite);
	aRenderWindow->draw(myButtonSprite);
}
