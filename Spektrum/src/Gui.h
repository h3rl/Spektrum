#pragma once
#include "stdafx.h"

#include "Config.h"

class Gui : private sf::NonCopyable
{
public:
	Gui();
	~Gui();

	void					update(sf::Time dtTime);
	void					render(sf::RenderStates& rs);
	void					init(sf::RenderWindow& window);

private:

	void					renderConfigWindow();
	void					renderTestWindow();
private:

	sf::RenderWindow*		window;
	sf::Font				font;
	sf::Clock				clock;

	sf::Text				fpsText;
};