#include "Application.h"
#include <SFML\Graphics\RenderWindow.hpp>
#include <SFML\Window\Event.hpp>
#include "Button.h"
#include "SFML\Window\Mouse.hpp"

bool CApplication::myIsInGame;
bool CApplication::myHasChangedState;

void CApplication::Init()
{
	myHasChangedState = false;
	myShouldClose = false;
	myIsWindowActive = true;
	myWindow = new sf::RenderWindow();

	sf::VideoMode vm;
	vm = sf::VideoMode::getDesktopMode();

	myFont.loadFromFile("font/font.ttf");
	myConnectMessage.setFont(myFont);
	myConnectMessage.setOutlineThickness(2.f);
	myConnectMessage.setFillColor(sf::Color(250, 253, 193));
	myConnectMessage.setOutlineColor(sf::Color::Black);

	//vm.height = 896;
	//vm.width = 896;
	vm.bitsPerPixel = sf::VideoMode::getDesktopMode().bitsPerPixel;

	myWindow->create(vm, "Sea of Thieves Competition", sf::Style::Close);

	CButton::SetWindow(myWindow);
	CButton::SetFont("font/font.ttf");

	myGame.SetWindow(myWindow);
	myMenu.SetWindow(myWindow);

	myGame.Init();
	myMenu.Init();

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
			myGame.Update();
		}
		else
		{
			myMenu.Update();
		}

		sf::Vector2i mPos = sf::Mouse::getPosition(*myWindow);
		myCursorSprite.setPosition(mPos.x, mPos.y);
		myWindow->setView(myScreenSpaceView);
		myWindow->draw(myCursorSprite);

		if (CNetworking::GetInstance().GetIsNetworkEnabled() && CNetworking::GetInstance().GetIsClient())
		{
			myConnectMessage.setString("Online: Connected");
		}
		else if (CNetworking::GetInstance().GetIsNetworkEnabled() && !CNetworking::GetInstance().GetIsClient())
		{
			myConnectMessage.setString("Online: Is Server");
		}
		else
		{
			myConnectMessage.setString("Online: Disconnected");
		}

		if (CNetworking::GetInstance().GetIsNetworkEnabled() && CNetworking::GetInstance().GetIsClient())
		{
			time_t r = CNetworking::GetInstance().GetLastRecieveTime();

			if (time(nullptr) - r > 10)
			{
				CNetworking::GetInstance().Disconnect();
			}
		}

		myWindow->draw(myConnectMessage);

		myWindow->display();
	}

	HandleWindowEvents();
}

void CApplication::ShutDown()
{
	CNetworking::GetInstance().Disconnect();
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
