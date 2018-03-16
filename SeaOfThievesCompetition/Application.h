#pragma once
#include "Game.h"
#include "Menu.h"

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
};