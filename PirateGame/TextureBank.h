#pragma once
#include <SFML\Graphics\Texture.hpp>
#include <SFML\Graphics\Font.hpp>
#include <array>

#define  GET_TEXTURE(aTextureEnum) CTextureBank::GetInstance().GetTexture(aTextureEnum)

enum class ETexture
{
	Ship,
	Island,
	IslandTwo,
	IslandThree,
	GoldIsland,
	Waves,
	ShipWavesBig,
	Cross,
	Map,

	MapIsland,
	MapIslandTwo,
	MapIslandThree,
	MapGoldIsland,
	Whirlwind,

	Slider,
	Handle,

	CompassBase,
	CompassPointer,

	Count,
};

class CTextureBank
{
public:
	static CTextureBank& GetInstance();
	void LoadAllGraphicItems();
	const sf::Texture& GetTexture(ETexture aTexture) const;

	const sf::Font& GetFont() const;
private:
	void LoadTexture(ETexture aTexture, const char* aPath);


	CTextureBank();
	std::array<sf::Texture, (size_t)ETexture::Count> myBank;
	sf::Font myFont;

};