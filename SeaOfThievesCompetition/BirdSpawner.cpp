#include "BirdSpawner.h"
#include "SFML\Graphics\RenderWindow.hpp"
#include "Math.h"

void CBirdSpawner::Init()
{
	myBirdTexture.loadFromFile("sprites/Fogel.png");
	myBirdSprite.setTexture(myBirdTexture);
	myBirdSprite.setOrigin(myBirdTexture.getSize().x / 2, myBirdTexture.getSize().y / 2);

	CreateFormations();
	myCurrentFormation = GetRandomFormation();

	myDirection = sf::Vector2f(1, 0);

	mySpeed = 250.f;
}

void CBirdSpawner::Update(float aDT, const sf::Vector2f& aViewCenter)
{
	if (Math::Length(aViewCenter - myPosition) > 2000.f)
	{
		float angle = Math::GetRandomInRange(0, 360);
		angle = Math::ToRadians(angle);
		float distance = 1500.f;
		myPosition = sf::Vector2f(cosf(angle), -sinf(angle));
		myPosition *= distance;
		myPosition += aViewCenter;

		myDirection = aViewCenter - myPosition;
		Math::Normalize(myDirection);

		float rotation = Math::GetRandomInRange(-30, 30);
		rotation = Math::ToRadians(rotation);
		Math::RotateVector(myDirection, rotation);
	}

	myPosition += mySpeed * aDT * myDirection;
}

void CBirdSpawner::Render(sf::RenderWindow * aRenderWindow)
{
	myBirdSprite.setRotation(Math::ToDegrees(atan2f(myDirection.x, -myDirection.y)));

	static sf::Vector2f shadowOffset = sf::Vector2f(-20, 64);

	// Shadowz
	myBirdSprite.setColor(sf::Color(0, 0, 0, 150));
	myBirdSprite.setScale(0.35f, 0.35f);
	for (sf::Vector2f pos : myCurrentFormation->myPositions)
	{
		sf::Vector2f rotatedPos = myPosition + pos;
		Math::RotateVectorAroundPoint(rotatedPos, myPosition, Math::ToRadians(myBirdSprite.getRotation()));
		myBirdSprite.setPosition(rotatedPos + shadowOffset);
		aRenderWindow->draw(myBirdSprite);
	}


	// Real Birdz
	myBirdSprite.setColor(sf::Color(255, 255, 255, 255));
	myBirdSprite.setScale(1, 1);
	for (sf::Vector2f pos : myCurrentFormation->myPositions)
	{
		sf::Vector2f rotatedPos = myPosition + pos;
		Math::RotateVectorAroundPoint(rotatedPos, myPosition, Math::ToRadians(myBirdSprite.getRotation()));
		myBirdSprite.setPosition(rotatedPos);
		aRenderWindow->draw(myBirdSprite);
	}
}

CBirdSpawner::SFormation * CBirdSpawner::GetRandomFormation()
{
	int index = Math::GetRandomInRange(0, myFormationPool.size() - 1);
	return &myFormationPool[index];
}

void CBirdSpawner::CreateFormations()
{
	myFormationPool.push_back({ {
								sf::Vector2f(0,0),
					sf::Vector2f(-100, 64), sf::Vector2f(100, 64),
		sf::Vector2f(-200, 128),						sf::Vector2f(200, 128)
		} });

	myFormationPool.push_back({ {
								sf::Vector2f(0,0),
					sf::Vector2f(-100, 64), sf::Vector2f(100, 64),
			sf::Vector2f(-200, 128), sf::Vector2f(0, 128),	sf::Vector2f(200, 128),
					sf::Vector2f(-100, 196), sf::Vector2f(100, 196),
								sf::Vector2f(0, 260)
		} });
}
