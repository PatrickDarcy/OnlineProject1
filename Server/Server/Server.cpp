#include "Server.h"

Server::Server(int Port, bool BroadcastPublically)
{
	WSADATA wsaData;
	WORD DllVersion = MAKEWORD(2, 1);
	if (WSAStartup(DllVersion, &wsaData) != 0)
	{
		MessageBoxA(NULL, "Winsock Startup Failed", "Error", MB_OK | MB_ICONERROR);
		exit(0);
	}

	if (BroadcastPublically)
		addr.sin_addr.s_addr = htonl(INADDR_ANY);
	else
		addr.sin_addr.s_addr = inet_addr("127.0.0.1");

	addr.sin_port = htons(Port); // Port
	addr.sin_family = AF_INET; //IPv4 Socket

	slisten = socket(AF_INET, SOCK_STREAM, NULL);
	
	if (bind(slisten, (SOCKADDR*)&addr, sizeof(addr)) == SOCKET_ERROR)
	{
		std::string ErrorMsg = "Failed to bind the address to our listening socket. Winsock Wrror: " + std::to_string(WSAGetLastError());
		MessageBoxA(NULL, ErrorMsg.c_str(), "Error", MB_OK | MB_ICONERROR);
		exit(1);
	}
	if (listen(slisten, SOMAXCONN) == SOCKET_ERROR)
	{
		std::string ErrorMsg = "Failed to listen on listening socket. Winsock Error: " + std::to_string(WSAGetLastError());
		MessageBoxA(NULL, ErrorMsg.c_str(), "Error", MB_OK | MB_ICONERROR);
		exit(1);
	}

	serverPtr = this;
}

bool Server::ListenForNewConnection()
{
	SOCKET newConnection;//Socket to hoild the client's connection
	newConnection = accept(slisten, (SOCKADDR*)&addr, &addrlen);//Accept a new connection 

	if (newConnection == 0)
	{
		std::cout << "Failed to accept the client's connection." << std::endl;
		return false;
	}
	else // if client cinnection properly accepted
	{
		std::cout << "Client Connected!" << std::endl;
		Connections[ConnectionConter] = newConnection;
		CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)ClientHandlerThread, (LPVOID)(ConnectionConter), NULL, NULL);// Create Thread to handle this
		std::string MOTD = "Welcome! This is the Message of the Day.";//Create buffer with message of the day
		SendString(MOTD, ConnectionConter);
		ConnectionConter++;
		return true;
	}
}

bool Server::processPacket(Packet _packetType, int index)
{
	switch (_packetType)
	{
	case P_ChatMessage:
	{
		std::string Message;
		if (!GetString(Message, index))
			return false;
		for (int i = 0; i < ConnectionConter; i++)
		{
			if (i == index)
				continue;
			if (!SendString(Message, i))
			{
				std::cout << "Failed to send message from client: " << index << " to client ID: " << i << std::endl;
			}
		}
		std::cout << "Processed chat message packet from user ID: " << index << std::endl;
		break;
	}

	default:
		std::cout << "Unrecognise packet: " << _packetType << std::endl;
		break;
	}
	return true;
}

void Server::ClientHandlerThread(int index)
{
	Packet packetType{};
	while (true)
	{
		if (!serverPtr->GetPacketType(packetType, index))
			break;
		if (!serverPtr->processPacket(packetType, index))
			break;
	}
	std::cout << "Lost Connection to Client ID: " << index << std::endl;
	closesocket(serverPtr->Connections[index]);
}