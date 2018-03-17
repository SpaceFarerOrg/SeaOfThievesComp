#include "Button.h"
#include "SFML\Graphics\RenderWindow.hpp"
#include "SFML\Window\Mouse.hpp"

sf::Font CButton::ourFont;
sf::RenderWindow* CButton::ourWindow;

void CButton::Init(const sf::String & aTitle, const sf::Vector2f & aPosition, std::function<void(void)> aFunctionToCall)
{
	myTitle.setString(aTitle);
	myTitle.setPosition(aPosition);
	myOnPressedFunction = aFunctionToCall;

	myTitle.setFont(ourFont);
	myTitle.setOrigin(0, myTitle.getLocalBounds().height / 2);
	myTitle.setFillColor(sf::Color(250, 253, 193));
	myTitle.setCharacterSize(ourWindow->getSize().x / 35);
}

void CButton::Update(float aDT)
{
	sf::Vector2i mPos = sf::Mouse::getPosition(*ourWindow);

	myTitle.setFillColor(sf::Color(250, 253, 193));

	if (myTitle.getGlobalBounds().contains(mPos.x, mPos.y))
	{
		myTitle.setFillColor(sf::Color(200, 203, 143));
		if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
		{
			myOnPressedFunction();
		}
	}
}

void CButton::Render(sf::RenderWindow* aRenderWindow)
{
	aRenderWindow->draw(myTitle);
}

void CButton::SetFont(const sf::String& aFontName)
{
	ourFont.loadFromFile(aFontName);
}

void CButton::SetWindow(sf::RenderWindow* aRenderWindow)
{
	ourWindow = aRenderWindow;
}
