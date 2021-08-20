#include "application.h"

vec2f application::get_rad_vec(const sf::CircleShape& circle) const
{
	auto rad = circle.getRadius();
	return vec2f(rad, rad);
}

//drawing help functions
void application::draw_tile(uint32_t x, uint32_t y)
{
	_tile.setPosition(vec2f(x * _tile_dim, y * _tile_dim));

	if (_mode == fov && !_world->is_visible(x, y))
		return;

	if (_world->is_solid(x, y))
	{
		_tile.setFillColor(sf::Color::Blue);
		_tile.setOutlineColor(sf::Color::Cyan);
	}
	else
	{
		_tile.setFillColor({ 0x01, 0x23, 0x45 });
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
	_source.setPosition(_src->get_pos() - get_rad_vec(_source));
	_window->draw(_source);
}

void application::draw_field(float rad, const sf::Color& color)
{
	_field.setRadius(rad);
	_field.setPosition(_src->get_pos() - get_rad_vec(_field));
	_field.setOutlineColor(color);
	_window->draw(_field);
}

void application::draw_line(const vec2f& a, const vec2f& b, const sf::Color& color)
{
	sf::VertexArray line(sf::Lines);
	line.append(sf::Vertex(a, color));
	line.append(sf::Vertex(b, color));
	_window->draw(line);
}

void application::draw_fan(const vec2f& center, const std::vector<vec2f>& points, const sf::Color& color, bool connect)
{
	sf::VertexArray fan(sf::TriangleFan);
	fan.append(sf::Vertex(center, color));
	for (const auto& point : points)
		fan.append(sf::Vertex(point, color));
	if (connect)
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

//rendering help functions
void application::render_layout()
{
	for (int x = 0; x < _world->width(); x++)
		for (int y = 0; y < _world->height(); y++)
			draw_tile(x, y);
}

void application::render_shadows(bool rays, bool connect, const sf::Color& color)
{
	const auto& data = std::get<0>(_data);
	const auto& points = data.first;
	auto src = _src->get_pos();

	if (!rays)
		draw_fan(src, points, color, connect);
	else
	{
		draw_star(src, points, color);
		for (const auto& point : points)
			draw_intersect(point, color);
	}

	_rays_cast = data.second;
	_rays_drawn = points.size();
}

void application::trace_closest_collision()
{
	const auto& data = std::get<1>(_data);
	auto closest = data.first;

	sf::Color color = sf::Color::Green;
	draw_line(_src->get_pos(), closest, color);
	draw_intersect(closest, color);
	draw_field(len(_src->get_pos() - closest), color);

	_rays_cast = data.second;
	_rays_drawn = 1;
}

void application::trace_mouse()
{
	auto mouse = vec2f(sf::Mouse::getPosition(*_window));
	auto src = _src->get_pos();
	auto end = _src->ray_cast_dda(mouse - src);
	sf::Color color = sf::Color::Red;

	draw_line(src, end, color);
	draw_intersect(end, color);

	_rays_cast++;
	_rays_drawn++;
}

void application::show_corners()
{
	auto corners = _world->get_corners();
	for (const auto& corner : corners)
		draw_corner(corner);
}

void application::toggle_mode(mode mode)
{
	_mode = _mode == mode ? none : mode;
}

//main loop functions
void application::handle_events()
{
	sf::Event event;
	while (_window->pollEvent(event))
	{
		if (event.type == sf::Event::Closed)
			_window->close();

		if (event.type == sf::Event::KeyReleased)
		{
			auto key = event.key.code;

			switch (key)
			{
			case sf::Keyboard::C:
				_world->clear();
				break;
			case sf::Keyboard::Q:
				toggle_mode(around);
				break;
			case sf::Keyboard::E:
				_show_corners ^= true;
				break;
			case sf::Keyboard::R:
				toggle_mode(light_rays);
				break;
			case sf::Keyboard::F:
				toggle_mode(light);
				break;
			case sf::Keyboard::T:
				toggle_mode(fov_rays);
				break;
			case sf::Keyboard::G:
				toggle_mode(fov);
				break;

			case sf::Keyboard::Up:
				_src->mod_range(INFINITY);
				break;
			case sf::Keyboard::Down:
				_src->mod_range(-INFINITY);
				break;

			case sf::Keyboard::Escape:
				_window->close();
				break;
			}

			if (key > sf::Keyboard::Num0 && key <= sf::Keyboard::Num9)
				_src->set_precision(key - sf::Keyboard::Num0);
			else if (key == sf::Keyboard::Num0)
				_src->set_precision(10);
		}

		if (event.type == sf::Event::KeyPressed)
			switch (event.key.code)
			{
			case sf::Keyboard::W:
				_src->move(DIRS[N], _elapsed);
				break;
			case sf::Keyboard::A:
				_src->move(DIRS[W], _elapsed);
				break;
			case sf::Keyboard::S:
				_src->move(DIRS[S], _elapsed);
				break;
			case sf::Keyboard::D:
				_src->move(DIRS[E], _elapsed);
				break;

			case sf::Keyboard::Left:
				_src->mod_range(-(float)_tile_dim);
				break;
			case sf::Keyboard::Right:
				_src->mod_range(_tile_dim);
				break;
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
				_world->toggle_solid(mouse.x, mouse.y);
				break;
			case sf::Mouse::Right:
				_trace_mouse ^= true;
				break;

			case sf::Mouse::XButton1:
				_src->mod_fov(-INFINITY);
				break;
			case sf::Mouse::XButton2:
				_src->mod_fov(INFINITY);
				break;
			}
		}

		if (event.type == sf::Event::MouseWheelScrolled)
		{
			_src->mod_fov(event.mouseWheelScroll.delta < 0.f ? -ALPHA : ALPHA);
		}
	}
}

void application::update()
{
	switch (_mode)
	{
	case light:
	case light_rays:
		_src->mod_range(INFINITY);
		_data = _src->line_of_sight();
		break;
	case fov:
	case fov_rays:
		_world->reset_visible();
		_data = _src->field_of_view(vec2f(sf::Mouse::getPosition(*_window)) - _src->get_pos());
		break;
	case around:
		_src->mod_range(INFINITY);
		_data = _src->closest_collision();
		break;
	}
}

void application::render()
{
	_window->clear();

	render_layout();

	switch (_mode)
	{
	case light:
		render_shadows(false, true, { 255, 160, 0, 192 });
		break;
	case light_rays:
		render_shadows(true, true, { 255, 160, 0, 192 });
		break;
	case fov:
		render_shadows(false, false, { 128, 160, 255, 192 });
		break;
	case fov_rays:
		render_shadows(true, false, { 128, 160, 255, 192 });
		break;
	case around:
		trace_closest_collision();
		break;
	}

	if (_trace_mouse)
		trace_mouse();
	if (_show_corners)
		show_corners();
	draw_source();

	_window->display();
}

//public interface
application::application(tileworld* world, source* source, const std::string& title)
	: _world(world), _src(source), _title(title)
{
	_tile_dim = _world->dim();
	_tile = sf::RectangleShape(vec2f(_tile_dim, _tile_dim));
	_tile.setOutlineThickness(-2.f);

	_source = sf::CircleShape(_tile_dim / 4.f, 6);
	_source.setFillColor(sf::Color::Black);
	_source.setOutlineColor(sf::Color::White);
	_source.setOutlineThickness(_tile_dim / -12.f);

	_field.setFillColor(sf::Color::Transparent);
	_field.setOutlineThickness(-2.f);

	_intersect = sf::CircleShape(_tile_dim / 4.f);
	_intersect.setFillColor(sf::Color::Transparent);
	_intersect.setOutlineThickness(-2.f);

	_corner = sf::CircleShape(_tile_dim / 8.f);
	_corner.setFillColor(sf::Color::White);

	_window = new sf::RenderWindow(sf::VideoMode(_world->width() * _tile_dim, _world->height() * _tile_dim), _title);
	//_window->setVerticalSyncEnabled(true);
}

application::~application()
{
	delete _window;
}

void application::run()
{
	std::chrono::system_clock clock;

	while (_window->isOpen())
	{
		_rays_cast = _rays_drawn = 0;
		auto then = clock.now();

		handle_events();
		update();
		render();

		_elapsed = std::chrono::duration_cast<std::chrono::microseconds>(clock.now() - then).count() / 1000000.f;
		uint64_t fps = 1.f / _elapsed;
		std::string log = "[FPS: " + std::to_string(fps)
			+ "] [Rays cast: " + std::to_string(_rays_cast) + ", rays drawn: " + std::to_string(_rays_drawn)
			+ "] [FOV: " + std::to_string((int)(_src->get_fov() / ALPHA)) + " deg, range: " + std::to_string((int)_src->get_range()) + " px]";
		_window->setTitle(_title + " " + log);
	}
}
