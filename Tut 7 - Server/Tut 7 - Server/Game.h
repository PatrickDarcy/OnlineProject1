#pragma once
#include <SFML/Graphics.hpp>
#include "Enums.h"
#include "Client.h"
#include "Server.h"
#include "Player.h"
#include <thread>
#include <iomanip>
#include <sstream>
#include <array>

class Game
{
public:
	Game();
	void run();
	void updatePlayer(PlayerData t_data);

	void updateAllPositions(std::array<sf::Vector2f, 3> t_positions);
	void setStartData(StartData t_data);
	void setEndData(EndData t_data);
	void changeState(GameState t_changeState);

private:

	sf::RenderWindow m_window;
	Server* m_server{ nullptr }; //Create server on port 100
	Client* m_client{ nullptr };
	Player m_players[3];

	std::thread* m_serverThread;

	bool m_host{ false };
	int m_playerIndex{ -1 };
	int m_targetIndex{ -1 };
	GameState m_state;

	std::string m_ipInput;
	std::array<char, 11> m_validInput = { '0','1','2','3','4','5','6','7','8','9','.' };
	sf::Time m_gameTime;
	sf::Time m_countDownTime;

	void listenForConnections();
	void startServer();
	void connect();
	void update(sf::Time dt);
	void processEvents();
	void render();

	void updateGameplay(sf::Time t_dt);
	void updateStart(sf::Time t_dt);
	void reset();

};

