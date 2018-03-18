#pragma once
#include "SFML/Graphics/Text.hpp"

#include "UIBase.h"

#include <functional>

class CButton : public CUIBase
{
public:
	void Init(const sf::String& aTitle, const sf::Vector2f& aPosition, std::function<void(void)> aFunctionToCall);
	void Update(float aDT);
	void Render(sf::RenderWindow* aRenderWindow);

private:
	sf::Text myTitle;

	std::function<void(void)> myOnPressedFunction;

	sf::Color myColor;
	sf::Color myShadowColor;
	sf::Color myHighlightedColor;

	bool myIsHighlighted;
};