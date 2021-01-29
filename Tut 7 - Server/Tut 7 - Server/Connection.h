#pragma once
#include <WinSock2.h>
#include "PacketManager.h"
class Connection
{
public:
	Connection(SOCKET t_sock) :
		m_socket(t_sock)
	{

	}

	SOCKET m_socket;
	PacketManager m_packetManager;
	int m_id = 0;
};