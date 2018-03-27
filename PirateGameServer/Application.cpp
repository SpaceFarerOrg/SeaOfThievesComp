#include "Application.h"
#include <iostream>
#include <string>
#include <SFML\Network\Packet.hpp>
#include "MessageManager.h"
#include "MessageEnum.h"

void CApplication::Init()
{
	std::cout << "Welcome to the Thief of Seas server setup!" << std::endl;
	std::cout << "Please enter a port to start the server on" << std::endl;
	std::cout << ">";
	
	std::cin >> myPort;

	std::cout << std::endl;
	std::cout << "Attempting to create server" << std::endl;

	sf::Socket::Status res = mySocket.bind(myPort);

	if (res == sf::Socket::Error)
	{
		myShouldRun = false;
		std::cout << "Error creating server!" << std::endl;
		return;
	}

	std::cout << "Server setup complete!" << std::endl;
	std::cout << "Generating world" << std::endl;
}

void CApplication::Update()
{
	sf::Packet recieved;
	sf::IpAddress ipOfSender;
	unsigned short port;

	if (mySocket.receive(recieved, ipOfSender, port) == sf::Socket::Status::Done)
	{
		ENetMessageType type = CMessageManager::GetInstance().GetMessageType(recieved);

		//HANDLE MESSAGES
	}
}

void CApplication::ShutDown()
{
	mySocket.unbind();
}

bool CApplication::GetShouldRun()
{
	return myShouldRun;
}