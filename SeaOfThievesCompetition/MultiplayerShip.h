#pragma once
#include <SFML\Graphics\Sprite.hpp>

namespace sf
{
	class RenderWindow;
}

class CMultiplayerShip
{
public:
	void Init(sf::Texture& aTexture);
	void Render(sf::RenderWindow& aWindow);
private:
	size_t myPlayerID;
	sf::Sprite mySprite;
};