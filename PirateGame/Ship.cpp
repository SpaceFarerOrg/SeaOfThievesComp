#include "Ship.h"
#include <SFML\Window\Keyboard.hpp>
#include <SFML\Graphics\RenderWindow.hpp>
#include <cmath>
#include "Math.h"
#include <SFML\Graphics\RectangleShape.hpp>
#include "Network.h"
#include "TextureBank.h"


void CShip::Init()
{
	myHasRespawned = false;
	mySprite.setTexture(GET_TEXTURE(ETexture::Ship));

	SetOriginMiddle();

	myAccelration = 100.f;
	myTurnSpeed = 40.f;
	myMaxSpeed = 200.f;

	myIsSinking = false;
	myIsDead = false;

	myCollisionPoints[0] = { mySprite.getPosition().x + mySprite.getOrigin().x / 2.f, mySprite.getPosition().y };
	myCollisionPoints[1] = { mySprite.getPosition().x - mySprite.getOrigin().x * 0.85f, mySprite.getPosition().y };
	myCollisionPoints[2] = { mySprite.getPosition().x, mySprite.getPosition().y + mySprite.getOrigin().y * 0.55f };
	myCollisionPoints[3] = { mySprite.getPosition().x, mySprite.getPosition().y - mySprite.getOrigin().y * 0.55f };

	myIsControlled = false;
}

void CShip::Update(float aDT)
{
	myInvisibilityTimer += aDT;

	for (unsigned i = 0; i < 4; ++i)
	{
		myTransformedCP[i] = myTransform.getTransform().transformPoint(myCollisionPoints[i]);
	}

	if (myIsSinking)
	{
		myCurrentOpacity -= 50.f * aDT;

		if (myCurrentOpacity <= 0.f)
		{
			myCurrentOpacity = 0.f;
			myIsDead = true;
		}

		mySprite.setColor({ 255,255,255, (sf::Uint8)myCurrentOpacity });
		return;
	}


	for (CAnimation& animation : myWaves)
	{
		animation.Update(aDT);
	}

	myIsControlled = false;

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
	{
		mySpeed += myAccelration * aDT;
		myIsControlled = true;
	}
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
	{
		mySpeed -= myAccelration * aDT;
		myIsControlled = true;
	}

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
	{
		myRotation += myTurnSpeed * aDT;
		myIsControlled = true;
	}
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
	{
		myRotation -= myTurnSpeed * aDT;
		myIsControlled = true;
	}

	if (mySpeed > myMaxSpeed)
	{
		mySpeed = myMaxSpeed;
	}
	else if (mySpeed < 0.f)
	{
		mySpeed = 0.f;
	}

	sf::Vector2f direction;
	direction.x = cosf(Math::ToRadians(myRotation));
	direction.y = sinf(Math::ToRadians(myRotation));

	myTransform.move(direction * mySpeed * aDT);
	myTransform.move(myWhirlwindDrag * aDT);
	myTransform.move(myShipNudge * aDT);

	myTransform.setRotation(myRotation);

	if (CNetworking::GetInstance().GetIsNetworkEnabled())
	{
		CNetworking::GetInstance().SendMyTranslation(myTransform);
	}
}

void CShip::Render()
{
	CAnimation& currentWaves = myWaves[(size_t)EWaves::Small];

	sf::Color color = Math::Lerp(sf::Color::Transparent, sf::Color::White, mySpeed / myMaxSpeed);

	currentWaves.SetOpacity(color.a);

	currentWaves.SetPosition(myTransform.getPosition());
	currentWaves.SetRotation(myTransform.getRotation());

	currentWaves.Render();

	CGameObject::Render();
}

void CShip::Respawn()
{
	mySprite.setColor({ 255,255,255,255 });
	myRotation = -45.f;
	mySpeed = 0.f;
	myIsSinking = false;
	myIsDead = false;
	myInvisibilityTimer = 0.f;
	myHasTreasure = false;
	myHasRespawned = true;

	for (unsigned i = 0; i < 4; ++i)
	{
		myTransformedCP[i] = myTransform.getTransform().transformPoint(myCollisionPoints[i]);
	}
}


void CShip::Sink()
{
	if (myInvisibilityTimer < 5.f)
		return;

	//Ensure the ship can not sink twice
	if (myIsSinking)
		return;

	mySpeed = 0.f;
	myCurrentOpacity = 255.f;
	myIsSinking = true;
}

float CShip::GetWidth() const
{
	return mySprite.getLocalBounds().height;
}

bool CShip::GetIsDead() const
{
	return myIsDead;
}

bool CShip::GetIsSinking() const
{
	return myIsSinking;
}

const std::array<sf::Vector2f, 4>& CShip::GetCollisionPoints() const
{
	return myTransformedCP;
}

bool CShip::GetIsControlled() const
{
	return myIsControlled;
}

sf::Vector2f CShip::GetPosition() const
{
	return std::move(myTransform.getPosition());
}

void CShip::SetHoldsTreasure(bool aDoesIt)
{
	myHasTreasure = aDoesIt;
}

bool CShip::GetHasTreasure() const
{
	return myHasTreasure;
}

void CShip::SetWhirlwindDrag(const sf::Vector2f & aDrag)
{
	myWhirlwindDrag = aDrag;
}

void CShip::SetShipNudge(const sf::Vector2f & aNudge)
{
	myShipNudge = aNudge;
}

bool CShip::GetIsStill() const
{
	return mySpeed == 0.f;
}

void CShip::SetPosition(const sf::Vector2f & aPosition)
{
	myTransform.setPosition(aPosition);
}

void CShip::SetWavesTextures(const sf::Texture & aSmallWaves,const sf::Texture & aBigWaves)
{
	myWaves[(size_t)EWaves::Small].Init(aSmallWaves, aSmallWaves.getSize().x / 3, aSmallWaves.getSize().y, 0.1f);
	myWaves[(size_t)EWaves::Big].Init(aBigWaves, aBigWaves.getSize().x / 3, aBigWaves.getSize().y, 0.1f);

}

bool CShip::GetIsInvincible() const
{
	return myInvisibilityTimer < 5.f;
}

bool CShip::GetHasRespawned()
{
	bool returnVal = myHasRespawned;
	myHasRespawned = false;
	return returnVal;
}

