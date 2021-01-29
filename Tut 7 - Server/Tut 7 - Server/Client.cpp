#include "Client.h"

bool Client::ProcessPacket(const PacketType t_packettype)
{
	switch (t_packettype)
	{
	case PacketType::PLAYERSET:
	{
		StartData data;
		if (!GetGameStart(data))
		{
			return false;
		}
		m_game->setStartData(data);
		break;
	}
	case PacketType::PLAYERUPDATE:
	{
		break;
	}
	case PacketType::GAMEUPDATE:
	{
		std::array<sf::Vector2f, 3> positions;
		if (!GetGameUpdate(positions))
		{
			return false;
		}
		m_game->updateAllPositions(positions);
		break;
	}
	case PacketType::GAMEEND:
	{
		EndData data;
		if (!GetGameEnd(data))
		{
			return false;
		}
		m_game->setEndData(data);
		break;
	}
	case PacketType::STATECHANGE:
	{
		GameState changeState;
		if (!GetStateChange(changeState))
		{
			return false;
		}
		m_game->changeState(changeState);
		break;
	}
	default:
		std::cout << "Unrecognized PacketType: " << (std::int32_t)t_packettype << std::endl;
		break;
	}

	return true;
}

void Client::clientThread(Client& t_client)
{
	PacketType PacketType;
	while (true)
	{
		if (t_client.m_endThreads) //Get packet type
			break; //If there is an issue getting the packet type, exit this loop
		if (!t_client.ProcessPacket(PacketType)) //Process packet (packet type)
			break; //If there is an issue processing the packet, exit this loop
		if (t_client.GetPacketType(PacketType))
			break;
	}
	std::cout << "Lost connection to the server." << std::endl;
	t_client.m_endThreads = true;
	if (t_client.CloseConnection()) //Try to close socket connection..., If connection socket was closed properly
	{
		std::cout << "Socket to the server was closed successfuly." << std::endl;
	}
	else //If connection socket was not closed properly for some reason from our function
	{
		std::cout << "Socket was not able to be closed." << std::endl;
	}
	t_client.m_game->changeState(GameState::SERVERCLOSED);
}

Client::Client(const char* IP, int PORT)
{
	//Winsock Startup
	WSAData wsaData;
	WORD DllVersion = MAKEWORD(2, 1);
	if (WSAStartup(DllVersion, &wsaData) != 0)
	{
		MessageBoxA(NULL, "Winsock startup failed", "Error", MB_OK | MB_ICONERROR);
		exit(0);
	}

	inet_pton(AF_INET, IP, &addr.sin_addr.s_addr);
	addr.sin_port = htons(PORT); //Port 
	addr.sin_family = AF_INET; //IPv4 Socket
}

Client::~Client()
{
	CloseConnection();
	m_packetSendThread.join();
	m_listenThread.join();
}

bool Client::Connect()
{
	Connection = socket(AF_INET, SOCK_STREAM, 0);
	int sizeofaddr = sizeof(addr);

	if (connect(Connection, (SOCKADDR*)&addr, sizeofaddr) != 0)
	{
		MessageBoxA(0, "Failed to Connect", "Error", MB_OK | MB_ICONERROR);
		return false;
	}

	std::cout << "Connected!" << std::endl;

	m_packetSendThread = std::thread(packetSenderThread, std::ref(*this));
	m_packetSendThread.detach();

	m_listenThread = std::thread(clientThread, std::ref(*this));
	m_listenThread.detach();

	return true;
}

void Client::disconnect()
{
	m_packetManager.clear();
	closesocket(Connection);
	std::cout << "Disconnected from server." << std::endl;
}

bool Client::CloseConnection()
{
	m_endThreads = true;

	if (closesocket(Connection) == SOCKET_ERROR)
	{
		if (WSAGetLastError() == WSAENOTSOCK) //If socket error is that operation is not performed on a socket (This happens when the socket has already been closed)
			return true; //return true since connection has already been closed

		std::string ErrorMessage = "Failed to close the socket. Winsock Error: " + std::to_string(WSAGetLastError()) + ".";
		MessageBoxA(NULL, ErrorMessage.c_str(), "Error", MB_OK | MB_ICONERROR);
		return false;
	}
	return true;
}

void Client::packetSenderThread(Client& t_client)
{
	while (true)
	{
		if (t_client.m_endThreads == true)
		{
			break;
		}

		while (t_client.m_packetManager.hasPendingPackets())
		{
			std::shared_ptr<Packet> packets = t_client.m_packetManager.retrieve();
			if (!t_client.sendall((const char*)(&packets->m_buffer[0]), packets->m_buffer.size()))
			{
				std::cout << "Failed to send packet to server..." << std::endl;
				break;
			}
		}
		Sleep(5);
	}

	std::cout << "Packet thread closing..." << std::endl;
}