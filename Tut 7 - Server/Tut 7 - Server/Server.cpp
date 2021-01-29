#include "Server.h"
#include <iostream>
#include "PacketStructure.h"
#pragma comment(lib,"ws2_32.lib")
#include "Game.h"

Server::Server(int t_port, bool t_broadcastPublically)
{
	//Winsock Startup
	WSAData wsaData;
	WORD dllVersion = MAKEWORD(2, 1);

	if (WSAStartup(dllVersion, &wsaData) != 0)
	{
		MessageBoxA(NULL, "WinSock startup failed", "Error", MB_OK | MB_ICONERROR);
		exit(1);
	}

	if (t_broadcastPublically)
	{
		addr.sin_addr.s_addr = htonl(INADDR_ANY);
	}

	else
	{
		addr.sin_addr.s_addr = inet_addr("127.0.0.1"); //Broadcast locally
	}

	addr.sin_port = htons(t_port);					//Port
	addr.sin_family = AF_INET;						//IPv4 Socket
	sListen = socket(AF_INET, SOCK_STREAM, NULL);	//Create socket to listen for new connections

	if (bind(sListen, (SOCKADDR*)&addr, sizeof(addr)) == SOCKET_ERROR)
	{
		std::string ErrorMsg = "Failed to bind the address to our listening socket. Winsock Error:" + std::to_string(WSAGetLastError());
		MessageBoxA(NULL, ErrorMsg.c_str(), "Error", MB_OK | MB_ICONERROR);
		exit(1);
	}

	if (listen(sListen, SOMAXCONN) == SOCKET_ERROR)
	{
		std::string ErrorMsg = "Failed to listen on listening socket. Winsock Error:" + std::to_string(WSAGetLastError());
		MessageBoxA(NULL, ErrorMsg.c_str(), "Error", MB_OK | MB_ICONERROR);
		exit(1);
	}

	m_idCounter = 0;

	std::thread PST(packetSenderThread, std::ref(*this));
	PST.detach();
	m_threads.push_back(&PST);
}

Server::~Server()
{
	m_terminateThreads = true;

	for (std::thread* t : m_threads)
	{
		t->join();
	}
}

bool Server::ListenForNewConnection()
{
	//Check if we have two connections if yes we will not bind an incoming connection.
	if (m_activeConnectionsCount == 2)
	{
		return false;
	}

	int addrlen = sizeof(addr);
	SOCKET newConnectionSocket = accept(sListen, (SOCKADDR*)&addr, &addrlen); //Accept a new connection

	if (newConnectionSocket == 0)
	{
		std::cout << "Failed to accept the client's connection." << std::endl;
		return false;
	}
	else
	{
		std::lock_guard<std::shared_mutex> lock(m_mutexConManager);
		std::shared_ptr<Connection> newConnection(std::make_shared<Connection>(newConnectionSocket));

		m_connections.push_back(newConnection);

		newConnection->m_id = m_idCounter;

		m_idCounter += 1;
		m_activeConnectionsCount += 1;

		std::cout << "Client Connected! ID:" << newConnection->m_id << std::endl;
		std::thread clientHandlerThread(ClientHandlerThread, std::ref(*this), newConnection);
		clientHandlerThread.detach();
		m_threads.push_back(&clientHandlerThread);
		return true;
	}
}

bool Server::ProcessPacket(std::shared_ptr<Connection> t_connection, PacketType t_packetType)
{
	switch (t_packetType)
	{
	case PacketType::PLAYERSET:
	{
		StartData data;
		if (!getGameStart(t_connection, data))
		{
			return false;
		}
		break;
	}
	case PacketType::PLAYERUPDATE:
	{
		PlayerData data;
		if (!getPlayerUpdate(t_connection, data))
		{
			return false;
		}
		m_game->updatePlayer(data);
		break;
	}
	default:
	{
		std::cout << "Unrecognized packet: " << (std::int32_t)t_packetType << std::endl;
		return false;
	}
	}
}

void Server::ClientHandlerThread(Server& t_server, std::shared_ptr<Connection> t_connection)
{
	PacketType packetType;
	while (true)
	{
		if (t_server.m_terminateThreads == true)
		{
			break;
		}

		if (!t_server.getPacketType(t_connection, packetType))
		{
			break;
		}
		if (!t_server.ProcessPacket(t_connection, packetType))
		{
			break;
		}
	}

	std::cout << "Lost connection to client ID: " << t_connection->m_id << std::endl;
	t_server.disconnect(t_connection);
	return;
}

void Server::packetSenderThread(Server& t_server)
{
	while (true)
	{
		if (t_server.m_terminateThreads == true)
		{
			break;
		}

		for (auto connection : t_server.m_connections)
		{
			if (connection->m_packetManager.hasPendingPackets())
			{
				std::shared_ptr<Packet> p = connection->m_packetManager.retrieve();
				if (!t_server.sendall(connection, (const char*)(&p->m_buffer[0]), p->m_buffer.size())) //send packet to connection
				{
					std::cout << "Failed to send packet to ID: " << connection->m_id << std::endl; //Print out if failed to send packet
				}
			}
		}
		Sleep(5);
	}

	std::cout << "Ending Packet Sender Thread..." << std::endl;
}

void Server::disconnect(std::shared_ptr<Connection> t_connection)
{
	std::lock_guard<std::shared_mutex> lock(m_mutexConManager);
	t_connection->m_packetManager.clear();
	closesocket(t_connection->m_socket);

	m_connections.erase(std::remove(m_connections.begin(), m_connections.end(), t_connection));

	std::cout << "Cleaned up client: " << t_connection->m_id << "." << std::endl;
	std::cout << "Total connections: " << m_connections.size() << std::endl;

	m_activeConnectionsCount -= 1;

	m_game->changeState(GameState::WAITING);
}

