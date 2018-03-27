#include "World.h"
#include "Math.h"
#include "UIMap.h"
#include "TextureBank.h"
#include "Ship.h"

void CWorld::CreateFromGeneratedMap(const SMap& aMap, CUIMap& aUIMap)
{
	ClearMap();
	myMap = aMap;

	for (size_t i = 0; i < myMap.GetSize(); ++i)
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

void CWorld::LootTreasure()
{
	myIslands[myTreasureIsland].SetHasTreasure(false);
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

	myTreasureIslandInMap = myIslands[islandToGetTreasure].GetIndexInMap();
	myTreasureIsland = islandToGetTreasure;

	myIslands[islandToGetTreasure].SetHasTreasure();

	aUIMap.SetTreasureIsland(myIslands[islandToGetTreasure].GetIndexInMap());
}

EPlayerAction CWorld::CheckPlayerWorldInteraction(CShip & aPlayerShip)
{
	const std::array<sf::Vector2f, 4>& cp = aPlayerShip.GetCollisionPoints();

	if (!IsInsideWorld(aPlayerShip.GetPosition()))
	{
		return EPlayerAction::OutsideWorldBounds;
	}

	for (CIsland& island : myIslands)
	{
		for (const sf::Vector2f& p : cp)
		{
			if (island.IsColliding(p))
			{
				return EPlayerAction::Crash;
			}

			if (island.IsInLootingRange(p))
			{
				if (island.GetIndexInMap() == myGoldIslandIndex && aPlayerShip.GetHasTreasure())
				{
					return EPlayerAction::Sell;
				}

				if (island.GetIndexInMap() == myTreasureIslandInMap && !aPlayerShip.GetHasTreasure())
				{
					return EPlayerAction::Loot;
				}
			}
		}
	}
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
		size_t indexInMap = Math::GetRandomInRange(0, (short)myMap.GetSize() - 1);

		if (myMap[indexInMap] == SEA)
		{
			sf::Vector2f pos = TranslateMapPointToWorldPosition(indexInMap);

			return std::move(pos);
		}
	}
}

const SMap& CWorld::GetRawMap() const
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
	for (int& chunk : myMap.myMap)
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
	myIslands.back().SetIndexInMap(myGoldIslandIndex);
}

void CWorld::PlaceIslands()
{
	size_t numberOfIsles = Math::GetRandomInRange(MIN_ISLAND_COUNT, MAX_ISLAND_COUNT);

	for (size_t i = 0; i < numberOfIsles; ++i)
	{
		bool placedIsland = false;

		while (!placedIsland)
		{
			size_t indexInMap = Math::GetRandomInRange(0, (short)myMap.myMap.size() - 1);

			if (indexInMap != myPlayerSpawnIndex && indexInMap != myGoldIslandIndex && myMap[indexInMap] < ISLAND_1)
			{
				short islandType = Math::GetRandomInRange(0, 2);

				float rotation = static_cast<float>(Math::GetRandomInRange(0, 360));

				myMap[indexInMap] = (int)(ISLAND_1 + islandType);
				myMap(indexInMap) = rotation;
				myIslands.push_back(CIsland());
				myIslands.back().SetIslandData((EIslandType)islandType, TranslateMapPointToWorldPosition(indexInMap));
				myIslands.back().Init();
				myIslands.back().SetIndexInMap(indexInMap);
				myIslands.back().SetRotation(rotation);

				placedIsland = true;
			}

		}

	}
}

void CWorld::CreateWaves()
{
}

bool CWorld::IsInsideWorld(const sf::Vector2f & aPosition) const
{
	float mapMin = 0.f;
	float mapMax = MAP_CHUNK_SIZE * MAP_AXIS_SIZE;
	float forgiveness = 200.f;

	float mapBorderMax = mapMax + forgiveness;
	float mapBorderMin = mapMin - forgiveness;

	if (aPosition.x > mapBorderMax 
		|| aPosition.y > mapBorderMax 
		|| aPosition.x < mapBorderMin 
		|| aPosition.y < mapBorderMin)
	{
		return false;
	}

	return true;
}
