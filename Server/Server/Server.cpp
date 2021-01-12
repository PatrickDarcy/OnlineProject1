#include "Server.h"

Server::Server(int Port, bool BroadcastPublically = false)
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

	bind(slisten, (SOCKADDR*)&addr, sizeof(addr));// Bind the address to the socket
	listen(slisten, SOMAXCONN);//places slisten socket in a state on which itis listening for an incoming connection

}