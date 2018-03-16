#include "Application.h"
#include <SFML\Graphics\RenderWindow.hpp>
#include <SFML\Window\Event.hpp>

bool CApplication::myIsInGame;
bool CApplication::myHasChangedState;

void CApplication::Init()
{
	myShouldClose = false;
	myIsWindowActive = true;
	myWindow = new sf::RenderWindow();

	sf::VideoMode vm;
	vm = sf::VideoMode::getDesktopMode();
	
	//vm.height = 896;
	//vm.width = 896;
	vm.bitsPerPixel = sf::VideoMode::getDesktopMode().bitsPerPixel;

	myWindow->create(vm, "Sea of Thieves Competition", sf::Style::Close);

	myGame.SetWindow(myWindow);
	myMenu.SetWindow(myWindow);

	myGame.Init();
	myMenu.Init();

}

void CApplication::Update()
{
	if (myIsWindowActive)
	{
		myWindow->clear({ 95,189,197 });
		if (myHasChangedState)
		{
			myHasChangedState = false;
			myGame.GenerateWorld();
		}

		if (myIsInGame)
		{
			myGame.Update();
		}
		else
		{
			myMenu.Update();
		}
		myWindow->display();
	}

	HandleWindowEvents();
}

void CApplication::StartGame()
{
	myIsInGame = true;
	myHasChangedState = true;
}

void CApplication::EnterMenu()
{
	myIsInGame = false;
	myHasChangedState = true;
}

bool CApplication::GetShouldRun() const
{
	return !myShouldClose;
}

void CApplication::HandleWindowEvents()
{
	sf::Event e;

	while (myWindow->pollEvent(e))
	{
		if (e.type == sf::Event::Closed)
		{
			myShouldClose = true;
		}

		if (e.type == sf::Event::LostFocus)
		{
			myIsWindowActive = false;
		}

		if (e.type == sf::Event::GainedFocus)
		{
			myIsWindowActive = true;
		}
	}
}
