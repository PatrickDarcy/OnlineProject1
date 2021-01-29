#pragma once
#include "SFML/Graphics.hpp"
#include "VecMath.h"

class Player
{
public:
	Player();
	void render(sf::RenderWindow& t_window);
	sf::Vector2f movePlayer();
	void setColour(sf::Color t_colour);
	void setPosition(sf::Vector2f t_pos);

	sf::Vector2f getPosition();

	void updatePos(sf::Vector2f t_pos);

	static float s_radius;
	static float s_speed;

private:
	sf::CircleShape m_player;
};

