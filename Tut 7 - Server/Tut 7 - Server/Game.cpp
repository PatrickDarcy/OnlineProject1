#include "Game.h"

Game::Game() :
	m_window(sf::VideoMode{ 600,600,32 }, "Tag Project"),
	m_state(GameState::SELECT)
{
	m_players[0].setColour(sf::Color::Red);
	m_players[0].setPosition(sf::Vector2f(50.0f, 50.0f));

	m_players[1].setColour(sf::Color::Green);
	m_players[1].setPosition(sf::Vector2f(500.0f, 50.0f));

	m_players[2].setColour(sf::Color::Blue);
	m_players[2].setPosition(sf::Vector2f(500.0f, 500.0f));

	m_ipInput = "127.0.0.1";

	m_font.loadFromFile("fonts/LTInternet-Regular/tff");

	m_gameTimeText.setFont(m_font);
	m_gameTimeText.setCharacterSize(40);
	m_gameTimeText.setPosition(300, 300);

	reset();
}

void Game::run()
{
	sf::Clock clock;
	sf::Time timeSinceLastUpdate = sf::Time::Zero;
	sf::Time timePerFrame = sf::seconds(1.f / 60.f);	//60 fps

	while (m_window.isOpen())
	{
		processEvents();								//As many as possible
		timeSinceLastUpdate += clock.restart();
		while (timeSinceLastUpdate > timePerFrame)
		{
			timeSinceLastUpdate -= timePerFrame;
			processEvents();							//At least 60 fps
			update(timePerFrame);						//60 fps
		}
		render();										//As many as possible
	}
}

void Game::updatePlayer(PlayerData t_data)
{
	m_players[t_data.m_playerIndex].updatePos(t_data.m_velocity);
}

void Game::updateAllPositions(std::array<sf::Vector2f, 3> t_positions)
{
	for (int i = 0; i< 3;i++)
	{
		m_players[i].setPosition(t_positions[i]);
	}
}

void Game::setStartData(StartData t_data)
{
	changeState(GameState::START);

	m_targetIndex = t_data.m_targetIndex;
	m_playerIndex = t_data.m_playerIndex;
}

void Game::setEndData(EndData t_data)
{
	changeState(GameState::GAMEOVER);

	std::stringstream timeStream;

	timeStream << std::fixed << std::setprecision(2) << t_data.m_timeLasted.asSeconds();

	std::string timeString = timeStream.str();

	if (t_data.m_playerIndex == m_playerIndex)
	{
		m_gameTimeText.setString(" lasted " + timeString + "s");
	}
	else if (t_data.m_targetIndex == m_playerIndex)
	{
		m_gameTimeText.setString("You Lasted " + timeString + "s");
	}
	else
	{
		m_gameTimeText.setString(" lasted " + timeString + "s");
	}

}

void Game::changeState(GameState t_changeState)
{
	m_state = t_changeState;

	if (m_state == GameState::WAITING)
	{
		reset();
	}
}

void Game::listenForConnections()
{
	while (true)
	{
		m_server->ListenForNewConnection();
	}
}

void Game::startServer()
{
	m_host = true;
	m_server = new Server(8000, "127.0.0.1");

	std::thread serverThread(&Game::listenForConnections, this);
	serverThread.detach();

	m_serverThread = &serverThread;
}

void Game::connect()
{
	m_client = new Client("127.0.0.1", 1111);
	
	if (!m_client->Connect())
	{
		m_client->CloseConnection();
		m_window.close();
	}
}

void Game::update(sf::Time dt)
{
	switch (m_state)
	{
	case GameState::GAMEPLAY:
		updateGameplay(dt);
		break;
	case GameState::START:
		updateStart(dt);
		break;
	default:
		break;
	}
}

void Game::processEvents()
{
	sf::Event event;

	while (m_window.pollEvent(event))
	{
		sf::Vector2f m_pos = m_window.mapPixelToCoords(sf::Mouse::getPosition(m_window));

		switch (m_state)
		{
		case GameState::SELECTBROADCAST:
			startServer();
			break;
		default:
			break;
		}

		if (event.type == sf::Event::KeyPressed)
		{
			switch (event.key.code)
			{
			case sf::Keyboard::Escape:
				if (!m_host)
				{
					if (m_client != nullptr)
					{
						m_client->disconnect();
					}
				}
				m_window.close();
				break;
			default:
				break;
			}
		}

		if (event.type == sf::Event::Closed)
		{
			if (!m_host)
			{
				if (m_client != nullptr)
				{
					m_client->disconnect();
				}
			}
			m_window.close();
		}
	}
}

void Game::render()
{
	m_window.clear();
	m_players[0].render(m_window);
	m_players[1].render(m_window);
	m_players[2].render(m_window);
	m_window.display();

	if (m_state == GameState::GAMEOVER)
	{
		m_window.draw(m_gameTimeText);
	}
}

void Game::updateGameplay(sf::Time t_dt)
{
	sf::Vector2f moveVector = m_players[m_playerIndex].movePlayer();

	if (m_host)
	{
		m_players[m_playerIndex].updatePos(moveVector);
		m_gameTime += t_dt;

		std::array<sf::Vector2f, 3> positions;

		for (int i = 0; i < 3; i++)
		{
			positions[i] = m_players[i].getPosition();
		}

		m_server->sendGameUpdate(positions);
	}
	else
	{
		if (VecMath::magnitude(moveVector) != 0)
		{
			PlayerData data;
			data.m_playerIndex = m_playerIndex;
			data.m_velocity = moveVector;
			m_client->SendPlayerUpdate(data);
		}
	}
}

void Game::updateStart(sf::Time t_dt)
{
	if (m_host)
	{
		m_countDownTime -= t_dt;

		if (m_countDownTime <= sf::seconds(0.0f))
		{
			m_state = GameState::GAMEPLAY;
			m_server->sendChangeState(GameState::GAMEPLAY);
		}
	}
}

void Game::reset()
{
	m_players[0].setPosition(sf::Vector2f(50.0f, 50.0f));

	m_players[1].setPosition(sf::Vector2f(500.0f, 50.0f));

	m_players[2].setPosition(sf::Vector2f(500.0f, 500.0f));
	m_gameTime = sf::seconds(0.0f);
}
