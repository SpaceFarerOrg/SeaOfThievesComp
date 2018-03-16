#include "Application.h"
#include <SFML\Graphics\RenderWindow.hpp>

bool CApplication::myIsInGame;
bool CApplication::myHasChangedState;

void CApplication::Init()
{
	myWindow = new sf::RenderWindow();

	sf::VideoMode vm;
	vm.height = 896;
	vm.width = 896;
	vm.bitsPerPixel = sf::VideoMode::getDesktopMode().bitsPerPixel;

	myWindow->create(vm, "Sea of Thieves Competition", sf::Style::Close);
	
	myGame.SetWindow(myWindow);
	myMenu.SetWindow(myWindow);

	myGame.Init();
	myMenu.Init();

}

void CApplication::Update()
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
	if (myIsInGame)
	{
		return myGame.GetShouldRun();
	}


	return myMenu.GetShouldRun();
}
