#include "PopupText.h"
#include "TextureBank.h"
#include "Renderer.h"

CPopupText::CPopupText()
{
	myShouldShow = false;
}

void CPopupText::Init(const sf::String & aText, float aTimeToShow)
{
	myText.setFont(CTextureBank::GetInstance().GetFont());

	myText.setString(aText);
	myTimeToShow = aTimeToShow;

	myText.setFillColor(sf::Color::White);
	myText.setOutlineThickness(2.f);
	myText.setOutlineColor(sf::Color::Black);
	myText.setCharacterSize(50);
}

void CPopupText::SetString(const sf::String & aText)
{
	myText.setString(aText);
}

void CPopupText::Display()
{
	myShouldShow = true;
	myText.setOrigin(myText.getGlobalBounds().width / 2.f, myText.getGlobalBounds().height / 2.f);
	myTimer.restart();
	myTimeShown = 0.f;
}

void CPopupText::Hide()
{
	myShouldShow = false;
}

void CPopupText::Render()
{
	if (!myShouldShow)
		return;

	myText.setPosition(CRenderer::GetInstance().GetViewCenter());

	float dt = myTimer.getElapsedTime().asSeconds();
	myTimer.restart();
	myTimeShown += dt;

	CRenderer::GetInstance().Render(myText);

	if (myTimeShown >= myTimeToShow)
	{
		sf::Uint8 alpha = myText.getFillColor().a;

		float diff = myTimeShown - myTimeToShow;
		alpha -= (sf::Uint8)(diff);
		
		if (alpha <= 0)
		{
			alpha = 255;
			Hide();
		}

		myText.setFillColor({ 255, 255, 255, alpha });
		myText.setOutlineColor({ 0,0,0,alpha });
	}
}
