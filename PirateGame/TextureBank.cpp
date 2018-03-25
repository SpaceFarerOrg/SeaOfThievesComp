#include "TextureBank.h"
#include <SFML/System/String.hpp>

CTextureBank & CTextureBank::GetInstance()
{
	static CTextureBank instance;
	return instance;
}

void CTextureBank::LoadAllGraphicItems()
{
	//Player bound textures
	LoadTexture(ETexture::Ship, "playerShip");
	LoadTexture(ETexture::ShipWavesBig, "shipWavesBig");

	//Environment bound textures
	LoadTexture(ETexture::GoldIsland, "goldIsland");
	LoadTexture(ETexture::Island, "island1");
	LoadTexture(ETexture::IslandTwo, "island2");
	LoadTexture(ETexture::IslandThree, "island3");
	LoadTexture(ETexture::Waves, "waterWaves");
	LoadTexture(ETexture::Whirlwind, "whirlwind");

	//Map bound textures
	LoadTexture(ETexture::Map, "map");
	LoadTexture(ETexture::MapIsland, "mapIsland");
	LoadTexture(ETexture::MapIslandTwo, "mapIsland2");
	LoadTexture(ETexture::MapIslandThree, "mapIsland3");
	LoadTexture(ETexture::MapGoldIsland, "mapGoldIsland");
	LoadTexture(ETexture::Cross, "cross");

	//UI
	LoadTexture(ETexture::Slider, "slider");
	LoadTexture(ETexture::Handle, "handle");

	//Fonts
	myFont.loadFromFile("font/font.ttf");
}

void CTextureBank::LoadTexture(ETexture aTexture, const char * aPath)
{
	sf::String fullPath = "sprites/";
	fullPath += aPath;
	fullPath += ".png";

	myBank[(size_t)aTexture].loadFromFile(fullPath);
	myBank[(size_t)aTexture].setSmooth(true);
}

const sf::Texture & CTextureBank::GetTexture(ETexture aTexture) const
{
	return myBank[(size_t)aTexture];
}

const sf::Font & CTextureBank::GetFont() const
{
	return myFont;
}

CTextureBank::CTextureBank()
{
}
