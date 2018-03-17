#pragma once
#include <SFML\Network\UdpSocket.hpp>
#include "NetworkMessages.h"
#include <sfml\System\String.hpp>
#include <vector>
#include <array>
#include "Defines.h"
#include <SFML\Graphics\Transformable.hpp>
#define SERVER_PORT (unsigned short)54000

struct SClient
{
	sf::IpAddress myIP;
	unsigned short myPort;
	sf::String myName;
	sf::Transformable myTransform;
};

class CNetworking
{
public:
	static CNetworking& GetInstance();
	void StartServer();
	void ConnectToServer(sf::IpAddress aIp);
	void Update();

	void SendMyTranslation(sf::Transformable& aTransform);
	void SetMap(const std::array<int, MAP_AXIS_SIZE * MAP_AXIS_SIZE>& aMap);
	const std::array<int, MAP_AXIS_SIZE * MAP_AXIS_SIZE>& GetMap() const;

	const std::vector<SClient>& GetPlayerList();

	bool GetIsNetworkEnabled() const;
	bool GetIsClient() const;
private:
	CNetworking();

	void UpdateAsServer(sf::Packet& aPacket, sf::IpAddress& aIP, unsigned short aPort, EMessageType aType);
	void UpdateAsClient(sf::Packet& aPacket, sf::IpAddress& aIP, unsigned short aPort, EMessageType aType);

	void ConnectClient(SConnectMessage& aMessage, sf::IpAddress& aIp, unsigned short aPort);

	sf::IpAddress myServerAdress;
	sf::UdpSocket mySocket;
	bool myIsClient;
	bool myIsNetworkEnabled;
	unsigned int myClientID;

	std::array<int, MAP_AXIS_SIZE * MAP_AXIS_SIZE> myMap;

	std::vector<SClient> myClients;
};
