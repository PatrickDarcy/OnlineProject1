#include "Server.h"
#include "Client.h"

bool Server::recvall(std::shared_ptr<Connection> t_connection,const char * data, int totalbytes)
{
	int bytesreceived = 0; //Holds the total bytes received
	while (bytesreceived < totalbytes) //While we still have more bytes to recv
	{
		int RetnCheck = recv(t_connection->m_socket, data + bytesreceived, totalbytes - bytesreceived, NULL); //Try to recv remaining bytes
		if (RetnCheck == SOCKET_ERROR) //If there is a socket error while trying to recv bytes
			return false; //Return false - failed to recvall
		bytesreceived += RetnCheck; //Add to total bytes received
	}
	return true; //Success!
}

bool Server::sendall(std::shared_ptr<Connection> t_connection,const char * data, int totalbytes)
{
	int bytessent = 0; //Holds the total bytes sent
	while (bytessent < totalbytes) //While we still have more bytes to send
	{
		int RetnCheck = send(t_connection->m_socket, data + bytessent, totalbytes - bytessent, NULL); //Try to send remaining bytes
		if (RetnCheck == SOCKET_ERROR) //If there is a socket error while trying to send bytes
			return false; //Return false - failed to sendall
		bytessent += RetnCheck; //Add to total bytes sent
	}
	return true; //Success!
}

bool Server::SendInt(std::shared_ptr<Connection> t_connection, int _int)
{
	if (!sendall(t_connection, (char*)&_int, sizeof(int))) //Try to send int... If int fails to send
		return false; //Return false: int not successfully sent
	return true; //Return true: int successfully sent
}

bool Server::GetInt(std::shared_ptr<Connection> t_connection, int & _int)
{
	if (!recvall(t_connection, (char*)&_int, sizeof(int))) //Try to receive int... If int fails to be recv'd
		return false; //Return false: Int not successfully received
	return true;//Return true if we were successful in retrieving the int
}

bool Server::SendPacketType(std::shared_ptr<Connection> t_connection, PacketType _packettype)
{
	if (!sendall(t_connection, (char*)&_packettype, sizeof(Packet))) //Try to send packet type... If packet type fails to send
		return false; //Return false: packet type not successfully sent
	return true; //Return true: packet type successfully sent
}

bool Server::GetPacketType(std::shared_ptr<Connection> t_connection, PacketType& _packettype)
{
	if (!recvall(t_connection, (char*)&_packettype, sizeof(Packet))) //Try to receive packet type... If packet type fails to be recv'd
		return false; //Return false: packet type not successfully received
	return true;//Return true if we were successful in retrieving the packet type
}
bool Server::getPacketType(std::shared_ptr<Connection> t_connection, PacketType& t_packetType)
{
	std::int32_t packetTypeInt;

	if (!getint32t(t_connection, packetTypeInt))
	{
		return false;
	}

	t_packetType = (PacketType)packetTypeInt;
	return true;
}

void Server::sendPlayerUpdate(PlayerData& t_updateData)
{
	PS::PlayerUpdate gameUpdate(t_updateData);
	std::shared_ptr<Packet> updatePacket = std::make_shared<Packet>(gameUpdate.toPacket());
	{
		std::shared_lock<std::shared_mutex> lock(m_mutexConManager);

		for (auto connection : m_connections) //For each connection...
		{
			connection->m_packetManager.append(updatePacket);
		}
	}
}

bool Server::getPlayerUpdate(std::shared_ptr<Connection> t_connection, PlayerData& t_updateData)
{
	std::int32_t bufferlength;

	if (!getint32t(t_connection, bufferlength))
	{
		return false;
	}

	if (bufferlength == 0)
	{
		return true;
	}

	return recvall(t_connection, (char*)&t_updateData, bufferlength);;
}

void Server::sendGameStart(StartData& t_startData, int t_index)
{
	PS::GameStart gameStart(t_startData);
	std::shared_ptr<Packet> startPacket = std::make_shared<Packet>(gameStart.toPacket());
	std::shared_lock<std::shared_mutex> lock(m_mutexConManager);

	m_connections[t_index]->m_packetManager.append(startPacket);
}

bool Server::getGameStart(std::shared_ptr<Connection> t_connection, StartData& t_startData)
{
	std::int32_t bufferlength;

	if (!getint32t(t_connection, bufferlength))
	{
		return false;
	}

	if (bufferlength == 0)
	{
		return true;
	}

	return recvall(t_connection, (char*)&t_startData, bufferlength);;
}

void Server::sendGameEnd(EndData& t_endData)
{
	PS::GameEnd gameEnd(t_endData);

	std::shared_ptr<Packet> startPacket = std::make_shared<Packet>(gameEnd.toPacket());
	std::shared_lock<std::shared_mutex> lock(m_mutexConManager);

	for (auto connection : m_connections)
	{
		connection->m_packetManager.append(startPacket);
	}
}

void Server::sendGameUpdate(std::array<sf::Vector2f, 3>& t_positions)
{
	PS::GameUpdate gameUpdate(t_positions);
	std::shared_ptr<Packet> updatePacket = std::make_shared<Packet>(gameUpdate.toPacket());
	std::shared_lock<std::shared_mutex> lock(m_mutexConManager);

	for (auto connection : m_connections)
	{
		connection->m_packetManager.append(updatePacket);
	}
}

