#pragma once
#include "NetMessage.h"
#include <SFML\Network\UdpSocket.hpp>

class CMessageManager
{
public:
	static CMessageManager& GetInstance();
	void BindSocket(sf::UdpSocket& aSocket);
	ENetMessageType GetMessageType(sf::Packet& aPacket);
	void SendMessageTo(SNetMessage& aMessageToSend, const sf::IpAddress& aIP, short aPort);
private:
	CMessageManager();
	sf::UdpSocket* mySocket;

};