#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#pragma comment(lib,"ws2_32.lib")
#include <WinSock2.h>
#include "Enums.h"
#include <string>
#include <iostream>
#include "Game.h"
#include "Connection.h"
#include <thread>
#include <shared_mutex>

class Game;

class Server
{
public:
	Server(int PORT, bool BroadcastPublically = false);
	bool ListenForNewConnection();

	int m_idCounter = 0;
	int m_activeConnectionsCount = 0;

	bool getint32t(std::shared_ptr<Connection> t_connection, std::int32_t& t_int32t);
	bool getPacketType(std::shared_ptr<Connection> t_connection, PacketType& t_packetType);

	void sendPlayerUpdate(PlayerData& t_updateData);
	bool getPlayerUpdate(std::shared_ptr<Connection> t_connection, PlayerData& t_updateData);

	void sendGameStart(StartData& t_startData, int t_index);
	bool getGameStart(std::shared_ptr<Connection> t_connection, StartData& t_startData);

	void sendGameEnd(EndData& t_endData);

	void sendGameUpdate(std::array<sf::Vector2f, 3>& t_positions);

	void sendChangeState(const GameState& t_gameState);

	Game* m_game;
private:
	bool sendall(std::shared_ptr<Connection> t_connection,const char * data, int totalbytes);
	bool recvall(std::shared_ptr<Connection> t_connection,const char * data, int totalbytes);

	bool SendInt(std::shared_ptr<Connection> t_connection, int _int);
	bool GetInt(std::shared_ptr<Connection> t_connection, int & _int);

	bool SendPacketType(std::shared_ptr<Connection> t_connection, PacketType _packettype);
	bool GetPacketType(std::shared_ptr<Connection> t_connection, PacketType& _packettype);

	bool ProcessPacket(std::shared_ptr<Connection> t_connection, PacketType _packettype);

	static void ClientHandlerThread(Server& t_server,std::shared_ptr<Connection> t_connection);
	static void packetSenderThread(Server& t_server);

	void disconnect(std::shared_ptr<Connection> t_connection);

private:
	std::vector<std::shared_ptr<Connection>> m_connections;
	std::shared_mutex m_mutexConManager;

	SOCKADDR_IN addr; //Address that we will bind our listening socket to
	SOCKET sListen;

	bool m_terminateThreads = false;
	std::vector<std::thread*> m_threads;
};