#pragma once
#include "Game.h"
#include "Menu.h"
#include "SFML\Graphics\Sprite.hpp"

namespace sf
{
	class RenderWindow;
}

class CApplication
{
public:
	void Init();
	void Update();
	static void StartGame();
	static void EnterMenu();

	bool GetShouldRun() const;
private:
	void HandleWindowEvents();

	bool myShouldClose;
	bool myIsWindowActive;

	static bool myIsInGame;
	static bool myHasChangedState;
	sf::RenderWindow* myWindow;
	CGame myGame;
	CMenu myMenu;

	sf::Texture myCursorTexture;
	sf::Sprite myCursorSprite;
	sf::View myScreenSpaceView;
};