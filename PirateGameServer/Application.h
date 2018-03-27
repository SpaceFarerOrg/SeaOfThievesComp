#pragma once
#include <SFML\Network\UdpSocket.hpp>
#include <SFML\Network\IpAddress.hpp>

class CApplication
{
public:
	void Init();
	void Update();
	void ShutDown();
	bool GetShouldRun();
private:
	sf::IpAddress myAdress;
	sf::UdpSocket mySocket;
	short myPort;
	bool myShouldRun;
};