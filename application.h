#pragma once
#include <thread>
#include <chrono>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include "world.h"

using namespace std::chrono_literals;

class application
{
	world* _world;

	std::string _title;
	sf::RenderWindow* _window;

	uint32_t _tile_dim;
	sf::RectangleShape _tile;
	sf::CircleShape _source;
	sf::CircleShape _dest;
	sf::CircleShape _inter;
	void draw(uint32_t x, uint32_t y);

	void handle_events(float elapsed);
	void update();
	void render();

public:
	application(world* world, const std::string& title = "");
	~application();

	void run();
};
