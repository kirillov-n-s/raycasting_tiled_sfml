#pragma once
#include <thread>
#include <chrono>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include "source.h"

using namespace std::chrono_literals;

class application
{
	tileworld* _world;
	source* _src;

	std::string _title;
	sf::RenderWindow* _window;

	uint32_t _tile_dim;
	sf::RectangleShape _tile;
	sf::CircleShape _source;
	sf::CircleShape _field;
	sf::CircleShape _intersect;
	sf::CircleShape _corner;

	vec2f get_rad_vec(const sf::CircleShape& circle) const;

	void draw_tile(uint32_t x, uint32_t y);
	void draw_intersect(const vec2f& pos, const sf::Color& color);
	void draw_corner(const vec2f& pos);
	void draw_source();
	void draw_field(float rad, const sf::Color& color);
	void draw_line(const vec2f& a, const vec2f& b, const sf::Color& color);
	void draw_fan(const vec2f& center, const std::vector<vec2f>& points, const sf::Color& color, bool connect);
	void draw_star(const vec2f& center, const std::vector<vec2f>& points, const sf::Color& color);

	const std::vector<vec2f> DIRS =
	{
		vec2f(0.f, -1.f),
		vec2f(1.f, 0.f),
		vec2f(0.f, 1.f),
		vec2f(-1.f, 0.f)
	};

	bool _show_corners = false;
	bool _trace_mouse = false;
	bool _trace_around = false;
	bool _trace_light = false;
	bool _trace_light_rays = false;
	bool _trace_fov = false;
	bool _trace_fov_rays = false;

	uint32_t _rays_cast = 0;
	uint32_t _rays_drawn = 0;

	void handle_events(float elapsed);
	void render();

public:
	application(tileworld* world, source* source, const std::string& title = "");
	~application();

	void run();
};
