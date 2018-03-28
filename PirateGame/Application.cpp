#include "Application.h"
#include <SFML\Graphics\RenderWindow.hpp>
#include <SFML\Window\Event.hpp>
#include "SFML\Window\Mouse.hpp"
#include "TextureBank.h"
#include "AudioSystem.h"

bool CApplication::myIsInGame;
bool CApplication::myHasChangedState;
float CApplication::myMasterVolume;

void CApplication::Init()
{
	myHasChangedState = false;
	myShouldClose = false;
	myIsWindowActive = true;

	sf::VideoMode vm;
	vm = sf::VideoMode::getDesktopMode();

	myFont.loadFromFile("font/font.ttf");
	myConnectMessage.setFont(myFont);
	myConnectMessage.setOutlineThickness(2.f);
	myConnectMessage.setFillColor(sf::Color(250, 253, 193));
	myConnectMessage.setOutlineColor(sf::Color::Black);

	vm.bitsPerPixel = sf::VideoMode::getDesktopMode().bitsPerPixel;

	myWindow.create(vm, "Sea of Thieves Competition", sf::Style::Close);

	CRenderer::GetInstance().BindWindow(myWindow);

	CTextureBank::GetInstance().LoadAllGraphicItems();
	CAudioSystem::GetInstance().LoadAllAudio();

	myGame.SetWindow(&myWindow);
	myMenu.SetWindow(&myWindow);

	myGame.Init();
	myMenu.Init();

	myMenuTextBox = myMenu.GetTextBox();
	myMenuNameBox = myMenu.GetNameBox();

	myCursorTexture.loadFromFile("sprites/cursor.png");
	myCursorSprite.setTexture(myCursorTexture);

	myWindow.setMouseCursorVisible(false);

	myScreenSpaceView = myWindow.getView();
	myNetworkThread = std::thread([&]()->void { UpdateNetworking(); });

	CNetworking::GetInstance().SetGame(&myGame);
}

void CApplication::Update()
{
	CAudioSystem::GetInstance().Update();
	CRenderer::GetInstance().Update();

	if (myIsWindowActive)
	{
		bool isMultiplayerClient = CNetworking::GetInstance().GetIsNetworkEnabled() && CNetworking::GetInstance().GetIsClient();

		myWindow.clear({ 95,189,197 });
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
			myGame.ReInit();
		}

		if (myIsInGame)
		{
			myShouldClose = !myGame.Update();
		}
		else
		{
			myShouldClose = !myMenu.Update();
		}

		sf::Vector2i mPos = sf::Mouse::getPosition(myWindow);
		myCursorSprite.setPosition(mPos.x, mPos.y);
		myWindow.setView(myScreenSpaceView);
		myWindow.draw(myCursorSprite);

		if (CNetworking::GetInstance().GetIsNetworkEnabled() && CNetworking::GetInstance().GetIsClient())
		{
			if (CNetworking::GetInstance().GetIsWelcomed())
			{
				myConnectMessage.setString("Online: Connected");
			}
			else
			{
				myConnectMessage.setString("Online: Waiting for Server");
			}
		}
		else if (CNetworking::GetInstance().GetIsNetworkEnabled() && !CNetworking::GetInstance().GetIsClient())
		{
			myConnectMessage.setString("Online: Is Server");
		}
		else
		{
			myConnectMessage.setString("Online: Disconnected");
		}


		myWindow.draw(myConnectMessage);

		myWindow.display();
	}

	if (CNetworking::GetInstance().GetIsNetworkEnabled() && CNetworking::GetInstance().GetIsClient())
	{
		time_t r = CNetworking::GetInstance().GetLastRecieveTime();

		if (time(nullptr) - r > 10)
		{
			CNetworking::GetInstance().Disconnect();
		}
	}
	else if (CNetworking::GetInstance().GetIsNetworkEnabled() && !CNetworking::GetInstance().GetIsClient())
	{
		CNetworking::GetInstance().DoPingUpdate();
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

	while (myWindow.pollEvent(e))
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
				myMenuNameBox->AddText(e.text.unicode);
			}
			else
			{
				myMenuTextBox->RemoveCharacter();
				myMenuNameBox->RemoveCharacter();
			}
		}
	}
}
