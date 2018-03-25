#pragma once
#include <vector>
#include "SFML\System\Vector2.hpp"
#include "SFML\Graphics\Sprite.hpp"
#include "SFML\Graphics\Texture.hpp"

namespace sf
{
	class RenderWindow;
}

#define MAX_BIRDS_IN_FORMATION 15

class CBirdSpawner
{

public:
	void Init();
	void Update(float aDT, const sf::Vector2f& aViewCenter);
	void Render(sf::RenderWindow* aRenderWindow);

private:

	struct SFormation
	{
		std::vector<sf::Vector2f> myPositions;
	};

	SFormation* myCurrentFormation;
	sf::Vector2f myPosition;
	sf::Vector2f myDirection;
	float mySpeed;

	sf::Texture myBirdTexture;
	sf::Sprite myBirdSprite;

	std::vector<SFormation> myFormationPool;

	SFormation* GetRandomFormation();
	void CreateFormations();
};