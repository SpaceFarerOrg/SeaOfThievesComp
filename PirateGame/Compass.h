#pragma once
#include <SFML\Graphics\Sprite.hpp>

class CCompass
{
public:
	void Init();
	void Update(float aDT, float aPlayerRotation);
	void Render();
private:
	sf::Sprite myCompassBase;
	sf::Sprite myCompassPointer;

	float myTargetRotation;
};