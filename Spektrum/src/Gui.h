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
	bool					init();

private:

	void					renderFpsText();	

	void					renderMenuV1();

	void					renderTabMain();
	void					renderTabConfig();
	void					renderTabGraphics();
	void					renderTabDebug();
private:
	sf::Font							m_font;
	sf::Clock							clock;

	sf::Text							m_fpsText;
};

extern Gui g_gui;