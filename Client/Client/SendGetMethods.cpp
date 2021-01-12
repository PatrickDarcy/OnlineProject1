#include "Client.h"

bool Client::SendInt(int _int)
{
	int RetnCheck = send(Connection, (char*)&_int, sizeof(int), NULL);
	if (RetnCheck == SOCKET_ERROR)
		return false;
	return true;
}

bool Client::GetInt(int& _int)
{
	int RetnCheck = recv(Connection, (char*)&_int, sizeof(int), NULL);
	if (RetnCheck == SOCKET_ERROR)
		return false;
	return true;
}

bool Client::SendPacketType(Packet _packetType)
{
	int RetnCheck = send(Connection, (char*)&_packetType, sizeof(Packet), NULL);
	if (RetnCheck == SOCKET_ERROR)
		return false;
	return true;
}

bool Client::GetPacketType(Packet& _packet)
{
	int RetnCheck = recv(Connection, (char*)&_packet, sizeof(Packet), NULL);
	if (RetnCheck == SOCKET_ERROR)
		return false;
	return true;
}

bool Client::SendString(std::string _string)
{
	if (!SendPacketType(P_ChatMessage))
		return false;
	int bufferlength = _string.size();
	if (!SendInt(bufferlength))
		return false;
	int RetnCheck = send(Connection, _string.c_str(), bufferlength, NULL);
	if (RetnCheck == SOCKET_ERROR)
		return false;
	return true;
}

bool Client::GetString(std::string& _string)
{
	int bufferLength;
	if (!GetInt(bufferLength))
		return false;
	char* buffer = new char[bufferLength + 1];
	buffer[bufferLength] = '\0';
	int RetnCheck = recv(Connection, buffer, bufferLength, NULL);
	_string = buffer;
	delete[] buffer;
	if (RetnCheck == SOCKET_ERROR)
		return false;
	return true;
}