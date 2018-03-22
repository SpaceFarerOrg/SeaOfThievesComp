#include "World.h"
#include "Math.h"
#include "UIMap.h"
#include "TextureBank.h"

void CWorld::CreateFormArray(const std::array<int, MAP_AXIS_SIZE*MAP_AXIS_SIZE>& aMap, CUIMap& aUIMap)
{
	ClearMap();
	myMap = aMap;

	for (size_t i = 0; i < myMap.size(); ++i)
	{
		ETexture islandTexture;
		if (myMap[i] == ISLAND_1)
		{
			islandTexture = ETexture::Island;
			myIslands.push_back(CIsland());
			myIslands.back().SetIslandData(EIslandType::IslandOne, TranslateMapPointToWorldPosition(i));
			myIslands.back().Init();
			myIslands.back().SetIndexInMap(i);
		}
		if (myMap[i] == ISLAND_2)
		{
			islandTexture = ETexture::IslandTwo;
			myIslands.push_back(CIsland());
			myIslands.back().SetIslandData(EIslandType::IslandTwo, TranslateMapPointToWorldPosition(i));
			myIslands.back().SetIndexInMap(i);
		}
		if (myMap[i] == ISLAND_3)
		{
			islandTexture = ETexture::IslandThree;
			myIslands.push_back(CIsland());
			myIslands.back().SetIslandData(EIslandType::IslandThree, TranslateMapPointToWorldPosition(i));
			myIslands.back().SetIndexInMap(i);
		}
		if (myMap[i] == GOLD_ISLAND)
		{
			myIslands.push_back(CIsland());
			myIslands.back().SetIslandData(EIslandType::GoldIsland, TranslateMapPointToWorldPosition(i));
			myGoldIslandIndex = i;
			myIslands.back().SetIndexInMap(i);
		}
	}

	CreateWaves();

	aUIMap.SetMap(myMap);
}

void CWorld::Generate(CUIMap& aMap)
{
	ClearMap();
	PlaceSpawnPointAndGoldIsland();
	PlaceIslands();
	CreateWaves();

	aMap.SetMap(myMap);
}

void CWorld::Render()
{
	for (CIsland& island : myIslands)
	{
		island.Render();
	}
}

void CWorld::PlaceTreasure(CUIMap& aUIMap)
{
	bool placedTreasure = false;

	short islandToGetTreasure = 0;


	for (CIsland& island : myIslands)
	{
		island.SetHasTreasure(false);
	}

	while (!placedTreasure)
	{
		islandToGetTreasure = Math::GetRandomInRange(0, (short)myIslands.size() - 1);

		if (islandToGetTreasure != myTreasureIsland && islandToGetTreasure != myGoldIslandIndex)
		{
			placedTreasure = true;
		}
	}

	myTreasureIsland = islandToGetTreasure;

	myIslands[islandToGetTreasure].SetHasTreasure();

	aUIMap.SetTreasureIsland(myIslands[islandToGetTreasure].GetIndexInMap());
}

std::vector<CIsland>& CWorld::GetIslands()
{
	return myIslands;
}

sf::Vector2f CWorld::GetSpawnPosition() const
{
	sf::Vector2f spawnPos = TranslateMapPointToWorldPosition(myPlayerSpawnIndex);

	return std::move(spawnPos);
}

sf::Vector2f CWorld::GetRandomSeaPosition() const
{
	while (true)
	{
		size_t indexInMap = Math::GetRandomInRange(0, (short)myMap.size() - 1);

		if (myMap[indexInMap] == SEA)
		{
			sf::Vector2f pos = TranslateMapPointToWorldPosition(indexInMap);

			return std::move(pos);
		}
	}
}

const std::array<int, MAP_AXIS_SIZE*MAP_AXIS_SIZE>& CWorld::GetRawMap() const
{
	return myMap;
}

sf::Vector2f CWorld::TranslateMapPointToWorldPosition(size_t aIndex) const
{
	size_t x = aIndex % MAP_AXIS_SIZE;
	size_t y = aIndex / MAP_AXIS_SIZE;

	float worldX = (float)x * MAP_CHUNK_SIZE;
	float worldY = (float)y * MAP_CHUNK_SIZE;

	sf::Vector2f returnPos = { worldX, worldY };

	return std::move(returnPos);
}

void CWorld::ClearMap()
{
	myIslands.clear();
	for (int& chunk : myMap)
	{
		chunk = 0;
	}
}

void CWorld::PlaceSpawnPointAndGoldIsland()
{
	int y = 7;
	int x = 7;

	myGoldIslandIndex = (MAP_AXIS_SIZE * y) - x;
	myPlayerSpawnIndex = (MAP_AXIS_SIZE * (y + 1)) - (x -1);

	myMap[myGoldIslandIndex] = GOLD_ISLAND;
	myMap[myPlayerSpawnIndex] = SPAWN_POSITION;

	myIslands.push_back(CIsland());
	myIslands.back().SetIslandData(EIslandType::GoldIsland, TranslateMapPointToWorldPosition(myGoldIslandIndex));
	myIslands.back().Init();
}

void CWorld::PlaceIslands()
{
	size_t numberOfIsles = Math::GetRandomInRange(MIN_ISLAND_COUNT, MAX_ISLAND_COUNT);

	for (size_t i = 0; i < numberOfIsles; ++i)
	{
		bool placedIsland = false;

		while (!placedIsland)
		{
			size_t indexInMap = Math::GetRandomInRange(0, (short)myMap.size() - 1);

			if (indexInMap != myPlayerSpawnIndex && indexInMap != myGoldIslandIndex && myMap[indexInMap] < ISLAND_1)
			{
				short islandType = Math::GetRandomInRange(0, 2);

				myMap[indexInMap] = (int)(ISLAND_1 + islandType);
				myIslands.push_back(CIsland());
				myIslands.back().SetIslandData((EIslandType)islandType, TranslateMapPointToWorldPosition(indexInMap));
				myIslands.back().Init();
				myIslands.back().SetIndexInMap(indexInMap);

				placedIsland = true;
			}

		}

	}
}

void CWorld::CreateWaves()
{
}
