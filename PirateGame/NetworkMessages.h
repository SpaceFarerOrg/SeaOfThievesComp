#pragma once
#include <SFML\Network\Packet.hpp>
#include <SFML\System\String.hpp>
#include <SFML\Graphics\Transformable.hpp>
#include "Defines.h"
#include <ctime>

enum class EMessageType
{
	Connect,
	Disconnect,
	Welcome,
	Transform,
	MapChange,
	Whirlwind,
	CloseToWin,
	Ping,
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
		aPacket >> myNameSize;

		for (size_t i = 0; i < myNameSize; ++i)
		{
			sf::Uint32 c;
			aPacket >> c;

			myName += c;
		}
	}

	sf::Packet GetAsPacket() override
	{
		sf::Packet packet;
		packet << (short)myType;
		packet << mySenderID;
		packet << myRotation;
		packet << myX;
		packet << myY;
		packet << myName.getSize();

		size_t size = myName.getSize();

		for (size_t i = 0; i < myName.getSize(); ++i)
		{
			sf::Uint32 c = myName[i];
			packet << c;
		}
		
		return std::move(packet);
	}

	sf::String myName;
	size_t myNameSize;

	float myRotation;
	float myX;
	float myY;
};

struct SWhirlwindMessage : public SNetMessage
{
	SWhirlwindMessage()
	{
		myType = EMessageType::Whirlwind;
	}

	void OpenPacket(sf::Packet& aPacket) override
	{
		aPacket >> myX;
		aPacket >> myY;
	}

	sf::Packet GetAsPacket() override
	{
		sf::Packet packet;
		packet << (short)myType;
		packet << myX;
		packet << myY;
		return std::move(packet);
	}

	float myX;
	float myY;
};

struct SDisconnectMessage : public SNetMessage
{
	SDisconnectMessage()
	{
		myType = EMessageType::Disconnect;
	}

	void OpenPacket(sf::Packet& aPacket) override
	{
		aPacket >> myDisconnectedClient;
	}

	sf::Packet GetAsPacket() override
	{
		sf::Packet packet;
		packet << (short)myType;
		packet << myDisconnectedClient;
		return std::move(packet);
	}

	size_t myDisconnectedClient;
};

struct SPingMessage : public SNetMessage
{
	SPingMessage()
	{
		myType = EMessageType::Ping;
		myTimeStamp = time(nullptr);
	}

	void OpenPacket(sf::Packet& aPacket) override
	{
		aPacket >> mySenderID;
		aPacket >> myTimeStamp;
	}

	sf::Packet GetAsPacket() override
	{
		sf::Packet packet;
		packet << (short)myType;
		packet << mySenderID;
		packet << myTimeStamp;
		return std::move(packet);
	}

	time_t myTimeStamp;
};

struct SCloseToWinMessage : public SNetMessage
{
	SCloseToWinMessage()
	{
		myType = EMessageType::CloseToWin;
	}

	void OpenPacket(sf::Packet& aPacket) override
	{
		aPacket >> myID;
		aPacket >> myIsCloseTo;
	}

	sf::Packet GetAsPacket() override
	{
		sf::Packet packet;
		packet << (short)myType;
		packet << myID;
		packet << myIsCloseTo;

		return std::move(packet);
	}

	bool myIsCloseTo;
	size_t myID;
};