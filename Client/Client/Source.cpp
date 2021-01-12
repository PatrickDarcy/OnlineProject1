#include"Client.h"

int main()
{
	Client myClient("127.0.0.1", 1111);

	if (!myClient.Connect())
	{
		std::cout << "Failed to connect to server ...." << std::endl;
		system("pause");
		return 1;
	}

	std::string buffer;

	while (true)
	{
		std::cin >> buffer;
		if (!myClient.SendString(buffer))
			break;

		Sleep(10);
	}

	system("Pause");
	return 0;
}