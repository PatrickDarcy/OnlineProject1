#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define NOMINMAX
#pragma comment(lib,"ws2_32.lib") //Required for WinSock
#include <WinSock2.h> //For win sockets
#include <string> //For std::string
#include "Enums.h"
#include <iostream> //For std::cout, std::endl, std::cin.getline
#include "PacketStructure.h"
#include "Game.h"
#include "PacketManager.h"
#include <WS2tcpip.h>

class Game;

class Client
{
public: //Public functions
	Client(const char* IP, int PORT);
	~Client();

	bool Connect();
	void disconnect();

	bool SendPlayerUpdate(const PlayerData& t_playerUpdate);
	void sendGameStart(const StartData& t_startData);
	bool CloseConnection();

	Game* m_game;

private: //Private functions
	bool ProcessPacket(const PacketType _packettype);

	//Sending Funcs
	bool sendall(const char * data, int totalbytes);
	bool SendInt(int _int);
	bool SendPacketType(Packet _packettype);
	

	//Getting Funcs
	bool recvall(char * data, int totalbytes);
	bool GetInt(int & _int);
	bool GetPacketType(PacketType & _packettype);
	bool GetString(std::string & _string);

	static void clientThread(Client& t_client);
	static void packetSenderThread(Client& t_client);

	bool GetGameStart(StartData& t_data);
	bool GetGameEnd(EndData& t_data);
	bool GetStateChange(GameState& t_state);
	bool GetGameUpdate(std::array<sf::Vector2f, 3>& t_position);

private:
	SOCKET Connection;//This client's connection to the server
	SOCKADDR_IN addr; //Address to be binded to our Connection socket

	bool m_endThreads = false;
	bool m_connected = false;
	PacketManager m_packetManager;

	std::thread m_packetSendThread;
	std::thread m_listenThread;
};