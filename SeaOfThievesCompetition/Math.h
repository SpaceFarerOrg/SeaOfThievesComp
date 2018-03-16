#pragma once
#include <random>

namespace Math
{
	static std::random_device random;

	static float ToRadians(float aDegrees)
	{
		return aDegrees * (3.141f / 180.f);
	}

	static float Lerp(float aStartValue, float aEndValue, float aPercentage)
	{
		return (1.f - aPercentage) * aStartValue + (aPercentage * aEndValue);//(aStartValue + aPercentage * (aEndValue - aStartValue));
	}

	static short GetRandomInRange(short aMin, short aMax)
	{
		std::uniform_int_distribution<> dist(aMin, aMax);

		short rnum = (short)dist(random);

		return rnum;
	}

	static float Length(const sf::Vector2f& aVec)
	{
		return sqrtf(aVec.x * aVec.x + aVec.y * aVec.y);
	}

	static bool Chance(short aPercent)
	{
		std::uniform_int_distribution<> dist(0, 100);
		short rnum = (short)dist(random);

		if (aPercent > rnum)
		{
			return true;
		}

		return false;
	}

}
