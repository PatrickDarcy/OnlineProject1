//Server
#include "Server.h"


bool ProcessPacket(int index, Packet packetType)
{
	switch (packetType)
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
		std::cout << "Unrecognise packet: " << packetType << std::endl;
		break;
	}
	return true;
}

void ClientHandlerThread(int index)
{
	Packet packetType{};
	while (true)
	{
		if (!GetPacketType(packetType, index))
			break;
		if (!ProcessPacket(index, packetType))
			break;
	}
	std::cout << "Lost Connection to Client ID: " << index << std::endl;
	closesocket(Connections[index]);
}

int main()
{
	SOCKET newConnection;//Socket to hoild the client's connection
	for (int i = 0; i < 100; i++)
	{
		newConnection = accept(slisten, (SOCKADDR*)&addr, &addrlen);//Accept a new connection 
		if (newConnection == 0)
		{
			std::cout << "Failed to accept the client's connection." << std::endl;
		}
		else // if client cinnection properly accepted
		{
			std::cout << "Client Connected!" << std::endl;
			Connections[i] = newConnection;
			ConnectionConter++;
			CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)ClientHandlerThread, (LPVOID)(i), NULL, NULL);// Create Thread to handle this
			std::string MOTD = "Welcome! This is the Message of the Day.";//Create buffer with message of the day
			SendString(MOTD, i);
		}
	}

	system("pause");
	return 0;
}