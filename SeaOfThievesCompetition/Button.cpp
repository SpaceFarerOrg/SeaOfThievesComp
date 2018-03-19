#include "Button.h"
#include "SFML\Window\Mouse.hpp"
#include "SFML\Graphics\RenderWindow.hpp"

void CButton::Init(const sf::String & aTitle, const sf::Vector2f & aPosition, std::function<void(void)> aFunctionToCall)
{
	myTitle.setString(aTitle);
	myTitle.setPosition(aPosition);
	myOnPressedFunction = aFunctionToCall;

	myTitle.setFont(ourFont);
	myColor = sf::Color(250, 253, 193);
	myShadowColor = sf::Color(16, 101, 83);
	myTitle.setCharacterSize(ourWindow->getSize().x / 35);
	myTitle.setOrigin(0, myTitle.getLocalBounds().height / 2);

	myIsHighlighted = false;
}

void CButton::Update(float aDT)
{
	sf::Vector2i mPos = sf::Mouse::getPosition(*ourWindow);

	myIsHighlighted = false;

	if (myTitle.getGlobalBounds().contains(mPos.x, mPos.y))
	{
		myIsHighlighted = true;
		if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
		{
			myOnPressedFunction();
		}
	}
}

void CButton::Render(sf::RenderWindow* aRenderWindow)
{
	sf::Vector2f shadowOffset = sf::Vector2f(2 * myIsHighlighted ? 2 : 1, 2 * myIsHighlighted ? 2 : 1);
	float shadowScale = 1.f;
	myTitle.setFillColor(myShadowColor);
	myTitle.move(shadowOffset);
	myTitle.setScale(shadowScale, shadowScale);
	aRenderWindow->draw(myTitle);
	myTitle.setFillColor(myColor);
	myTitle.move(-shadowOffset);
	myTitle.setScale(1, 1);
	aRenderWindow->draw(myTitle);
}
