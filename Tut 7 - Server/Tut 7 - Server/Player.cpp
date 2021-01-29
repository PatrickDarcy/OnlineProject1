#include "Player.h"

float Player::s_radius = 20.0f;
float Player::s_speed = 5.0f;
float Player::s_worldWidth = 600.0f;
float Player::s_worldHeight = 600.0f;

Player::Player()
{
	m_player.setPosition(sf::Vector2f(0, 0));
	m_player.setRadius(s_radius);
	m_player.setOrigin(s_radius,s_radius);
}

void Player::render(sf::RenderWindow& t_window)
{
	t_window.draw(m_player);
}

sf::Vector2f Player::movePlayer()
{
	sf::Vector2f moveVector = sf::Vector2f(0.0f, 0.0f);

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
	{
		moveVector += sf::Vector2f(0.0f, -1.0f);
	}
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
	{
		moveVector += sf::Vector2f(0.0f, 1.0f);
	}

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
	{
		moveVector += sf::Vector2f(-1.0f, 0.0f);
	}
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
	{
		moveVector += sf::Vector2f(1.0f, 0.0f);
	}

	moveVector = VecMath::unit(moveVector);
	moveVector = VecMath::applyScaler(moveVector, s_speed);

	return moveVector;
}

void Player::setColour(sf::Color t_colour)
{
	m_player.setFillColor(t_colour);
}

void Player::setPosition(sf::Vector2f t_pos)
{
	m_player.setPosition(t_pos);
}

sf::Vector2f Player::getPosition()
{
	return m_player.getPosition();
}

void Player::updatePos(sf::Vector2f t_velocity)
{
	m_player.setPosition(m_player.getPosition() + t_velocity);
	checkBoundary();
}

void Player::checkBoundary()
{
	sf::Vector2f m_position = m_player.getPosition();

	if (m_position.x + s_radius < 0)
	{
		m_position.x = s_worldWidth + s_radius;
	}

	else if (m_position.x - s_radius > s_worldWidth)
	{
		m_position.x = 0 - s_radius;
	}

	if (m_position.y + s_radius < 0)
	{
		m_position.y = s_worldWidth + s_radius;
	}

	else if (m_position.y - s_radius > s_worldWidth)
	{
		m_position.y = 0 - s_radius;
	}

	m_player.setPosition(m_position);
}
