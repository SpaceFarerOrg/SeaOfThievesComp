#pragma once
#include <SFML\System\String.hpp>
#include <SFML\Graphics\Text.hpp>
#include <SFML\Graphics\RenderWindow.hpp>
#include <SFML\Graphics\RectangleShape.hpp>

class CTextBox
{
public:
	void Init(const sf::Vector2f& aPosition);
	void Clear();
	void Update();
	void SetActive(bool aState);
	void AddText(sf::Uint32 aTextAdded);
	void RemoveCharacter();
	const sf::String& GetText() const;
	void Render(sf::RenderWindow& aWindow);
	void SetWindow(sf::RenderWindow* aWindow);
private:
	sf::Vector2f myPosition;
	sf::Font myFont;
	sf::Text myGraphicsText;
	sf::RectangleShape myShape;
	sf::RenderWindow* myWindow;

	bool myIsActive;
	sf::String myText;
};