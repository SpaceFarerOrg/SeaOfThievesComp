#include "Animation.h"
#include <SFML\Graphics\RenderWindow.hpp>
#include "Math.h"

void CAnimation::Init(sf::Texture & aTexture, short aFrameSize, short aFrameheight, float aFrameTime)
{
	mySprite.setTexture(aTexture);
	myFrameSize = aFrameSize;
	myFrameHeight = aFrameheight;
	myFrameTime = aFrameTime;
	myCurrentFrame = 0;

	mySprite.setOrigin({ (float)myFrameSize / 2, (float)myFrameHeight / 2 });

	SetAppropriateFrame();
}

void CAnimation::RandomizeStartFrame()
{
	myCurrentFrame = 0;
	myCurrentFrame = Math::GetRandomInRange(0, 2);
	myCurrentFrameTime = myFrameTime * (float)Math::GetRandomInRange(0, 100) / 100.f;
	SetAppropriateFrame();
}

void CAnimation::SetPosition(const sf::Vector2f & aPosition)
{
	myTransform.setPosition(aPosition);
}

void CAnimation::SetRotation(float aRotation)
{
	myTransform.setRotation(aRotation);
}

void CAnimation::Update(float aDT)
{
	myCurrentFrameTime += aDT;

	if (myCurrentFrameTime >= myFrameTime)
	{
		myCurrentFrameTime = 0.f;
		myCurrentFrame++;
		myCurrentFrame %= 3;
		SetAppropriateFrame();
	}

}

void CAnimation::Render(sf::RenderWindow & aWindow)
{
	mySprite.setPosition(myTransform.getPosition());
	mySprite.setRotation(myTransform.getRotation());

	aWindow.draw(mySprite);
}

void CAnimation::SetOpacity(short aOpacity)
{
	mySprite.setColor({ 255,255,255, (sf::Uint8)aOpacity });
}

void CAnimation::SetAppropriateFrame()
{
	sf::IntRect textureRect;
	textureRect.width = myFrameSize;
	textureRect.height = myFrameHeight;
	textureRect.left = myFrameSize * myCurrentFrame;
	textureRect.top = 0;

	mySprite.setTextureRect(textureRect);
}
