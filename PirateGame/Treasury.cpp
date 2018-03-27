#include "Treasury.h"
#include <SFML\Graphics\RenderWindow.hpp>
#include "Renderer.h"

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
	myTargetGold += aAmount;
	if (myTargetGold < 0)
		myTargetGold = 0;
}

short CTreasury::GetGold() const
{
	return myGold;
}

void CTreasury::Render()
{
	if (myGold < myTargetGold)
	{
		++myGold;
	}
	else if (myGold > myTargetGold)
	{
		--myGold;
	}

	myText.setString("Gold: " + std::to_string(myGold));

	float ox = myText.getGlobalBounds().width / 2.f;
	float oy = myText.getGlobalBounds().height / 2.f;

	myText.setOrigin(ox, oy);

	sf::Vector2f wCenter = CRenderer::GetInstance().GetViewCenter();
	float wHeight = CRenderer::GetInstance().GetViewSize().y;
	myText.setPosition(wCenter.x, wCenter.y - wHeight / 2.f + 10.f);

	CRenderer::GetInstance().Render(myText);
}
