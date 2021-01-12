#include"Server.h"

bool Server::SendInt(int _int, int index)
{
	int RetnCheck = send(Connections[index], (char*)&_int, sizeof(int), NULL);
	if (RetnCheck == SOCKET_ERROR)
		return false;
	return true;
}

bool Server::GetInt(int& _int, int index)
{
	int RetnCheck = recv(Connections[index], (char*)&_int, sizeof(int), NULL);
	if (RetnCheck == SOCKET_ERROR)
		return false;
	return true;
}

bool Server::SendPacketType(Packet _packet, int index)
{
	int RetnCheck = send(Connections[index], (char*)&_packet, sizeof(Packet), NULL);
	if (RetnCheck == SOCKET_ERROR)
		return false;
	return true;
}

bool Server::GetPacketType(Packet& _packet, int index)
{
	int RetnCheck = recv(Connections[index], (char*)&_packet, sizeof(Packet), NULL);
	if (RetnCheck == SOCKET_ERROR)
		return false;
	return true;
}

bool Server::SendString(std::string& _string, int index)
{
	if (!SendPacketType(P_ChatMessage, index))
		return false;
	int bufferlength = _string.size();
	if (!SendInt(bufferlength, index))
		return false;
	int RetnCheck = send(Connections[index], _string.c_str(), bufferlength, NULL);
	if (RetnCheck == SOCKET_ERROR)
		return false;
	return true;
}

bool Server::GetString(std::string& _string, int index)
{
	int bufferLength;
	if (!GetInt(bufferLength, index))
		return false;
	char* buffer = new char[bufferLength + 1];
	buffer[bufferLength] = '\0';
	int RetnCheck = recv(Connections[index], buffer, bufferLength, NULL);
	_string = buffer;
	delete[] buffer;
	if (RetnCheck == SOCKET_ERROR)
		return false;
	return true;
}
