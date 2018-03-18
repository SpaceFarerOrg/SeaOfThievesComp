#include "TextBox.h"

void CTextBox::Init(const sf::Vector2f & aPosition)
{
	myFont.loadFromFile("font/font.ttf");
	myGraphicsText.setFont(myFont);


	myShape.setFillColor({ 0,0,0,25 });
	myShape.setSize({ 250, (float)myGraphicsText.getCharacterSize() + 10.f });
	myShape.setPosition(aPosition);
	myShape.setOrigin(myShape.getGlobalBounds().width / 2.f, myShape.getGlobalBounds().height / 2.f);

	myGraphicsText.setPosition(myShape.getPosition().x, myShape.getPosition().y - 5.f);
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
	if (myIsActive)
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
	myGraphicsText.setOrigin(myGraphicsText.getGlobalBounds().width / 2.f, myGraphicsText.getGlobalBounds().height / 2.f);

	aWindow.draw(myShape);
	aWindow.draw(myGraphicsText);
}
