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
	//sf::String a = "Press enter to generate island and start\n";
	//sf::String controllers = "\nControlls:\n\tSpeed Up & Down - Up & Down Arrow\n\tTurn Left & Right - Left & Right Arrow\n\tMap - M\n\tReturn To Menu - Esc";
	//sf::String about = "\n\nCollect bounty, sell for gold at the habited island\nUpgrade your ship and become a Pirate Legend";
	//
	//myText.setString(a + controllers + about);

	myMenuTexture.loadFromFile("sprites/controls.png");
	myMenuSprite.setTexture(myMenuTexture);

	myPressEnterTexture.loadFromFile("sprites/pressEnter.png");
	myPressEnterPrompt.setTexture(myPressEnterTexture);

	myPressEnterPrompt.setOrigin(myPressEnterTexture.getSize().x / 2, myPressEnterTexture.getSize().y / 2);

	myMenuSprite.setPosition(0, 0);
	myMenuSprite.setScale((float)myWindow->getSize().x / myMenuTexture.getSize().x, (float)myWindow->getSize().y / myMenuTexture.getSize().y);

	myPressEnterPrompt.setScale(myMenuSprite.getScale());

	myPressEnterPrompt.setPosition(myWindow->getSize().x / 2, myWindow->getSize().y - 2 * myPressEnterTexture.getSize().y);

	myShouldRun = true;
	myTotalTime = 0.f;
}

void CMenu::SetWindow(sf::RenderWindow * aWindow)
{
	myWindow = aWindow;
}

void CMenu::Update()
{
	float dt = myDeltaTimer.getElapsedTime().asSeconds();
	myDeltaTimer.restart();
	dt = dt > 1.f ? 1.f : dt;
	myTotalTime += dt;

	myPressEnterPrompt.setRotation(4.f * sinf(myTotalTime));

	myCamera.setSize(myWindow->getSize().x, myWindow->getSize().y);
	myCamera.setCenter((float)myWindow->getSize().x/2.f, (float)myWindow->getSize().y/2.f);
	myWindow->setView(myCamera);

	//myText.setOrigin(myText.getGlobalBounds().width / 2.f, myText.getGlobalBounds().height / 2.f);
	//myText.setPosition(myCamera.getCenter());

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Return))
	{
		CApplication::StartGame();
	}

	//myWindow->draw(myText);
	myWindow->draw(myMenuSprite);
	myWindow->draw(myPressEnterPrompt);
}

bool CMenu::GetShouldRun() const
{
	return myShouldRun;
}
