#pragma once
#include <SFML\Network\UdpSocket.hpp>
#include <SFML\Network\SocketSelector.hpp>
#include "NetworkMessages.h"
#include <sfml\System\String.hpp>
#include <vector>
#include <array>
#include "Defines.h"
#include <SFML\Graphics\Transformable.hpp>
#include "Game.h"
#define SERVER_PORT (unsigned short)54000

struct SClient
{
	sf::IpAddress myIP;
	unsigned short myPort;
	sf::String myName;
	time_t myLastPing;
	sf::Transformable myTransform;
	bool myIsWelcomed = false;
	bool myConnected = true;
};

class CNetworking
{
public:
	static CNetworking& GetInstance();

	void SetName(const sf::String& aName);

	void StartServer();
	void ConnectToServer(sf::IpAddress aIp);
	void Disconnect();

	void Update();

	void SendMyTranslation(sf::Transformable& aTransform);
	void SendWhirlwindSpawn(const sf::Vector2f& aPosition);
	
	void SetMap(const std::array<int, MAP_AXIS_SIZE * MAP_AXIS_SIZE>& aMap);
	const std::array<int, MAP_AXIS_SIZE * MAP_AXIS_SIZE>& GetMap() const;

	time_t GetLastRecieveTime();
	void DoPingUpdate();

	void SendWinning(bool aOnlyCloseTo);

	const std::vector<SClient>& GetPlayerList();

	size_t GetSelfInClientList() const;

	void SetGame(CGame* aGame);

	bool GetIsWelcomed() const;

	bool GetIsNetworkEnabled() const;
	bool GetIsClient() const;
private:

	CNetworking();

	time_t myLastPingTime;
	time_t myLastRecieveTime;

	size_t mySelfInClientList;

	CGame* myGame;

	void UpdateAsServer(sf::Packet& aPacket, sf::IpAddress& aIP, unsigned short aPort, EMessageType aType);
	void UpdateAsClient(sf::Packet& aPacket, sf::IpAddress& aIP, unsigned short aPort, EMessageType aType);

	void ConnectClient(SConnectMessage& aMessage, sf::IpAddress& aIp, unsigned short aPort);

	sf::String myName;

	sf::IpAddress myServerAdress;
	sf::UdpSocket mySocket;
	sf::SocketSelector mySelector;
	bool myIsClient;
	bool myIsWelcomed;
	bool myIsNetworkEnabled;
	unsigned int myClientID;

	std::array<int, MAP_AXIS_SIZE * MAP_AXIS_SIZE> myMap;

	std::vector<SClient> myClients;
};
