#include "Menu.h"
#include <SFML\Graphics\RenderWindow.hpp>
#include <SFML\Window\Keyboard.hpp>
#include "Application.h"
#include <SFML\Window\Event.hpp>

void CMenu::Init()
{
	myFont.loadFromFile("font/font.ttf");
	myText.setFont(myFont);

	/*Menu Text*/
	sf::String a = "Press enter to generate island and start\n";
	sf::String controllers = "\nControlls:\n\tSpeed Up & Down - Up & Down Arrow\n\tTurn Left & Right - Left & Right Arrow\n\tMap - M\n\tReturn To Menu - Esc";
	sf::String about = "\n\nCollect bounty, sell for gold at the habited island\nUpgrade your ship and become a Pirate Legend";

	myText.setString(a + controllers + about);

	myShouldRun = true;
}

void CMenu::SetWindow(sf::RenderWindow * aWindow)
{
	myWindow = aWindow;
}

void CMenu::Update()
{
	HandleWindowEvents();

	myCamera.setCenter((float)myWindow->getSize().x/2.f, (float)myWindow->getSize().y/2.f);
	myWindow->setView(myCamera);

	myText.setOrigin(myText.getGlobalBounds().width / 2.f, myText.getGlobalBounds().height / 2.f);
	myText.setPosition(myCamera.getCenter());

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Return))
	{
		CApplication::StartGame();
	}

	myWindow->draw(myText);
}

bool CMenu::GetShouldRun() const
{
	return myShouldRun;
}

void CMenu::HandleWindowEvents()
{
	sf::Event e;

	while (myWindow->pollEvent(e))
	{
		if (e.type == sf::Event::Closed)
		{
			myShouldRun = false;
		}
	}

}
