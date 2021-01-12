#pragma once
//Client
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#pragma comment(lib,"ws2_32.lib")
#include <WinSock2.h>
#include <string>
#include <iostream>

static SOCKET Connection;

enum Packet
{
	P_ChatMessage,
	P_Test
};

class Client
{
public:
	Client(std::string IP, int Port);
	bool Connect();
	bool CloseConnection();
	bool SendString(std::string _string);

private:
	bool ProcessPacket(Packet _packetType);
	static void ClientThread();
	
	//Send Func
	bool SendInt(int _int);
	bool SendPacketType(Packet _packetType);

	//Get Func
	bool GetInt(int& _int);
	bool GetPacketType(Packet& _packet);
	bool GetString(std::string& _string);

private:
	SOCKET Connection;
	SOCKADDR_IN addr;
	int sizeofaddr = sizeof(addr);
};

static Client* clientptr;