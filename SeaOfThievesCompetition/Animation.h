#pragma once
#include <sfml\Graphics\Sprite.hpp>


namespace sf
{
	class RenderWindow;
}

class CAnimation
{
public:
	void Init(sf::Texture& aTexture, short aFrameSize, short aFrameHeight, float aFrameTime);
	void RandomizeStartFrame();

	void Update(float aDT);
	void Render(sf::RenderWindow& aWindow);

	void SetOpacity(short aOpacity);

	void SetPosition(const sf::Vector2f& aPosition);
	void SetRotation(float aRotation);
private:
	void SetAppropriateFrame();

	sf::Transformable myTransform;

	short myCurrentFrame;
	float myCurrentFrameTime;
	float myFrameTime;
	short myFrameSize;
	short myFrameHeight;
	sf::Sprite mySprite;
};