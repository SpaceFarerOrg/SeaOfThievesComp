#include "Network.h"
#include <iostream>

CNetworking & CNetworking::GetInstance()
{
	static CNetworking instance;
	return instance;
}

void CNetworking::SetName(const sf::String & aName)
{
	myName = aName;
}

void CNetworking::StartServer()
{
	if (myIsNetworkEnabled)
		return;

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
	if (myIsNetworkEnabled)
		return;

	if (mySocket.bind(sf::Socket::AnyPort) == sf::Socket::Error)
	{
		std::cout << "Failed to bind client port" << std::endl;
		myIsNetworkEnabled = false;
		myIsClient = false;
		mySocket.unbind();
		return;
	}

	myLastPingTime = time(nullptr);
	mySocket.setBlocking(true);
	myLastRecieveTime = time(nullptr);
	myIsNetworkEnabled = true;
	myIsClient = true;


	//std::cout << "Tried connect to server" << std::endl;
	mySelector.add(mySocket);

	SConnectMessage msg;
	msg.myText = myName;
	sf::Packet p = msg.GetAsPacket();


	myServerAdress = aIp;

	if (!mySocket.send(p, myServerAdress, SERVER_PORT) == sf::Socket::Done)
	{
		std::cout << "Failed to send connect message" << std::endl;
	}
}

void CNetworking::Disconnect()
{
	if (!myIsNetworkEnabled)
		return;

	if (myIsClient)
	{
		SDisconnectMessage msg;
		msg.myDisconnectedClient = myClientID;
		sf::Packet p = msg.GetAsPacket();

		mySocket.send(p, myServerAdress, SERVER_PORT);
		mySocket.unbind();
		myIsNetworkEnabled = false;
		myIsClient = false;
	}
	else if (!myIsClient)
	{
		SDisconnectMessage msg;
		msg.myDisconnectedClient = 0;
		sf::Packet p = msg.GetAsPacket();

		for (size_t i = 0; i < myClients.size(); ++i)
		{
			mySocket.send(p, myClients[i].myIP, myClients[i].myPort);
		}

		mySocket.unbind();
		myIsNetworkEnabled = false;
		myIsClient = false;
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

	if (mySocket.receive(p, ip, port) == sf::Socket::Done)
	{
		myLastRecieveTime = time(nullptr);
		mySocket.receive(p, ip, port);

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

void CNetworking::SendWhirlwindSpawn(const sf::Vector2f & aPosition)
{
	SWhirlwindMessage msg;
	msg.myX = aPosition.x;
	msg.myY = aPosition.y;

	sf::Packet p = msg.GetAsPacket();

	for (size_t i = 1; i < myClients.size(); ++i)
	{
		mySocket.send(p, myClients[i].myIP, myClients[i].myPort);
	}
}

void CNetworking::SetMap(const std::array<int, MAP_AXIS_SIZE*MAP_AXIS_SIZE>& aMap)
{
	myMap = aMap;
}

const std::array<int, MAP_AXIS_SIZE*MAP_AXIS_SIZE>& CNetworking::GetMap() const
{
	return myMap;
}

time_t CNetworking::GetLastRecieveTime()
{
	return myLastRecieveTime;
}

const std::vector<SClient>& CNetworking::GetPlayerList()
{
	return myClients;
}

void CNetworking::SetGame(CGame * aGame)
{
	myGame = aGame;
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
	myName = "Kocklice";
	myIsNetworkEnabled = false;
	myIsClient = false;
	myClientID = 0;
}

void CNetworking::UpdateAsServer(sf::Packet& aPacket, sf::IpAddress& aIP, unsigned short aPort, EMessageType aType)
{
	for (size_t i = 1; i < myClients.size(); ++i)
	{
		if (time(nullptr) - myClients[i].myLastPing > 2)
		{
			SPingMessage ping;
			sf::Packet p = ping.GetAsPacket();
			mySocket.send(p, myClients[i].myIP, myClients[i].myPort);
		}
	}

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
	if (aType == EMessageType::Disconnect)
	{
		SDisconnectMessage msg;
		msg.OpenPacket(aPacket);

		myClients.erase(myClients.begin() + msg.myDisconnectedClient);

		SDisconnectMessage sendM;
		sendM.myDisconnectedClient = msg.myDisconnectedClient;
		sf::Packet p = sendM.GetAsPacket();

		for (size_t i = 0; i < myClients.size(); ++i)
		{
			mySocket.send(p, myClients[i].myIP, myClients[i].myPort);
		}
	}
	if (aType == EMessageType::Ping)
	{
		SPingMessage msg;
		msg.OpenPacket(aPacket);

		myClients[msg.mySenderID].myLastPing = msg.myTimeStamp;
	}

}

void CNetworking::UpdateAsClient(sf::Packet& aPacket, sf::IpAddress& aIP, unsigned short aPort, EMessageType aType)
{
	if (time(nullptr) - myLastPingTime > 5)
	{
		Disconnect();
	}

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

		SConnectMessage msg;
		msg.OpenPacket(aPacket);
		myClients.back().myName = msg.myText;

	}
	if (aType == EMessageType::Transform)
	{
		STransformMessage msg;
		msg.OpenPacket(aPacket);
		SClient& client = myClients[msg.mySenderID];
		client.myTransform.setRotation(msg.myRotation);
		client.myTransform.setPosition(msg.myX, msg.myY);
	}
	if (aType == EMessageType::Whirlwind)
	{
		SWhirlwindMessage msg;
		msg.OpenPacket(aPacket);
		myGame->PlaceWhirlwind({ msg.myX, msg.myY });
	}
	if (aType == EMessageType::Disconnect)
	{
		SDisconnectMessage msg;
		msg.OpenPacket(aPacket);

		if (msg.mySenderID == 0)
		{
			Disconnect();
			myClients.clear();
		}
		else
		{
			myClients.erase(myClients.begin() + msg.mySenderID);
		}
	}
	if (aType == EMessageType::Ping)
	{
		SPingMessage msg;
		msg.OpenPacket(aPacket);
		myLastPingTime = msg.myTimeStamp;

		SPingMessage returnMsg;
		returnMsg.mySenderID = myClientID;
		sf::Packet p = returnMsg.GetAsPacket();
		mySocket.send(p, myServerAdress, SERVER_PORT);
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
		msg.myText = newClient.myName;

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
