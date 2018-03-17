#pragma once
#include <SFML\Network\Packet.hpp>
#include <SFML\System\String.hpp>
#include <SFML\Graphics\Transformable.hpp>
#include "Defines.h"

enum class EMessageType
{
	Connect,
	Welcome,
	Transform,
	MapChange,
};

struct SNetMessage
{
	virtual sf::Packet GetAsPacket() = 0;
	virtual void OpenPacket(sf::Packet& aPacket) = 0;

	EMessageType myType;
	size_t mySenderID;
};

struct SConnectMessage : public SNetMessage
{
	SConnectMessage()
	{
		myType = EMessageType::Connect;
	}

	void OpenPacket(sf::Packet& aPacket) override
	{
		aPacket >> myText;
	}

	sf::Packet GetAsPacket() override
	{
		sf::Packet packet;
		packet << (short)myType;
		packet << myText;

		return std::move(packet);
	}

	sf::String myText;
};

struct SWelcomeMessage : public SNetMessage
{
	SWelcomeMessage()
	{
		myType = EMessageType::Welcome;
	}

	void OpenPacket(sf::Packet& aPacket) override
	{
		aPacket >> myID;
		aPacket >> myCurrentPlayerCount;

		for (size_t i = 0; i < MAP_AXIS_SIZE * MAP_AXIS_SIZE; ++i)
		{
			aPacket >> myCurrentMap[i];
		}
	}

	sf::Packet GetAsPacket() override
	{
		sf::Packet packet;
		packet << (short)myType;
		packet << myID;
		packet << myCurrentPlayerCount;

		for (size_t i = 0; i < MAP_AXIS_SIZE * MAP_AXIS_SIZE; ++i)
		{
			packet << myCurrentMap[i];
		}

		return std::move(packet);
	}
	
	unsigned int myCurrentPlayerCount;
	int myCurrentMap[MAP_AXIS_SIZE * MAP_AXIS_SIZE];
	unsigned int myID;
};

struct STransformMessage : public SNetMessage
{
	STransformMessage()
	{
		myType = EMessageType::Transform;
	}

	void OpenPacket(sf::Packet& aPacket) override
	{
		aPacket >> mySenderID;
		aPacket >> myRotation;
		aPacket >> myX;
		aPacket >> myY;
	}

	sf::Packet GetAsPacket() override
	{
		sf::Packet packet;
		packet << (short)myType;
		packet << mySenderID;
		packet << myRotation;
		packet << myX;
		packet << myY;
		return std::move(packet);
	}
	
	float myRotation;
	float myX;
	float myY;
};