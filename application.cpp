#include "application.h"

void application::draw(uint32_t x, uint32_t y)
{
	_tile.setPosition(vec2f(x * _tile_dim, y * _tile_dim));
	_tile.setFillColor(_world->tile_solid(x, y) ? sf::Color::Blue : sf::Color::Black);
	_window->draw(_tile);
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
				_world->move_source(vec2f(0.f, -1.f), elapsed);
				break;
			case sf::Keyboard::A:
				_world->move_source(vec2f(-1.f, 0.f), elapsed);
				break;
			case sf::Keyboard::S:
				_world->move_source(vec2f(0.f, 1.f), elapsed);
				break;
			case sf::Keyboard::D:
				_world->move_source(vec2f(1.f, 0.f), elapsed);
				break;
			}

		if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Right)
		{
			auto mouse = vec2u(sf::Mouse::getPosition(*_window)) / _tile_dim;
			if (mouse.x < 1 || mouse.y < 1 || mouse.x > _world->width() - 2 || mouse.y > _world->height() - 2)
				continue;
			_world->toggle_tile(mouse.x, mouse.y);
		}

		if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left)
		{
			auto mouse = vec2f(sf::Mouse::getPosition(*_window));
			_dest.setPosition(_world->ray_cast_dda(mouse));
		}
	}
}

void application::update()
{
}

void application::render()
{
	_window->clear();

	for (int x = 0; x < _world->width(); x++)
		for (int y = 0; y < _world->height(); y++)
			draw(x, y);

	_window->draw(_world->get_map());

	_source.setPosition(_world->get_source_pos());
	_window->draw(_source);

	_window->draw(_dest);

	_window->display();
}

application::application(world* world, const std::string& title)
	: _world(world), _title(title)
{
	_tile_dim = _world->dim();
	_tile = sf::RectangleShape(vec2f(_tile_dim, _tile_dim));

	_source = sf::CircleShape(_world->get_source_rad(), 6);
	_source.setFillColor(sf::Color::Magenta);

	_dest = sf::CircleShape(_world->get_source_rad(), 3);
	_dest.setFillColor(sf::Color::Cyan);

	_inter = sf::CircleShape(_world->get_source_rad());
	_inter.setFillColor(sf::Color::Transparent);
	_inter.setOutlineColor(sf::Color::Yellow);
	_inter.setOutlineThickness(2.f);

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
		//upd
		render();

		elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(clock.now() - then).count();
		uint64_t fps = 1000 / elapsed;
		std::string log = "[FPS: " + std::to_string(fps) + "]";
		_window->setTitle(_title + " " + log);
	}
}
