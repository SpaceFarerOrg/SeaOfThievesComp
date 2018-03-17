#include "Network.h"
#include <iostream>

CNetworking & CNetworking::GetInstance()
{
	static CNetworking instance;
	return instance;
}

void CNetworking::StartServer()
{
	myIsNetworkEnabled = true;
	mySocket.setBlocking(true);
	myIsClient = false;
	if (!mySocket.bind(SERVER_PORT) == sf::Socket::Done)
	{
		std::cout << "Failed to bind server port" << std::endl;
	}

	myClients.push_back(SClient());
}

void CNetworking::ConnectToServer(sf::IpAddress aIp)
{
	myIsNetworkEnabled = true;
	mySocket.setBlocking(true);
	myIsClient = true;

	if (!mySocket.bind(sf::Socket::AnyPort) == sf::Socket::Done)
	{
		std::cout << "Failed to bind server port" << std::endl;
	}

	SConnectMessage msg;
	sf::Packet p = msg.GetAsPacket();

	myServerAdress = aIp;

	if (!mySocket.send(p, myServerAdress, SERVER_PORT) == sf::Socket::Done)
	{
		std::cout << "Failed to send connect message" << std::endl;
	}
}

void CNetworking::Update()
{
	if (!myIsNetworkEnabled)
	{
		return;
	}

	sf::Packet p;
	sf::IpAddress ip;
	unsigned short port;

	if (mySocket.receive(p, ip, port) == sf::Socket::Status::Done)
	{
		short shortType;
		p >> shortType;
		EMessageType type = (EMessageType)shortType;

		if (!myIsClient)
		{
			UpdateAsServer(p, ip, port, type);
		}
		else if (myIsClient)
		{
			UpdateAsClient(p, ip, port, type);
		}
	}
}

void CNetworking::SendMyTranslation(sf::Transformable & aTransform)
{
	if (!myIsClient)
	{
		STransformMessage msg;
		msg.mySenderID = 0;
		msg.myRotation = aTransform.getRotation();
		msg.myX = aTransform.getPosition().x;
		msg.myY = aTransform.getPosition().y;

		sf::Packet p = msg.GetAsPacket();

		for (size_t i = 1; i < myClients.size(); ++i)
		{
			mySocket.send(p, myClients[i].myIP, myClients[i].myPort);
		}
		return;
	}

	STransformMessage msg;
	msg.mySenderID = myClientID;
	msg.myRotation = aTransform.getRotation();
	msg.myX = aTransform.getPosition().x;
	msg.myY = aTransform.getPosition().y;

	sf::Packet p = msg.GetAsPacket();

	mySocket.send(p, myServerAdress, SERVER_PORT);
}

void CNetworking::SetMap(const std::array<int, MAP_AXIS_SIZE*MAP_AXIS_SIZE>& aMap)
{
	myMap = aMap;
}

const std::array<int, MAP_AXIS_SIZE*MAP_AXIS_SIZE>& CNetworking::GetMap() const
{
	return myMap;
}

const std::vector<SClient>& CNetworking::GetPlayerList()
{
	return myClients;
}

bool CNetworking::GetIsNetworkEnabled() const
{
	return myIsNetworkEnabled;
}

bool CNetworking::GetIsClient() const
{
	return myIsClient;
}

CNetworking::CNetworking()
{
	myIsNetworkEnabled = false;
	myIsClient = false;
	myClientID = 0;
}

void CNetworking::UpdateAsServer(sf::Packet& aPacket, sf::IpAddress& aIP, unsigned short aPort, EMessageType aType)
{
	if (aType == EMessageType::Connect)
	{
		SConnectMessage msg;
		msg.OpenPacket(aPacket);
		ConnectClient(msg, aIP, aPort);
	}
	if (aType == EMessageType::Transform)
	{
		STransformMessage msg;
		msg.OpenPacket(aPacket);
		SClient& client = myClients[msg.mySenderID];
		client.myTransform.setRotation(msg.myRotation);
		client.myTransform.setPosition(msg.myX, msg.myY);

		STransformMessage toClients;
		toClients.mySenderID = msg.mySenderID;
		toClients.myRotation = msg.myRotation;
		toClients.myX = msg.myX;
		toClients.myY = msg.myY;
		sf::Packet p = toClients.GetAsPacket();

		for (SClient& client : myClients)
		{
			mySocket.send(p, client.myIP, client.myPort);
		}
	}
}

void CNetworking::UpdateAsClient(sf::Packet& aPacket, sf::IpAddress& aIP, unsigned short aPort, EMessageType aType)
{
	if (aType == EMessageType::Welcome)
	{
		SWelcomeMessage msg;
		msg.OpenPacket(aPacket);
		myClientID = msg.myID;
		
		for (size_t i = 0; i < msg.myCurrentPlayerCount; ++i)
		{
			myClients.push_back(SClient());
		}

		for (size_t i = 0; i < MAP_AXIS_SIZE * MAP_AXIS_SIZE; ++i)
		{
			myMap[i] = msg.myCurrentMap[i];
		}

		myClients.push_back(SClient());
	}
	if (aType == EMessageType::Connect)
	{
		myClients.push_back(SClient());
	}
	if (aType == EMessageType::Transform)
	{
		STransformMessage msg;
		msg.OpenPacket(aPacket);
		SClient& client = myClients[msg.mySenderID];
		client.myTransform.setRotation(msg.myRotation);
		client.myTransform.setPosition(msg.myX, msg.myY);
	}
}

void CNetworking::ConnectClient(SConnectMessage & aMessage, sf::IpAddress& aIp, unsigned short aPort)
{
	SClient newClient;
	newClient.myName = aMessage.myText;
	newClient.myIP = aIp;
	newClient.myPort = aPort;

	for (size_t i = 1; i < myClients.size(); ++i)
	{
		SConnectMessage msg;

		sf::Packet p = msg.GetAsPacket();
		mySocket.send(p, myClients[i].myIP, myClients[i].myPort);
	}

	myClients.push_back(newClient);

	SWelcomeMessage msg;
	msg.myCurrentPlayerCount = myClients.size() - 1;
	msg.myID = myClients.size() - 1;

	for (size_t i = 0; i < myMap.size(); ++i)
	{
		msg.myCurrentMap[i] = myMap[i];
	}

	sf::Packet p = msg.GetAsPacket();

	std::cout << "Client connected" << std::endl;

	mySocket.send(p, newClient.myIP, newClient.myPort);
}
