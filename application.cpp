#include "application.h"

void application::draw(uint32_t x, uint32_t y)
{
	_tile.setPosition(vec2f(x * _tile_dim, y * _tile_dim));

	if (_world->tile_solid(x, y))
	{
		_tile.setFillColor(sf::Color::Blue);
		_tile.setOutlineColor(sf::Color::Cyan);
	}
	else
	{
		_tile.setFillColor(sf::Color::Black);
		_tile.setOutlineColor(sf::Color(16, 16, 16));
	}

	_window->draw(_tile);
}

vec2f application::get_rad_vec(const sf::CircleShape& circle) const
{
	auto rad = circle.getRadius();
	return vec2f(rad, rad);
}

void application::handle_events(float elapsed)
{
	sf::Event event;
	while (_window->pollEvent(event))
	{
		if (event.type == sf::Event::Closed)
			_window->close();

		if (event.type == sf::Event::KeyReleased)
		{
			switch (event.key.code)
			{
			case sf::Keyboard::Escape:
				_window->close();
				break;
			}
		}

		if (event.type == sf::Event::KeyPressed)
			switch (event.key.code)
			{
			case sf::Keyboard::W:
				_world->move_source(DIRS[N], elapsed);
				break;
			case sf::Keyboard::A:
				_world->move_source(DIRS[W], elapsed);
				break;
			case sf::Keyboard::S:
				_world->move_source(DIRS[S], elapsed);
				break;
			case sf::Keyboard::D:
				_world->move_source(DIRS[E], elapsed);
				break;

			case sf::Keyboard::C:
				_world->clear();
				break;
			case sf::Keyboard::Q:
				_trace_around ^= true;
				break;
			/*case sf::Keyboard::Space:
				_trace_light ^= true;
				break;*/
			}

		if (event.type == sf::Event::MouseButtonReleased)
		{
			vec2u mouse;
			switch (event.mouseButton.button)
			{
			case sf::Mouse::Left:
				mouse = vec2u(sf::Mouse::getPosition(*_window)) / _tile_dim;
				if (mouse.x < 2 || mouse.y < 2 || mouse.x > _world->width() - 3 || mouse.y > _world->height() - 3)
					continue;
				_world->toggle_tile(mouse.x, mouse.y);
				break;
			case sf::Mouse::Right:
				_trace_mouse ^= true;
				break;
			}
		}
	}
}

void application::render()
{
	_window->clear();

	for (int x = 0; x < _world->width(); x++)
		for (int y = 0; y < _world->height(); y++)
			draw(x, y);

	auto corners = _world->corners();
	for (const auto& corner : corners)
	{
		_corner.setPosition(corner - get_rad_vec(_corner));
		_window->draw(_corner);
	}

	auto src = _world->get_source_pos();

	if (_trace_mouse)
	{
		auto mouse = vec2f(sf::Mouse::getPosition(*_window));
		auto dest = _world->ray_cast_dda(mouse - src);

		sf::VertexArray line(sf::Lines);
		line.append(sf::Vertex(src, sf::Color::Yellow));
		line.append(sf::Vertex(dest, sf::Color::Yellow));
		_window->draw(line);

		/*sf::CircleShape field(len(dest - src), 100);
		field.setPosition(src - get_rad_vec(field));
		field.setFillColor(sf::Color::Transparent);
		field.setOutlineColor(sf::Color(255, 128, 0));
		field.setOutlineThickness(-4.f);
		_window->draw(field);*/

		_inter.setPosition(dest - get_rad_vec(_inter));
		_window->draw(_inter);
	}

	if (_trace_around)
	{
		for (const auto& dir : DIRS)
		{
			auto dest = _world->ray_cast_dda(dir);

			sf::VertexArray line(sf::Lines);
			line.append(sf::Vertex(src, sf::Color::Red));
			line.append(sf::Vertex(dest, sf::Color::Red));
			_window->draw(line);

			_inter.setPosition(dest - get_rad_vec(_inter));
			_window->draw(_inter);
		}
	}

	/*if (_trace_light)
	{

	}*/

	_source.setPosition(src - get_rad_vec(_source));
	_window->draw(_source);

	_window->display();
}

application::application(world* world, const std::string& title)
	: _world(world), _title(title)
{
	_tile_dim = _world->dim();
	_tile = sf::RectangleShape(vec2f(_tile_dim, _tile_dim));
	_tile.setOutlineThickness(-2.f);

	auto rad = _world->get_source_rad();
	_source = sf::CircleShape(rad, 6);
	_source.setFillColor(sf::Color::Transparent);
	_source.setOutlineColor(sf::Color::Magenta);
	_source.setOutlineThickness(-2.f);

	_inter = sf::CircleShape(rad / 2.f);
	_inter.setFillColor(sf::Color::Transparent);
	_inter.setOutlineColor(sf::Color::Green);
	_inter.setOutlineThickness(-2.f);

	_corner = sf::CircleShape(rad / 4.f);
	_corner.setFillColor(sf::Color::White);
	/*_corner.setOutlineColor(sf::Color::Cyan);
	_corner.setOutlineThickness(-2.f);*/

	_window = new sf::RenderWindow(sf::VideoMode(_world->width() * _tile_dim, _world->height() * _tile_dim), _title);
	_window->setVerticalSyncEnabled(true);
	//_window->setFramerateLimit(120);
}

application::~application()
{
	delete _window;
}

void application::run()
{
	std::chrono::system_clock clock;
	uint64_t elapsed = 0;

	while (_window->isOpen())
	{
		auto then = clock.now();

		handle_events(elapsed / 1000.f);
		render();

		elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(clock.now() - then).count();
		uint64_t fps = 1000 / elapsed;
		std::string log = "[FPS: " + std::to_string(fps) + "]";
		_window->setTitle(_title + " " + log);
	}
}
