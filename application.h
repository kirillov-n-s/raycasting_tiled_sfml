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
	sf::CircleShape _inter;
	sf::CircleShape _corner;
	void draw(uint32_t x, uint32_t y);
	vec2f get_rad_vec(const sf::CircleShape& circle) const;

	const std::vector<vec2f> DIRS =
	{
		vec2f(0.f, -1.f),
		vec2f(1.f, 0.f),
		vec2f(0.f, 1.f),
		vec2f(-1.f, 0.f)
	};

	bool _trace_mouse = false;
	bool _trace_around = false;
	bool _trace_light = false;

	void handle_events(float elapsed);
	void render();

public:
	application(world* world, const std::string& title = "");
	~application();

	void run();
};
