#include "Treasury.h"
#include <SFML\Graphics\RenderWindow.hpp>

CTreasury::CTreasury()
{
	myFont.loadFromFile("font/font.ttf");
	myText.setFont(myFont);
	myGold = 0;
}

void CTreasury::SetGold(short aAmount)
{
	myGold = aAmount;
}

void CTreasury::GiveGold(short aAmount)
{
	myGold += aAmount;
}

void CTreasury::Render(sf::RenderWindow & aWindow)
{
	myText.setString("Gold: " + std::to_string(myGold));

	float ox = myText.getGlobalBounds().width / 2.f;
	float oy = myText.getGlobalBounds().height / 2.f;

	myText.setOrigin(ox, oy);

	myText.setPosition(aWindow.getView().getCenter().x, aWindow.getView().getCenter().y - aWindow.getView().getSize().y / 2.f);

	aWindow.draw(myText);
}
