#include "application.h"

vec2f application::get_rad_vec(const sf::CircleShape& circle) const
{
	auto rad = circle.getRadius();
	return vec2f(rad, rad);
}

//drawing functions
void application::draw_tile(uint32_t x, uint32_t y)
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

void application::draw_intersect(const vec2f& pos, const sf::Color& color)
{
	_intersect.setOutlineColor(color);
	_intersect.setPosition(pos - get_rad_vec(_intersect));
	_window->draw(_intersect);
}

void application::draw_corner(const vec2f& pos)
{
	_corner.setPosition(pos - get_rad_vec(_corner));
	_window->draw(_corner);
}

void application::draw_source()
{
	_source.setRadius(_world->get_src_rad());
	_source.setPosition(_world->get_src_pos() - get_rad_vec(_source));
	_window->draw(_source);
}

void application::draw_line(const vec2f& a, const vec2f& b, const sf::Color& color)
{
	sf::VertexArray line(sf::Lines);
	line.append(sf::Vertex(a, color));
	line.append(sf::Vertex(b, color));
	_window->draw(line);
}

void application::draw_fan(const vec2f& center, const std::vector<vec2f>& points, const sf::Color& color)
{
	sf::VertexArray fan(sf::TriangleFan);
	fan.append(sf::Vertex(center, color));
	for (const auto& point : points)
		fan.append(sf::Vertex(point, color));
	fan.append(sf::Vertex(points[0], color));
	_window->draw(fan);
}

void application::draw_star(const vec2f& center, const std::vector<vec2f>& points, const sf::Color& color)
{
	sf::VertexArray star(sf::Lines);
	for (int i = 0; i < points.size(); i++)
	{
		star.append(sf::Vertex(center, color));
		star.append(sf::Vertex(points[i], color));
	}
	_window->draw(star);
}

//event handling
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
			case sf::Keyboard::C:
				_world->clear();
				break;
			case sf::Keyboard::Q:
				_trace_around ^= true;
				break;
			case sf::Keyboard::E:
				_show_corners ^= true;
				break;
			case sf::Keyboard::R:
				_trace_eye_rays ^= true;
				_trace_fov = false;
				break;
			case sf::Keyboard::F:
				_trace_fov ^= true;
				_trace_eye_rays = false;
				break;

			/*case sf::Keyboard::Up:
				_world->mod_src_rng(INFINITY);
				break;
			case sf::Keyboard::Down:
				_world->mod_src_rng(-INFINITY);
				break;
			case sf::Keyboard::End:
				_world->mod_src_fov(INFINITY);
				break;
			case sf::Keyboard::Home:
				_world->mod_src_fov(-INFINITY);
				break;
			case sf::Keyboard::Equal:
				_world->mod_src_rad(INFINITY);
				break;
			case sf::Keyboard::Hyphen:
				_world->mod_src_rad(-INFINITY);
				break;*/

			case sf::Keyboard::Escape:
				_window->close();
				break;
			}
		}

		if (event.type == sf::Event::KeyPressed)
			switch (event.key.code)
			{
			case sf::Keyboard::W:
				_world->move_src(DIRS[N], elapsed);
				break;
			case sf::Keyboard::A:
				_world->move_src(DIRS[W], elapsed);
				break;
			case sf::Keyboard::S:
				_world->move_src(DIRS[S], elapsed);
				break;
			case sf::Keyboard::D:
				_world->move_src(DIRS[E], elapsed);
				break;

			/*case sf::Keyboard::Left:
				_world->mod_src_rng(-(float)_tile_dim);
				break;
			case sf::Keyboard::Right:
				_world->mod_src_rng(_tile_dim);
				break;
			case sf::Keyboard::PageUp:
				_world->mod_src_fov(PI * 0.25f);
				break;
			case sf::Keyboard::PageDown:
				_world->mod_src_fov(-PI * 0.25f);
				break;
			case sf::Keyboard::LBracket:
				_world->mod_src_rad(-2.f);
				break;
			case sf::Keyboard::RBracket:
				_world->mod_src_rad(2.f);
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

//render
void application::render()
{
	_window->clear();

	for (int x = 0; x < _world->width(); x++)
		for (int y = 0; y < _world->height(); y++)
			draw_tile(x, y);

	auto src = _world->get_src_pos();

	if (_trace_fov || _trace_eye_rays)
	{
		auto points = _world->line_of_sight();
		if (_trace_fov)
			draw_fan(src, points, { 255, 160, 0, 192 });
		else
		{
			sf::Color color = sf::Color::Yellow;
			draw_star(src, points, color);
			for (const auto& point : points)
				draw_intersect(point, color);
		}

		_rays_cast += points.size();
	}

	if (_trace_mouse)
	{
		auto mouse = vec2f(sf::Mouse::getPosition(*_window));
		auto dest = _world->ray_cast_dda(mouse - src);
		sf::Color color = sf::Color::Red;

		draw_line(src, dest, color);
		draw_intersect(dest, color);

		/*sf::CircleShape field(len(dest - src), 100);
		field.setPosition(src - get_rad_vec(field));
		field.setFillColor(sf::Color::Transparent);
		field.setOutlineColor(sf::Color(255, 128, 0));
		field.setOutlineThickness(-4.f);
		_window->draw(field);*/

		_rays_cast++;
	}

	if (_trace_around)
	{
		for (const auto& dir : DIRS)
		{
			auto dest = _world->ray_cast_dda(dir);
			sf::Color color = sf::Color::Green;

			draw_line(src, dest, color);
			draw_intersect(dest, color);
		}

		_rays_cast += DIRS.size();
	}

	if (_show_corners)
	{
		auto corners = _world->get_corners();
		for (const auto& corner : corners)
			draw_corner(corner);
	}

	draw_source();

	_window->display();
}

//public interface
application::application(world* world, const std::string& title)
	: _world(world), _title(title)
{
	_tile_dim = _world->dim();
	_tile = sf::RectangleShape(vec2f(_tile_dim, _tile_dim));
	_tile.setOutlineThickness(-2.f);

	_source = sf::CircleShape(_world->get_src_rad(), 6);
	_source.setFillColor(sf::Color::Magenta);
	/*_source.setOutlineColor(sf::Color::Magenta);
	_source.setOutlineThickness(-2.f);*/

	_intersect = sf::CircleShape(_tile_dim / 4.f);
	_intersect.setFillColor(sf::Color::Transparent);
	_intersect.setOutlineThickness(-2.f);

	_corner = sf::CircleShape(_tile_dim / 8.f);
	_corner.setFillColor(sf::Color::White);

	_window = new sf::RenderWindow(sf::VideoMode(_world->width() * _tile_dim, _world->height() * _tile_dim), _title);
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
		_rays_cast = 0;
		auto then = clock.now();

		handle_events(elapsed / 1000.f);
		render();

		elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(clock.now() - then).count();
		uint64_t fps = 1000 / elapsed;
		std::string log = "[FPS: " + std::to_string(fps) + "] [Rays cast: " + std::to_string(_rays_cast) + "]";
		_window->setTitle(_title + " " + log);
	}
}
