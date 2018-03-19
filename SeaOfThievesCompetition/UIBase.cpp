#include "UIBase.h"
#include "SFML\Graphics\RenderWindow.hpp"

sf::Font CUIBase::ourFont;
sf::RenderWindow* CUIBase::ourWindow;

CUIBase::CUIBase()
{
}


CUIBase::~CUIBase()
{
}


void CUIBase::SetFont(const sf::String& aFontName)
{
	ourFont.loadFromFile(aFontName);
}

void CUIBase::SetWindow(sf::RenderWindow* aRenderWindow)
{
	ourWindow = aRenderWindow;
}