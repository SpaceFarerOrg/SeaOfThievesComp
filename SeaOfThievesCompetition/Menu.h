#pragma once
#include <SFML\Graphics\View.hpp>
#include <SFML\Graphics\Text.hpp>
#include "SFML\Graphics\Sprite.hpp"
#include "SFML\System\Clock.hpp"
#include "Network.h"
#include "Button.h"
#include "TextBox.h"

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

	void SetMenuState();
	bool GetShouldRun() const;

	CTextBox* GetTextBox();
private:
	void StartClient();
	
	enum class EMenuState
	{
		StartGame,
		Controls,
		Connect
	};

	bool myShouldRun;

	bool myIsNetwork;


	sf::RenderWindow* myWindow;
	sf::View myCamera;
	sf::Font myFont;
	sf::Text myText;
	sf::Text myEnterIpText;

	sf::Sprite myMenuSprite;
	sf::Texture myMenuTexture;

	sf::Sprite myConnectSprite;
	sf::Texture myConnectTexture;

	sf::Sprite myPressEnterPrompt;
	sf::Texture myPressEnterTexture;

	sf::Clock myDeltaTimer;
	float myTotalTime;

	std::vector<CButton> myButtons;
	std::vector<CButton> myNetworkButtons;

	CTextBox myTextBox;

	static EMenuState myMenuState;
};