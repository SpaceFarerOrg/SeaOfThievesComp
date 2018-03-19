#include "Treasury.h"
#include <SFML\Graphics\RenderWindow.hpp>

CTreasury::CTreasury()
{
	myFont.loadFromFile("font/font.ttf");
	myText.setFont(myFont);
	myText.setCharacterSize(50);
	myGold = 0;
}

void CTreasury::SetGold(short aAmount)
{
	myGold = aAmount;
}

void CTreasury::GiveGold(short aAmount)
{
	myGold += aAmount;
	if (myGold < 0)
		myGold = 0;
}

short CTreasury::GetGold() const
{
	return myGold;
}

void CTreasury::Render(sf::RenderWindow & aWindow)
{
	myText.setString("Gold: " + std::to_string(myGold));

	float ox = myText.getGlobalBounds().width / 2.f;
	float oy = myText.getGlobalBounds().height / 2.f;

	myText.setOrigin(ox, oy);

	myText.setPosition(aWindow.getView().getCenter().x, aWindow.getView().getCenter().y - aWindow.getView().getSize().y / 2.f + 10.f);

	aWindow.draw(myText);
}
