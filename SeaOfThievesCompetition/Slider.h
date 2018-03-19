#pragma once
#include "UIBase.h"
#include "SFML/Graphics/Sprite.hpp"

namespace sf
{
	class RenderWindow;
}

class CSlider : public CUIBase
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

	static sf::Texture ourSliderTexture;
	static sf::Texture ourButtonTexture;
};

