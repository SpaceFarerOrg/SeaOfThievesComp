#pragma once
#include "SFML/System/Vector2.hpp"
#include "SFML/Graphics/Font.hpp"

namespace sf
{
	class RenderWindow;
}

class CUIBase
{
public:
	CUIBase();
	~CUIBase();

	static void SetFont(const sf::String& aFontName);
	static void SetWindow(sf::RenderWindow* aRenderWindow);

protected:

	static sf::Font ourFont;
	static sf::RenderWindow* ourWindow;
};

