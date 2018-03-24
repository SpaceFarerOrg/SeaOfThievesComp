#pragma once
#include "SFML/Graphics/Sprite.hpp"

namespace sf
{
	class RenderWindow;
}

class CSlider
{
public:
	CSlider();
	~CSlider();

	void Init(const sf::Vector2f& aPosition);

	float GetValue();
	void Update(float aDT);
	void Render(sf::RenderWindow* aRenderWindow);


private:
	float myValue;
	sf::Vector2f myPosition;

	sf::Sprite mySliderSprite;
	sf::Sprite myButtonSprite;
};

