#pragma once
#include "stdafx.h"

class Gui : private sf::NonCopyable
{
public:
	Gui();
	~Gui();
	void					preupdate();
	void					update(const sf::Time& dtTime);
	void					render();
	bool					init(shared_ptr<Window> window);

private:

	void					renderFpsText();	

	void					renderMenuV1();

	void					renderTabMain();
	void					renderTabConfig();
	void					renderTabGraphics();
	void					renderTabDebug();
private:

	shared_ptr<Window>		m_window;
	sf::Font							m_font;
	sf::Clock							clock;

	sf::Text							m_fpsText;
};