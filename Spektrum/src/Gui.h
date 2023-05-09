#pragma once
#include "stdafx.h"

class Gui : private sf::NonCopyable
{
public:
							Gui();
							~Gui();

	void					update(sf::Time dtTime);
	void					render();
	bool					init(shared_ptr<Window> window);

private:

	void					renderFpsText();	

	void					renderMenuV1();

	void					renderConfigWindow();
	void					renderTestWindow();
private:

	shared_ptr<Window>		m_window;
	sf::Font							m_font;
	sf::Clock							clock;

	sf::Text							m_fpsText;
};