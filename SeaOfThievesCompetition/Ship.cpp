#include "Ship.h"
#include <SFML\Window\Keyboard.hpp>
#include <SFML\Graphics\RenderWindow.hpp>
#include <cmath>
#include "Math.h"
#include <SFML\Graphics\RectangleShape.hpp>
#include "Network.h"

void CShip::Init(sf::Texture & aTexture)
{
	mySprite.setTexture(aTexture);

	mySprite.setOrigin(mySprite.getGlobalBounds().width / 2.f, mySprite.getGlobalBounds().height / 2.f);

	myAccelration = 100.f;
	myTurnSpeed = 10.f;
	myMaxSpeed = 200.f;

	myIsSinking = false;
	myIsDead = false;

	myCollisionPoints[0] = { mySprite.getPosition().x + mySprite.getOrigin().x / 2.f, mySprite.getPosition().y };
	myCollisionPoints[1] = { mySprite.getPosition().x - mySprite.getOrigin().x * 0.85f, mySprite.getPosition().y };
	myCollisionPoints[2] = { mySprite.getPosition().x, mySprite.getPosition().y + mySprite.getOrigin().y * 0.55f };
	myCollisionPoints[3] = { mySprite.getPosition().x, mySprite.getPosition().y - mySprite.getOrigin().y * 0.55f};

	myIsControlled = false;
}

void CShip::Update(float aDT)
{
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

	myTransform.setRotation(myRotation);

	if (CNetworking::GetInstance().GetIsNetworkEnabled())
	{
		CNetworking::GetInstance().SendMyTranslation(myTransform);
	}
}

void CShip::Render(sf::RenderWindow & aWindow)
{
	mySprite.setRotation(myTransform.getRotation());
	mySprite.setPosition(myTransform.getPosition());

	myWavesSprite.setRotation(myTransform.getRotation());
	myWavesSprite.setPosition(myTransform.getPosition());

	if (mySpeed > 20.f)
	{
		CAnimation& currentWaves = myWaves[(size_t)EWaves::Small];
		currentWaves.SetPosition(myTransform.getPosition());
		currentWaves.SetRotation(myTransform.getRotation());

		currentWaves.Render(aWindow);
	}
	aWindow.draw(myWavesSprite);
	aWindow.draw(mySprite);
}

void CShip::Respawn()
{
	mySprite.setColor({ 255,255,255,255 });
	myRotation = -45.f;
	mySpeed = 0.f;
	myIsSinking = false;
	myIsDead = false;
}


void CShip::Sink()
{
	//Ensure the ship can not sink twice
	if (myIsSinking)
		return;

	mySpeed = 0.f;
	myCurrentOpacity = 255.f;
	myIsSinking = true;
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

bool CShip::GetIsStill() const
{
	return mySpeed == 0.f;
}

void CShip::SetPosition(const sf::Vector2f & aPosition)
{
	myTransform.setPosition(aPosition);
}

void CShip::SetWavesTextures(sf::Texture & aSmallWaves, sf::Texture & aBigWaves)
{
	myWaves[(size_t)EWaves::Small].Init(aSmallWaves, 64, 0.1f);
	myWaves[(size_t)EWaves::Big].Init(aBigWaves, 64, 0.1f);
	myWavesSprite.setTexture(aSmallWaves);
	myWavesSprite.setOrigin(aSmallWaves.getSize().x / 2, aSmallWaves.getSize().y / 2);
}

