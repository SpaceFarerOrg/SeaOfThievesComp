#pragma once
#include <SFML\Graphics\View.hpp>
#include <SFML\Graphics\Text.hpp>
#include "SFML\Graphics\Sprite.hpp"
#include "SFML\System\Clock.hpp"

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
	bool myShouldRun;

	sf::RenderWindow* myWindow;
	sf::View myCamera;
	sf::Font myFont;
	sf::Text myText;

	sf::Sprite myMenuSprite;
	sf::Texture myMenuTexture;

	sf::Sprite myPressEnterPrompt;
	sf::Texture myPressEnterTexture;

	sf::Clock myDeltaTimer;
	float myTotalTime;
};