void Server::sendChangeState(const GameState& t_gameState)
{
	PS::ChangeState changeState(t_gameState);

	std::shared_ptr<Packet> startPacket = std::make_shared<Packet>(changeState.toPacket());
	std::shared_lock<std::shared_mutex> lock(m_mutexConManager);

	for (auto connection : m_connections)
	{
		connection->m_packetManager.append(startPacket);
	}
}


//////////////////////////////////CLIENT///////////////////////////////////


bool Client::recvall(char* data, int totalbytes)
{
	int bytesreceived = 0; //Holds the total bytes received
	while (bytesreceived < totalbytes) //While we still have more bytes to recv
	{
		int RetnCheck = recv(Connection, data + bytesreceived, totalbytes - bytesreceived, NULL); //Try to recv remaining bytes
		if (RetnCheck == SOCKET_ERROR) //If there is a socket error while trying to recv bytes
			return false; //Return false - failed to recvall
		bytesreceived += RetnCheck; //Add to total bytes received
	}
	return true; //Success!
}

bool Client::sendall(const char* data, int totalbytes)
{
	int bytessent = 0; //Holds the total bytes sent
	while (bytessent < totalbytes) //While we still have more bytes to send
	{
		int RetnCheck = send(Connection, data + bytessent, totalbytes - bytessent, NULL); //Try to send remaining bytes
		if (RetnCheck == SOCKET_ERROR) //If there is a socket error while trying to send bytes
			return false; //Return false - failed to sendall
		bytessent += RetnCheck; //Add to total bytes sent
	}
	return true; //Success!
}
bool Client::SendInt(int _int)
{
	if (!sendall((char*)&_int, sizeof(int))) //Try to send int... If int fails to send
		return false; //Return false: int not successfully sent
	return true; //Return true: int successfully sent
}

bool Client::GetInt(int& _int)
{
	if (!recvall((char*)&_int, sizeof(int))) //Try to receive int... If int fails to be recv'd
		return false; //Return false: Int not successfully received
	_int = ntohl(_int);
	return true;//Return true if we were successful in retrieving the int
}

bool Client::SendPacketType(Packet _packettype)
{
	if (!sendall((char*)&_packettype, sizeof(Packet))) //Try to send packet type... If packet type fails to send
		return false; //Return false: packet type not successfully sent
	return true; //Return true: packet type successfully sent
}

bool Client::GetPacketType(PacketType & _packettype)
{
	std::int32_t packetInt;
	if (!GetInt(packetInt)) //Try to receive packet type... If packet type fails to be recv'd
		return false; //Return false: packet type not successfully received
	_packettype = (PacketType)packetInt;
	return true;//Return true if we were successful in retrieving the packet type
}

bool Client::GetString(std::string& _string)
{
	int bufferlength; //Holds length of the message
	if (!GetInt(bufferlength)) //Get length of buffer and store it in variable: bufferlength
		return false; //If get int fails, return false
	char* buffer = new char[bufferlength + 1]; //Allocate buffer
	buffer[bufferlength] = '\0'; //Set last character of buffer to be a null terminator so we aren't printing memory that we shouldn't be looking at
	if (!recvall(buffer, bufferlength)) //receive message and store the message in buffer array. If buffer fails to be received...
	{
		delete[] buffer; //delete buffer to prevent memory leak
		return false; //return false: Fails to receive string buffer
	}
	_string = buffer; //set string to received buffer message
	delete[] buffer; //Deallocate buffer memory (cleanup to prevent memory leak)
	return true;//Return true if we were successful in retrieving the string
}

bool Client::GetGameStart(StartData& t_data)
{
	int32_t bufferlength;
	if (!GetInt(bufferlength))
	{
		return false;
	}

	if (bufferlength == 0)
	{
		return true;
	}

	return recvall((char*)&t_data, bufferlength);
}

bool Client::GetGameEnd(EndData& t_data)
{
	int32_t bufferlength;
	if (!GetInt(bufferlength))
	{
		return false;
	}

	if (bufferlength == 0)
	{
		return true;
	}

	return recvall((char*)&t_data, bufferlength);
}

bool Client::GetStateChange(GameState& t_state)
{
	int32_t bufferlength;
	if (!GetInt(bufferlength))
	{
		return false;
	}

	if (bufferlength == 0)
	{
		return true;
	}

	return recvall((char*)&t_state, bufferlength);
}

bool Client::GetGameUpdate(std::array<sf::Vector2f, 3>& t_position)
{
	int32_t bufferlength;
	if (!GetInt(bufferlength))
	{
		return false;
	}

	if (bufferlength == 0)
	{
		return true;
	}

	return recvall((char*)&t_position[0], bufferlength);;
}


bool Client::SendPlayerUpdate(const PlayerData& t_playerUpdate)
{
	PS::PlayerUpdate gameUpdate(t_playerUpdate);
	m_packetManager.append(gameUpdate.toPacket());
}

void Client::sendGameStart(const StartData& t_startData)
{
	PS::GameStart gameStart(t_startData);
	m_packetManager.append(gameStart.toPacket());
}