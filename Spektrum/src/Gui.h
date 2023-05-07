#pragma once
#include "stdafx.h"

#include "Config.h"

class Gui : private sf::NonCopyable
{
public:
							Gui();
							~Gui();

	void					update(sf::Time dtTime);
	void					render();
	bool					init(shared_ptr<sf::RenderWindow> window);

private:

	void					renderConfigWindow();
	void					renderTestWindow();
private:

	shared_ptr<sf::RenderWindow>		m_window;
	sf::Font							m_font;
	sf::Clock							clock;

	sf::Text							m_fpsText;
};