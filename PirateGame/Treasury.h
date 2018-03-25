#pragma once
#include <SFML\Graphics\Text.hpp>

namespace sf
{
	class RenderWindow;
}

class CTreasury
{
public:
	CTreasury();
	void SetGold(short aAmount);
	void GiveGold(short aAmount);
	short GetGold() const;
	void Render(sf::RenderWindow& aWindow);
private:
	short myGold;
	sf::Font myFont;
	sf::Text myText;
};