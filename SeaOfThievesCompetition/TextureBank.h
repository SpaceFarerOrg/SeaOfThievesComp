#pragma once
#include <SFML\Graphics\Texture.hpp>
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
	Count,
};

class CTextureBank
{
public:
	static CTextureBank& GetInstance();
	void LoadTexture(ETexture aTexture, const char* aPath);
	const sf::Texture& GetTexture(ETexture aTexture) const;
private:
	CTextureBank();
	std::array<sf::Texture, (size_t)ETexture::Count> myBank;

};