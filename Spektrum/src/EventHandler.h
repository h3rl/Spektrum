#pragma once
#include "stdafx.h"

class EventHandler
{
public:
	struct Mouse {
		struct Wheel
		{
			float delta;
			float position;
		} wheel;
		sf::Vector2f pos;
		sf::Vector2f delta;
	} mouse;

	std::unordered_map<sf::Mouse::Button, bool> mo_pressed;
	std::unordered_map<sf::Keyboard::Key, bool> kb_pressed;

public:
	EventHandler();

	void handleEvents();

	void init(shared_ptr<sf::RenderWindow> window);

	bool pressed(sf::Keyboard::Key key);
	bool pressed(sf::Mouse::Button button);

private:

	void setpressed(sf::Keyboard::Key key, bool value);
	void setpressed(sf::Mouse::Button button, bool value);

	void reset();

	bool firstTimeDelta;
	shared_ptr<sf::RenderWindow> m_window;
};
