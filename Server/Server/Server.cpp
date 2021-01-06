//Server
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#pragma comment(lib,"ws2_32.lib")
#include <WinSock2.h>
#include <iostream>

SOCKET Connections[100];
int ConnectionConter = 0;

void ClientHandlerThread(int index)
{
	int bufferlength;

	while (true)
	{
		recv(Connections[index], (char*)&bufferlength, sizeof(bufferlength), NULL);
		char* buffer = new char[bufferlength];
		recv(Connections[index], buffer, bufferlength, NULL);
		
		for (int i = 0; i < ConnectionConter; i++)
		{
			if (i == index)continue;
			send(Connections[i], (char*)&bufferlength, sizeof(int), NULL);
			send(Connections[i], buffer, bufferlength, NULL);
		}

		delete[] buffer;
	}
}

int main()
{
	WSADATA wsaData;
	WORD DllVersion = MAKEWORD(2, 1);
	if (WSAStartup(DllVersion, &wsaData) != 0)
	{
		MessageBoxA(NULL, "Winsock Startup Failed", "Error", MB_OK | MB_ICONERROR);
		exit(1);
	}

	SOCKADDR_IN addr; //Address that we will bind our listeningsocket to
	int addrlen = sizeof(addr);//lenght of the address (required to accept call)
	addr.sin_addr.s_addr = inet_addr("127.0.0.1");// Broadcast locally
	addr.sin_port = htons(1111); // Port
	addr.sin_family = AF_INET; //IPv4 Socket

	SOCKET slisten = socket(AF_INET, SOCK_STREAM, NULL);//Create socket so listen for new connections
	bind(slisten, (SOCKADDR*)&addr, sizeof(addr));// Bind the address to the socket
	listen(slisten, SOMAXCONN);//places slisten socket in a state on which itis listening for an incoming connection

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
			std::string MOTD = "Welcome! This is the Message of the Day.";//Create buffer with message of the day
			int MOTDLenght = MOTD.size();
			send(newConnection, (char*)&MOTDLenght, sizeof(int), NULL);
			send(newConnection, MOTD.c_str(), MOTDLenght, NULL);//Send MOTD buffer
			Connections[i] = newConnection;
			ConnectionConter++;
			CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)ClientHandlerThread, (LPVOID)(i), NULL, NULL);// Create Thread to handle this
		}
	}

	system("pause");
	return 0;
}