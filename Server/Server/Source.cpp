//Server
#include "Server.h"

int main()
{
	Server myServer(1111, false);
	for (int i = 0; i < 100; i++)
	{
		myServer.ListenForNewConnection();
	}

	system("pause");
	return 0;
}