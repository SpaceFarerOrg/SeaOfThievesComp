#include "TextureBank.h"
#include <SFML/System/String.hpp>

CTextureBank & CTextureBank::GetInstance()
{
	static CTextureBank instance;
	return instance;
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

CTextureBank::CTextureBank()
{
}
