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
	sf::CircleShape _intersect;
	sf::CircleShape _corner;

	vec2f get_rad_vec(const sf::CircleShape& circle) const;

	void draw_tile(uint32_t x, uint32_t y);
	void draw_intersect(const vec2f& pos, const sf::Color& color);
	void draw_corner(const vec2f& pos);
	void draw_source();
	void draw_line(const vec2f& a, const vec2f& b, const sf::Color& color);
	void draw_fan(const vec2f& center, const std::vector<vec2f>& points, const sf::Color& color);
	void draw_star(const vec2f& center, const std::vector<vec2f>& points, const sf::Color& color);

	const std::vector<vec2f> DIRS =
	{
		vec2f(0.f, -1.f),
		vec2f(1.f, 0.f),
		vec2f(0.f, 1.f),
		vec2f(-1.f, 0.f)
	};

	bool _show_corners = true;
	bool _trace_mouse = false;
	bool _trace_around = false;
	bool _trace_fov = false;
	bool _trace_eye_rays = false;

	void handle_events(float elapsed);
	void render();

public:
	application(world* world, const std::string& title = "");
	~application();

	void run();
};
