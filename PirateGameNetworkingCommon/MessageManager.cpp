#include "MessageManager.h"

CMessageManager::CMessageManager()
	: mySocket(nullptr)
{

}

CMessageManager & CMessageManager::GetInstance()
{
	static CMessageManager instance;
	return instance;
}

void CMessageManager::BindSocket(sf::UdpSocket & aSocket)
{
	mySocket = &aSocket;
}

ENetMessageType CMessageManager::GetMessageType(sf::Packet & aPacket)
{
	size_t messageTypeAsSizeT;
	aPacket >> messageTypeAsSizeT;

	return (ENetMessageType)messageTypeAsSizeT;
}

void CMessageManager::SendMessageTo(SNetMessage & aMessageToSend, const sf::IpAddress & aIP, short aPort)
{
	sf::Packet packetToSend = aMessageToSend.GetAsPacket();

	mySocket->send(packetToSend, aIP, aPort);
}
