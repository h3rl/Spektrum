#include "Window.h"


Window::Window() :
m_viewAxis(WindowAxis::Mathematical),
m_viewOrigin(WindowOrigin::BottomLeft)
{

}

bool Window::init()
{
	sf::ContextSettings settings;
	settings.antialiasingLevel = config::graphics::antialiasing_level;

	this->create(sf::VideoMode(config::window_width, config::window_height), "App", sf::Style::Default, settings);

	if (config::graphics::vsync)
	{
		this->setVerticalSyncEnabled(true);
	}
	else
	{
		this->setVerticalSyncEnabled(false);
		this->setFramerateLimit(config::graphics::framerate_limit);
	}

	_D("window set");

	this->updateSize();

	return true;
}

sf::View Window::getSizedView()
{
	sf::View view = this->getDefaultView();
	sf::Vector2f wSize = (sf::Vector2f)this->getSize();

	view.setSize(wSize);
	if (m_viewOrigin == Center)
	{
		view.setCenter(0,0);
	}
	else
	{
		view.setCenter(wSize.x / 2.f, wSize.y / 2.f);
	}

	return view;
}

void Window::updateSize()
{

	sf::View view = this->getDefaultView();

	sf::Vector2u wSize = this->getSize();
	const float wWidth = wSize.x;
	const float wHeight = wSize.y;

	// update config
	config::window_height = wHeight;
	config::window_width = wWidth;

	// Correct axis
	switch (this->m_viewAxis)
	{
	case Mathematical:
		view.setSize(wWidth, -wHeight);
		break;
	case Computer:
		view.setSize(wWidth, wHeight);
		break;
	default:
		break;
	}

	// Correct origin
	sf::Vector2f center{ wWidth / 2.f,wHeight / 2.f };
	switch (this->m_viewOrigin)
	{
	case Center:
		center = { 0,0 };
		break;
	case TopLeft:
		center.y *= -1.f;
		break;
	case BottomRight:
		center.x *= -1.f;
		break;
	case TopRight:
		center.x *= -1.f;
		center.y *= -1.f;
		break;
	case BottomLeft:
		break;
	default:
		break;
	}
	view.setCenter(center);

	this->setView(view);
}

sf::Vector2f Window::getCenter()
{
	return this->getSizef() / 2.f;
}

sf::Vector2f Window::getSizef()
{
	return (sf::Vector2f)this->getSize();
}
