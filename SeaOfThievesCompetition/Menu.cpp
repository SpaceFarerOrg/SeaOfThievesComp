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

	button.Init("Play", { x, y }, []() { CMenu::myMenuState = CMenu::EMenuState::StartGame; });
	myButtons.push_back(button);

	button.Init("Controls", { x, y + increment }, []() { CMenu::myMenuState = CMenu::EMenuState::Controls; });
	myButtons.push_back(button);

	button.Init("Online", { x, y + 2 * increment }, []() { CMenu::myMenuState = CMenu::EMenuState::Connect; });
	myButtons.push_back(button);

	button.Init("Settings", { x, y + 3 * increment }, []() { CMenu::myMenuState = CMenu::EMenuState::Settings; });
	myButtons.push_back(button);

	button.Init("Quit Game", { x, y + 4 * increment }, []() { CMenu::myMenuState = CMenu::EMenuState::Exit; });
	myButtons.push_back(button);

	button.Init("Connect", { myWindow->getView().getCenter().x + (float)myWindow->getSize().x / 10.f, y + increment * 2 }, [&]() { StartClient(); });
	myNetworkButtons.push_back(button);

	myEnterIpText.setFont(myFont);
	myEnterIpText.setFillColor(sf::Color(250, 253, 193));
	myEnterIpText.setPosition({ (float)myWindow->getSize().x / 10.f,  y + increment * 2 });
	myEnterIpText.setString("Enter IP: ");
	myEnterIpText.setCharacterSize(myWindow->getSize().x / 35);
	myEnterIpText.setOrigin(0, myEnterIpText.getLocalBounds().height / 2);

	myTextBox.Init({ (float)myWindow->getSize().x * 4.f / 10,  y + increment * 2 - 27 });
	
	button.Init("Start As Server", { myWindow->getView().getCenter().x - (float)myWindow->getSize().x / 5.f, y + 5 * increment }, []() {if (!CNetworking::GetInstance().GetIsNetworkEnabled()) { CNetworking::GetInstance().StartServer(); }});
	myNetworkButtons.push_back(button);


	myVolumeSlider.Init({myWindow->getSize().x / 3.f, y + increment });
	CApplication::SetVolume(myVolumeSlider.GetValue());
	myVolumeText.setString("Volume");
	myVolumeText.setFont(myFont);
	myVolumeText.setPosition(myWindow->getSize().x / 6, y + increment);
	myVolumeText.setFillColor(sf::Color(250, 253, 193));
	myVolumeText.setCharacterSize(myWindow->getSize().x / 35);
}

void CMenu::SetWindow(sf::RenderWindow * aWindow)
{
	myWindow = aWindow;
}

bool CMenu::Update()
{
	float dt = myDeltaTimer.getElapsedTime().asSeconds();
	myDeltaTimer.restart();
	dt = dt > 1.f ? 1.f : dt;
	myTotalTime += dt;

	myCamera.setSize(myWindow->getSize().x, myWindow->getSize().y);
	myCamera.setCenter((float)myWindow->getSize().x / 2.f, (float)myWindow->getSize().y / 2.f);
	myWindow->setView(myCamera);

	myTextBox.SetActive(false);

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
		myTextBox.SetActive(true);

		myTextBox.Render(*myWindow);

		myWindow->draw(myEnterIpText);

		for (CButton& button : myNetworkButtons)
		{
			button.Update(dt);
			button.Render(myWindow);
		}
		break;
	case EMenuState::Settings:
		myWindow->draw(myConnectSprite);

		myWindow->draw(myVolumeText);

		myVolumeSlider.Update(dt);
		myVolumeSlider.Render(myWindow);
		CApplication::SetVolume(myVolumeSlider.GetValue());

		break;
	case EMenuState::Exit:
		myShouldRun = false;
		break;
	}

	for (CButton& button : myButtons)
	{
		button.Update(dt);
		button.Render(myWindow);
	}

	return myShouldRun;
}

bool CMenu::GetShouldRun() const
{
	return myShouldRun;
}

CTextBox * CMenu::GetTextBox()
{
	return &myTextBox;
}

void CMenu::StartClient()
{
	sf::IpAddress ip(myTextBox.GetText());
	if (!CNetworking::GetInstance().GetIsNetworkEnabled())
	{
		CNetworking::GetInstance().ConnectToServer(ip);
	}
}

void CMenu::SetMenuState()
{
	myMenuState = EMenuState::Controls;
}
