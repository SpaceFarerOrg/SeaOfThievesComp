#pragma once
#include <SFML\Graphics\View.hpp>
#include <SFML\Graphics\Text.hpp>

namespace sf
{
	class RenderWindow;
}

class CMenu
{
public:

	void Init();
	void SetWindow(sf::RenderWindow* aWindow);
	void Update();

	bool GetShouldRun() const;
private:
	void HandleWindowEvents();
	bool myShouldRun;

	sf::RenderWindow* myWindow;
	sf::View myCamera;
	sf::Font myFont;
	sf::Text myText;
};