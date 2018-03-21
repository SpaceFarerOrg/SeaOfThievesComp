#include "GameObject.h"
#include "Renderer.h"

void CGameObject::Update(float aDT)
{
}

void CGameObject::Render()
{
	mySprite.setPosition(myTransform.getPosition());
	mySprite.setRotation(myTransform.getRotation());
	mySprite.setScale(myTransform.getScale());

	CRenderer::GetInstance().Render(mySprite);
}

sf::Transformable & CGameObject::GetTransform()
{
	return myTransform;
}

sf::Sprite & CGameObject::GetSprite()
{
	return mySprite;
}

void CGameObject::SetPosition(const sf::Vector2f & aPosition)
{
	myTransform.setPosition(aPosition);
}

void CGameObject::SetOriginMiddle()
{
	mySprite.setOrigin(mySprite.getLocalBounds().width / 2.f, mySprite.getLocalBounds().height / 2.f);
}

void CGameObject::SetOriginUpperLeft()
{
	mySprite.setOrigin(0.f, 0.f);
}
