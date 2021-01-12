#include"Client.h"

Client::Client(std::string IP, int Port)
{
	WSADATA wsaData;
	WORD DllVersion = MAKEWORD(2, 1);
	if (WSAStartup(DllVersion, &wsaData) != 0)
	{
		MessageBoxA(NULL, "Winsock Startup Failed", "Error", MB_OK | MB_ICONERROR);
		exit(0);
	}

	addr.sin_addr.s_addr = inet_addr(IP.c_str());// Broadcast locally
	addr.sin_port = htons(Port); // Port
	addr.sin_family = AF_INET; //IPv4 Socket
	clientptr = this;
}

bool Client::Connect()
{
	Connection = socket(AF_INET, SOCK_STREAM, NULL);
	if (connect(Connection, (SOCKADDR*)&addr, sizeof(addr)) != 0)
	{
		MessageBoxA(NULL, "Failed to Connect", "Error", MB_OK | MB_ICONERROR);
		return false;
	}
	CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)ClientThread, NULL, NULL, NULL);
	return true;
}

bool Client::ProcessPacket(Packet packetType)
{
	switch (packetType)
	{
	case P_ChatMessage:
	{
		std::string Message;
		if (!GetString(Message))
			return false;
		std::cout << Message << std::endl;
		break;
	}
	default:
		std::cout << "Unrecognise packet: " << packetType << std::endl;
		break;
	}
	return true;
}

bool Client::CloseConnection()
{
	if (closesocket(Connection) == SOCKET_ERROR)
	{
		if (WSAGetLastError() == WSAENOTSOCK)
			return true;

		std::string ErrorMessage = "Failed to close the socket. Winsock Error: " + std::to_string(WSAGetLastError()) + ".";
		MessageBoxA(NULL, ErrorMessage.c_str(), "Error", MB_OK | MB_ICONERROR);
		return false;
	}
	return true;
}

void Client::ClientThread()
{
	Packet packetType{};

	while (true)
	{
		if (!clientptr->GetPacketType(packetType))
			break;
		if (!clientptr->ProcessPacket(packetType))
			break;
	}
	std::cout << "Lost Connection to the server" << std::endl;
	if (clientptr->CloseConnection())
	{
		std::cout << "Socket to the server was closed successfully" << std::endl;
	}
	else
	{
		std::cout << "Socket was not able to be closed" << std::endl;
	}
}