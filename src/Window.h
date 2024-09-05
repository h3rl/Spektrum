#pragma once
#include "stdafx.h"
#include <SFML/Graphics/RenderWindow.hpp>

enum WindowOrigin : int
{
	Center,
	TopLeft,
	TopRight,
	BottomLeft,
	BottomRight
};

enum WindowAxis : int
{
	Mathematical, // x left, y up
	Computer // x right, y down
};

extern sf::RenderWindow *p_window;
namespace Window
{
extern WindowOrigin viewOrigin;
extern WindowAxis viewAxis;
extern unsigned int height;
extern unsigned int width;
extern unsigned int antialiasing_level;
extern unsigned int framerate_limit;
extern bool vsync;
extern bool needs_redraw;

void init();
sf::View getSizedView();
void updateSize();
sf::Vector2f getCenter();
sf::Vector2f getSizef();

}; // namespace Window