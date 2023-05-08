#pragma once
#include "stdafx.h"

#include <vector>

#include "Config.h"
#include "AudioSink.h"

class Scene : private sf::NonCopyable
{
public:
							Scene();
	void					init(shared_ptr<Window> window, shared_ptr<AudioSink> audiosink);
	void					update(sf::Time dtTime);
	void					render();

	void					buildScene();

private:
	std::vector<sf::RectangleShape>	rects;

private:
	shared_ptr<Window>	m_window;
	shared_ptr<AudioSink>			m_audiosink;
};