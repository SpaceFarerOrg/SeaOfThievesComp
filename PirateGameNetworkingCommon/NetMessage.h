#pragma once
#include "MessageEnum.h"
#include <SFML\Network\Packet.hpp>

struct SNetMessage
{
public:
	virtual sf::Packet GetAsPacket();
protected:
	ENetMessageType myType;
	short mySenderID;
};