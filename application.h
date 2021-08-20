#pragma once
#include <chrono>
#include <variant>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include "source.h"

using namespace std::chrono_literals;

class application
{
	const std::vector<vec2f> DIRS =
	{
		vec2f(0.f, -1.f),
		vec2f(1.f, 0.f),
		vec2f(0.f, 1.f),
		vec2f(-1.f, 0.f)
	};

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

	void render_layout();
	void render_shadows(bool rays, bool connect, const sf::Color& color);
	void trace_closest_collision();
	void trace_mouse();
	void show_corners();

	mode _mode = none;
	void toggle_mode(mode mode);
	bool _show_corners = false;
	bool _trace_mouse = false;

	float _elapsed = 0.f;

	std::variant<
		std::pair<std::vector<vec2f>, uint32_t>,
		std::pair<vec2f, uint32_t>
	> _data;

	uint32_t _rays_cast = 0;
	uint32_t _rays_drawn = 0;

	void handle_events();
	void update();
	void render();

public:
	application(tileworld* world, source* source, const std::string& title = "");
	~application();

	void run();
};
