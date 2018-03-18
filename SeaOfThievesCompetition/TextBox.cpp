#include "TextBox.h"

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
	if (myText.getSize() > 0)
	{
		myText.erase(myText.getSize()-1,1);
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
