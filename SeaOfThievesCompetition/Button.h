#pragma once
#include "SFML/System/Vector2.hpp"
#include "SFML/Graphics/Text.hpp"
#include "SFML/Graphics/Font.hpp"

#include <functional>

namespace sf
{
	class RenderWindow;
}


class CButton
{
public:
	void Init(const sf::String& aTitle, const sf::Vector2f& aPosition, std::function<void(void)> aFunctionToCall);
	void Update(float aDT);
	void Render(sf::RenderWindow* aRenderWindow);

	static void SetFont(const sf::String& aFontName);
	static void SetWindow(sf::RenderWindow* aRenderWindow);

private:
	sf::Text myTitle;

	std::function<void(void)> myOnPressedFunction;

	static sf::Font ourFont;
	static sf::RenderWindow* ourWindow;
};