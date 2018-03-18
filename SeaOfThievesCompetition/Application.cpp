#include "Application.h"
#include <SFML\Graphics\RenderWindow.hpp>
#include <SFML\Window\Event.hpp>
#include "UIBase.h"
#include "SFML\Window\Mouse.hpp"

bool CApplication::myIsInGame;
bool CApplication::myHasChangedState;
float CApplication::myMasterVolume;

void CApplication::Init()
{
	myHasChangedState = false;
	myShouldClose = false;
	myIsWindowActive = true;
	myWindow = new sf::RenderWindow();

	sf::VideoMode vm;
	vm = sf::VideoMode::getDesktopMode();

	//vm.height = 896;
	//vm.width = 896;
	vm.bitsPerPixel = sf::VideoMode::getDesktopMode().bitsPerPixel;

	myWindow->create(vm, "Sea of Thieves Competition", sf::Style::Close);

	CUIBase::SetWindow(myWindow);
	CUIBase::SetFont("font/font.ttf");

	myGame.SetWindow(myWindow);
	myMenu.SetWindow(myWindow);

	myGame.Init();
	myMenu.Init();
	myGame.UpdateVolumes();

	myMenuTextBox = myMenu.GetTextBox();

	myCursorTexture.loadFromFile("sprites/cursor.png");
	myCursorSprite.setTexture(myCursorTexture);

	myWindow->setMouseCursorVisible(false);

	myScreenSpaceView = myWindow->getView();
	myNetworkThread = std::thread([&]()->void { UpdateNetworking(); });
}

void CApplication::Update()
{
	if (myIsWindowActive)
	{
		bool isMultiplayerClient = CNetworking::GetInstance().GetIsNetworkEnabled() && CNetworking::GetInstance().GetIsClient();

		myGame.UpdateVolumes();

		myWindow->clear({ 95,189,197 });
		if (myHasChangedState && myIsInGame)
		{
			myHasChangedState = false;

			if (!isMultiplayerClient)
			{
				myGame.GenerateWorld();
			}
			else
			{
				myGame.LoadMapFromServer(CNetworking::GetInstance().GetMap());
			}
		}
		else if (myHasChangedState)
		{
			myHasChangedState = false;
			myMenu.SetMenuState();
		}

		if (myIsInGame)
		{
			myShouldClose = !myGame.Update();
		}
		else
		{
			myShouldClose = !myMenu.Update();
		}

		sf::Vector2i mPos = sf::Mouse::getPosition(*myWindow);
		myCursorSprite.setPosition(mPos.x, mPos.y);
		myWindow->setView(myScreenSpaceView);
		myWindow->draw(myCursorSprite);

		myWindow->display();
	}

	HandleWindowEvents();
}

void CApplication::ShutDown()
{
	myNetworkThread.join();
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

void CApplication::SetVolume(float aMasterVolume)
{
	myMasterVolume = aMasterVolume;
}

float CApplication::GetVolume()
{
	return myMasterVolume;
}

bool CApplication::GetShouldRun() const
{
	return !myShouldClose;
}

void CApplication::UpdateNetworking()
{
	while (!myShouldClose)
	{
		CNetworking::GetInstance().Update();
		std::this_thread::yield();
	}
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

		else if (e.type == sf::Event::TextEntered)
		{
			if (e.text.unicode != 8)
			{
				myMenuTextBox->AddText(e.text.unicode);
			}
			else
			{
				myMenuTextBox->RemoveCharacter();
			}
		}
	}
}
