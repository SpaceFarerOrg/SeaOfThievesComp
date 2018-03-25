#include "Slider.h"
#include "SFML\Graphics\RenderWindow.hpp"
#include "SFML\Window\Mouse.hpp"
#include "Math.h"
#include "TextureBank.h"
#include "Renderer.h"

CSlider::CSlider()
{
}


CSlider::~CSlider()
{
}

void CSlider::Init(const sf::Vector2f & aPosition)
{
	mySliderSprite.setTexture(CTextureBank::GetInstance().GetTexture(ETexture::Slider));
	myButtonSprite.setTexture(CTextureBank::GetInstance().GetTexture(ETexture::Handle));

	myButtonSprite.setOrigin(myButtonSprite.getLocalBounds().width / 2.f, 0.f);

	myValue = 50.f;
	myPosition = aPosition;
}

float CSlider::GetValue()
{
	return myValue;
}

void CSlider::Update(float aDT)
{
	sf::Vector2i mPos = sf::Mouse::getPosition(CRenderer::GetInstance().GetWindow());

	if (mySliderSprite.getGlobalBounds().contains(mPos.x, mPos.y))
	{
		if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
		{
			myValue = 100.f * (mPos.x - mySliderSprite.getPosition().x) / mySliderSprite.getLocalBounds().width;
		}
	}
}

void CSlider::Render(sf::RenderWindow * aRenderWindow)
{
	sf::Vector2f valueOffset = myValue / 100.f * sf::Vector2f(mySliderSprite.getLocalBounds().width, 0);

	valueOffset.x = Math::Clamp(valueOffset.x, myButtonSprite.getLocalBounds().width / 2.f, mySliderSprite.getLocalBounds().width - myButtonSprite.getLocalBounds().width / 2.f);

	mySliderSprite.setPosition(myPosition);
	myButtonSprite.setPosition(myPosition + valueOffset);

	CRenderer::GetInstance().Render(mySliderSprite);
	CRenderer::GetInstance().Render(myButtonSprite);
}
