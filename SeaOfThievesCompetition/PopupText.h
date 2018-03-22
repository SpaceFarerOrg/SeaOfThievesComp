#pragma once
#include <SFML\Graphics\Text.hpp>
#include <SFML\System\Clock.hpp>

class CPopupText
{
public:
	CPopupText();
	void Init(const sf::String& aText, float aTimeToShow);
	void SetString(const sf::String& aText);
	void Display();
	void Hide();
	void Render();
private:
	sf::Clock myTimer;
	bool myShouldShow;
	float myTimeToShow;
	float myTimeShown;
	sf::Text myText;
};