#pragma once
#include "Defines.h"
#include "Island.h"
#include <array>
#include <vector>

class CUIMap;

class CWorld
{
public:
	void CreateFormArray(const std::array<int, MAP_AXIS_SIZE*MAP_AXIS_SIZE>& aMap, CUIMap& aUIMap);
	void Generate(CUIMap& aUIMap);
	void Render();

	void PlaceTreasure(CUIMap& aUIMap);

	std::vector<CIsland>& GetIslands();

	sf::Vector2f GetSpawnPosition() const;
	sf::Vector2f GetRandomSeaPosition() const;

	const std::array<int, MAP_AXIS_SIZE*MAP_AXIS_SIZE>& GetRawMap() const;
private:
	sf::Vector2f TranslateMapPointToWorldPosition(size_t aIndex) const;
	void ClearMap();

	void PlaceSpawnPointAndGoldIsland();
	void PlaceIslands();
	void CreateWaves();

	size_t myTreasureIsland;
	size_t myGoldIslandIndex;
	size_t myPlayerSpawnIndex;
	std::array<int, MAP_AXIS_SIZE * MAP_AXIS_SIZE> myMap;
	std::vector<CIsland> myIslands;

};