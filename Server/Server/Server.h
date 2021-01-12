#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#pragma comment(lib,"ws2_32.lib")
#include <WinSock2.h>
#include <iostream>

enum Packet
{
	P_ChatMessage,
	P_Test
};


class Server
{
public: 
	Server(int Port, bool BroadcastPublically = false);
	bool ListenForNewConnection();

private:
	bool SendInt(int _int, int index);
	bool GetInt(int& _int, int index);

	bool SendPacketType(Packet _packetType, int index);
	bool GetPacketType(Packet& _packetType, int index);

	bool SendString(std::string& _string, int index);
	bool GetString(std::string& _string, int index);

	bool processPacket(Packet _packetType, int index);

	static void ClientHandlerThread(int index);



private:
	SOCKET Connections[100];
	int ConnectionConter = 0;

	SOCKADDR_IN addr; //Address that we will bind our listeningsocket to
	int addrlen = sizeof(addr);//lenght of the address (required to accept call)
	SOCKET slisten;//Create socket so listen for new connections
};

Server* serverPtr;