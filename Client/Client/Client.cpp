//Client
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#pragma comment(lib,"ws2_32.lib")
#include <WinSock2.h>
#include <string.h>
#include <iostream>

SOCKET Connection;

void ClientThread()
{
	int bufferlength;
	while (true)
	{
		recv(Connection, (char*)&bufferlength, sizeof(int), NULL);
		char* buffer = new char[bufferlength+1];
		buffer[bufferlength] = '\0';
		recv(Connection, buffer, bufferlength, NULL);
		std::cout << buffer << std::endl;
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

	Connection = socket(AF_INET, SOCK_STREAM, NULL);
	if (connect(Connection, (SOCKADDR*)&addr, sizeof(addr)) != 0)
	{
		MessageBoxA(NULL, "Failed to Connect", "Error", MB_OK | MB_ICONERROR);
		return 0;
	}
	std::cout << "Connect!" << std::endl;
	CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)ClientThread, NULL, NULL, NULL);

	std::string buffer;

	while (true)
	{
		std::cin >> buffer;
		int bufferlength = buffer.size();
		send(Connection, (char*)&bufferlength, sizeof(int), NULL);
		send(Connection, buffer.c_str(), bufferlength, NULL);

		Sleep(10);
	}

	system("Pause");
	return 0;
}