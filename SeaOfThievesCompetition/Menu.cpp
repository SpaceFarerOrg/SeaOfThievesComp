#include "Menu.h"
#include <SFML\Graphics\RenderWindow.hpp>
#include <SFML\Window\Keyboard.hpp>
#include "Application.h"
#include <SFML\Window\Event.hpp>

CMenu::EMenuState CMenu::myMenuState = CMenu::EMenuState::Controls;

void CMenu::Init()
{
	myFont.loadFromFile("font/font.ttf");
	myText.setFont(myFont);

	/*Menu Text*/
	//sf::String a = "Press enter to generate island and start\n";
	//sf::String controllers = "\nControlls:\n\tSpeed Up & Down - Up & Down Arrow\n\tTurn Left & Right - Left & Right Arrow\n\tMap - M\n\tReturn To Menu - Esc";
	//sf::String about = "\n\nCollect bounty, sell for gold at the habited island\nUpgrade your ship and become a Pirate Legend";
	//
	//myText.setString(a + controllers + about);

	myMenuTexture.loadFromFile("sprites/controls.png");
	myMenuSprite.setTexture(myMenuTexture);

	myMenuSprite.setPosition(0, 0);
	myMenuSprite.setScale((float)myWindow->getSize().x / myMenuTexture.getSize().x, (float)myWindow->getSize().y / myMenuTexture.getSize().y);

	myConnectTexture.loadFromFile("sprites/connect.png");
	myConnectSprite.setTexture(myConnectTexture);
	myConnectSprite.setScale(myMenuSprite.getScale());

	myShouldRun = true;
	myTotalTime = 0.f;

	CButton button;
	float x = (float)myWindow->getSize().x * 8.2f / 10.f;
	float y = (float)myWindow->getSize().y / 3.5f;
	float increment = (float)myWindow->getSize().y / 10.f;

	button.Init("Play", {x, y}, []() { CMenu::myMenuState = CMenu::EMenuState::StartGame; });
	myButtons.push_back(button);

	button.Init("Controls", { x, y + increment }, []() { CMenu::myMenuState = CMenu::EMenuState::Controls; });
	myButtons.push_back(button);

	button.Init("Connect", { x, y + 2 * increment }, []() { CMenu::myMenuState = CMenu::EMenuState::Connect; });
	myButtons.push_back(button);
}

void CMenu::SetWindow(sf::RenderWindow * aWindow)
{
	myWindow = aWindow;
}

#include <iostream>
void CMenu::Update()
{
	float dt = myDeltaTimer.getElapsedTime().asSeconds();
	myDeltaTimer.restart();
	dt = dt > 1.f ? 1.f : dt;
	myTotalTime += dt;

	myCamera.setSize(myWindow->getSize().x, myWindow->getSize().y);
	myCamera.setCenter((float)myWindow->getSize().x/2.f, (float)myWindow->getSize().y/2.f);
	myWindow->setView(myCamera);

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Return))
	{
		CApplication::StartGame();
	}
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::N) && CNetworking::GetInstance().GetIsNetworkEnabled() == false)
	{
		CNetworking::GetInstance().StartServer();
	}
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::M) && CNetworking::GetInstance().GetIsNetworkEnabled() == false)
	{
		std::string ip;
		std::cout << "Enter ip" << std::endl;
		std::cin >> ip;
		sf::IpAddress ipa(ip);

		CNetworking::GetInstance().ConnectToServer(ipa);
	}

	switch (myMenuState)
	{
	case EMenuState::StartGame:
		CApplication::StartGame();
		break;
	case EMenuState::Controls:
		myWindow->draw(myMenuSprite);
		break;
	case EMenuState::Connect:
		myWindow->draw(myConnectSprite);
		break;
	}

	for (CButton& button : myButtons)
	{
		button.Update(dt);
		button.Render(myWindow);
	}
}

bool CMenu::GetShouldRun() const
{
	return myShouldRun;
}
