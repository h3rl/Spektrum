#pragma once
#include "stdafx.h"

#include <vector>

#include "Config.h"
#include "AudioSink.h"

class Scene : private sf::NonCopyable
{
public:
							Scene();
	void					init(shared_ptr<sf::RenderWindow> window, shared_ptr<AudioSink> audiosink);
	void					update(sf::Time dtTime);
	void					render();

private:

	void					buildScene();

	std::vector<sf::RectangleShape>	rects;

private:
	shared_ptr<sf::RenderWindow>	m_window;
	shared_ptr<AudioSink>			m_audiosink;
};