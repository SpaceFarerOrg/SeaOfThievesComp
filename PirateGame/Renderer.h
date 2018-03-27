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
	sf::Vector2f GetViewCenter() const; 
	sf::Vector2u GetWindowSize() const;
	sf::Vector2f GetViewSize() const;

	const sf::RenderWindow& GetWindow() const;

	void Render(sf::Drawable& aDrawable);
private:
	CRenderer();
	sf::RenderWindow* myWindow;
};