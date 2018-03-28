#pragma once
#include <SFML\Graphics\Drawable.hpp>
#include <SFML\Graphics\View.hpp>
#include <SFML\System\Clock.hpp>

namespace sf
{
	class RenderWindow;
}

class CRenderer
{
public:
	static CRenderer& GetInstance();
	void BindWindow(sf::RenderWindow& aWindow);
	sf::Vector2f GetWorldViewCenter() const; 
	sf::Vector2u GetWindowSize() const;
	sf::Vector2f GetViewSize() const;
	sf::Vector2f GetWindowCenter() const;


	const sf::RenderWindow& GetWindow() const;

	void SetPosition(const sf::Vector2f& aPosition);
	void SetPosition(float aX, float aY);
	void SetRotation(float aRotation);

	void RenderInScreenSpace(sf::Drawable& aDrawable);
	void Update();
	void Render(sf::Drawable& aDrawable);
private:
	CRenderer();

	sf::Clock myDeltaTimer;

	float myCurrentRotation;
	float myTargetRotation;

	sf::View myScreenSpaceView;
	sf::View myWorldSpaceView;
	sf::RenderWindow* myWindow;
};