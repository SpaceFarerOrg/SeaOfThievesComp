#pragma once
#include <SFML\Graphics\Drawable.hpp>

namespace sf
{
	class RenderWindow;
}

class CRenderer
{
public:
	static CRenderer& GetInstance();
	void BindWindow(sf::RenderWindow& aWindow);
	void Render(sf::Drawable& aDrawable);
private:
	CRenderer();
	sf::RenderWindow* myWindow;
};