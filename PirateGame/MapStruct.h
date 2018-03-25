#pragma once
#include "Defines.h"
#include <array>

struct SMap
{
	std::array<int, MAP_AXIS_SIZE * MAP_AXIS_SIZE> myMap;
	std::array<float, MAP_AXIS_SIZE*MAP_AXIS_SIZE> myRotationLUT;

	inline const size_t GetSize() const
	{
		return myMap.size();
	}

	int operator[](size_t aInt) const
	{
		return myMap[aInt];
	}

	int& operator[](size_t aInt)
	{
		return myMap[aInt];
	}

	float& operator()(size_t aInt)
	{
		return myRotationLUT[aInt];
	}
};