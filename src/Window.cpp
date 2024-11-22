#include "Window.h"
#include "logging.h"

sf::RenderWindow window;
sf::RenderWindow *p_window = &window;

namespace Window
{

// Globals
WindowOrigin viewOrigin = WindowOrigin::BottomLeft;
WindowAxis viewAxis = WindowAxis::Mathematical;
unsigned int height = 900;
unsigned int width = 1600;
unsigned int antialiasing_level = 8;
unsigned int framerate_limit = 0;
bool vsync = false;
bool needs_redraw = false;

void init()
{
	sf::ContextSettings settings;
	settings.antialiasingLevel = antialiasing_level;

	window.create(sf::VideoMode(width, height), "App", sf::Style::Default, settings);

	if (vsync)
	{
		window.setVerticalSyncEnabled(true);
	}
	else
	{
		window.setVerticalSyncEnabled(false);
		window.setFramerateLimit(framerate_limit);
	}

	log_msg_ln("window set");

	// updateSize();
}

void update()
{
	// update framelimit, vsync and aliasinglevel
}

// sf::View getSizedView()
// {
// 	sf::View view = window.getDefaultView();
// 	sf::Vector2f wSize = getSizef();

// 	view.setSize(wSize);
// 	if (viewOrigin == Center)
// 	{
// 		view.setCenter(0, 0);
// 	}
// 	else
// 	{
// 		view.setCenter(wSize.x / 2.f, wSize.y / 2.f);
// 	}

// 	return view;
// }

// void updateSize()
// {

// 	sf::View view = window.getDefaultView();

// 	sf::Vector2u wSize = window.getSize();
// 	height = wSize.x;
// 	width = wSize.y;

// 	// Correct axis
// 	switch (viewAxis)
// 	{
// 	case Mathematical:
// 		view.setSize(width, -height);
// 		break;
// 	case Computer:
// 		view.setSize(width, height);
// 		break;
// 	default:
// 		break;
// 	}

// 	// Correct origin
// 	sf::Vector2f center{ (float)width / 2.0f, (float)height / 2.0f };
// 	switch (viewOrigin)
// 	{
// 	case Center:
// 		center = { 0, 0 };
// 		break;
// 	case TopLeft:
// 		center.y *= -1.f;
// 		break;
// 	case BottomRight:
// 		center.x *= -1.f;
// 		break;
// 	case TopRight:
// 		center.x *= -1.f;
// 		center.y *= -1.f;
// 		break;
// 	case BottomLeft:
// 		break;
// 	default:
// 		break;
// 	}
// 	view.setCenter(center);
// 	window.setView(view);
// }

// sf::Vector2f getCenter()
// {
// 	return getSizef() / 2.f;
// }

// sf::Vector2f getSizef()
// {
// 	return (sf::Vector2f)window.getSize();
// }

}; // namespace Window