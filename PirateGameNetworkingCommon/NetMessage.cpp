#include "NetMessage.h"

sf::Packet SNetMessage::GetAsPacket()
{
	sf::Packet packet;
	packet << (size_t)myType;
	packet << mySenderID;

	return std::move(packet);
}
