#pragma once
#include "stdafx.h"


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

class Window : public sf::RenderWindow
{
public:
							Window();

	bool					init();
	sf::View				getSizedView();
	void					updateSize();
	sf::Vector2f			getCenter();
	sf::Vector2f			getSizef();
private:
	WindowOrigin			m_viewOrigin;
	WindowAxis				m_viewAxis;
};

extern Window g_window;