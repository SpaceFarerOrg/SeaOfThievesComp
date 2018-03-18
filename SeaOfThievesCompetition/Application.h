#pragma once
#include "Game.h"
#include "Menu.h"
#include "SFML\Graphics\Sprite.hpp"
#include <SFML\Network\UdpSocket.hpp>
#include <thread>

namespace sf
{
	class RenderWindow;
}

class CApplication
{
public:
	void Init();
	void Update();
	void ShutDown();
	
	static void StartGame();
	static void EnterMenu();

	bool GetShouldRun() const;
private:
	void UpdateNetworking();

	void HandleWindowEvents();
	
	std::thread myNetworkThread;

	bool myShouldClose;
	bool myIsWindowActive;

	static bool myIsInGame;
	static bool myHasChangedState;
	sf::RenderWindow* myWindow;
	CGame myGame;
	CMenu myMenu;

	CTextBox* myMenuTextBox;

	sf::Texture myCursorTexture;
	sf::Sprite myCursorSprite;
	sf::View myScreenSpaceView;
};