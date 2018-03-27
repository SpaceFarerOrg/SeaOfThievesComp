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
	void Render();
private:
	short myGold;
	short myTargetGold;
	sf::Font myFont;
	sf::Text myText;
};