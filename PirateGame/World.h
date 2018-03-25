#pragma once
#include "Defines.h"
#include "Island.h"
#include <array>
#include <vector>
#include "PlayerActions.h"
#include "MapStruct.h"

class CUIMap;
class CShip;


class CWorld
{
public:
	void CreateFromGeneratedMap(const SMap& aMap, CUIMap& aUIMap);
	void Generate(CUIMap& aUIMap);
	void Render();

	void PlaceTreasure(CUIMap& aUIMap);

	EPlayerAction CheckPlayerWorldInteraction(CShip& aPlayerShip);

	sf::Vector2f GetSpawnPosition() const;
	sf::Vector2f GetRandomSeaPosition() const;

	const SMap& GetRawMap() const;
private:
	sf::Vector2f TranslateMapPointToWorldPosition(size_t aIndex) const;
	void ClearMap();

	void PlaceSpawnPointAndGoldIsland();
	void PlaceIslands();
	void CreateWaves();

	bool IsInsideWorld(const sf::Vector2f& aPosition) const;


	size_t myTreasureIsland;
	size_t myGoldIslandIndex;
	size_t myPlayerSpawnIndex;

	SMap myMap;

	std::vector<CIsland> myIslands;

};