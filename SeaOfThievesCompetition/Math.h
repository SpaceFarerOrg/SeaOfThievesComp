#pragma once
#include <random>

namespace Math
{
	static std::random_device random;

	static float ToRadians(float aDegrees)
	{
		return aDegrees * (3.141f / 180.f);
	}

	static float ToDegrees(float aRadians)
	{
		return aRadians / (3.14f / 180.f);
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

	static void Normalize(sf::Vector2f& aVec)
	{
		float l = Length(aVec);
		aVec.x /= l;
		aVec.y /= l;
	}

	static void RotateVector(sf::Vector2f& aVec, float aRotation)
	{
		float newX = aVec.x * cosf(aRotation) - aVec.y * sinf(aRotation);
		float newY = aVec.x * sinf(aRotation) + aVec.y * cosf(aRotation);

		aVec = sf::Vector2f(newX, newY);
	}

	static void RotateVectorAroundPoint(sf::Vector2f& aVec, const sf::Vector2f& aPoint, float aRotation)
	{
		aVec -= aPoint;
		RotateVector(aVec, aRotation);
		aVec += aPoint;
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
