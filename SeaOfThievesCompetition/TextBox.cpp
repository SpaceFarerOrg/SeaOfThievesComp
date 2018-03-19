#include "TextBox.h"
#include <SFML\Window\Mouse.hpp>

void CTextBox::Init(const sf::Vector2f & aPosition)
{
	myFont.loadFromFile("font/font.ttf");
	myGraphicsText.setFont(myFont);
	myGraphicsText.setCharacterSize(56);
	myGraphicsText.setFillColor(sf::Color(250, 253, 193));

	myShape.setFillColor({ 0,0,0,25 });
	myShape.setSize({ 500, (float)myGraphicsText.getCharacterSize() + 15.f });
	myShape.setPosition(aPosition);
	myShape.setOrigin(myShape.getGlobalBounds().width / 2.f, 0);

	myGraphicsText.setPosition(10 + myShape.getPosition().x - myShape.getGlobalBounds().width / 2, myShape.getPosition().y + 5.f);
	myGraphicsText.setOrigin(0, 0);
}

void CTextBox::Clear()
{
	myText = "";
}

void CTextBox::Update()
{
	sf::FloatRect rect = myShape.getGlobalBounds();
	sf::Vector2f mousePos = { (float)sf::Mouse::getPosition(*myWindow).x, (float)sf::Mouse::getPosition(*myWindow).y };
	if (rect.contains(mousePos))
	{
		myShape.setFillColor({ 0,0,0,50 });
	}
	else
	{
		myShape.setFillColor({ 0,0,0,25 });
	}

	if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
	{

		if (rect.contains(mousePos))
		{
			SetActive(true);
		}
		else
		{
			SetActive(false);
		}
	}
}

void CTextBox::SetActive(bool aState)
{
	myIsActive = aState;
}

void CTextBox::AddText(sf::Uint32 aTextAdded)
{
	if (myIsActive && myText.getSize() < 16)
	{
		myText.insert(myText.getSize(), aTextAdded);
	}
}

void CTextBox::RemoveCharacter()
{
	if (myText.getSize() > 0 && myIsActive)
	{
		myText.erase(myText.getSize() - 1, 1);
	}
}

const sf::String & CTextBox::GetText() const
{
	return myText;
}

void CTextBox::Render(sf::RenderWindow & aWindow)
{
	myGraphicsText.setString(myText);

	aWindow.draw(myShape);
	aWindow.draw(myGraphicsText);
}

void CTextBox::SetWindow(sf::RenderWindow * aWindow)
{
	myWindow = aWindow;
}
