#pragma once
#include <SFML\Graphics\Transformable.hpp>
#include <SFML\Graphics\Sprite.hpp>
#include "Renderer.h"

class CGameObject
{
public:
	virtual void Init() = 0;
	virtual void Update(float aDT);
	virtual void Render();

	sf::Transformable& GetTransform();
	sf::Sprite& GetSprite();

	void SetPosition(const sf::Vector2f& aPosition);


	void SetOriginMiddle();
	void SetOriginUpperLeft();
protected:
	sf::Sprite mySprite;
	sf::Transformable myTransform;
};