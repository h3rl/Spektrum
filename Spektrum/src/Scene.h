#pragma once
#include "stdafx.h"

#include <vector>

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
	sf::RectangleShape				radial_gradient_rect;
	sf::Shader						radial_gradient_shader;
	float							gradient_strength = 1.f;

	float*							m_pData;

private:
	shared_ptr<Window>				m_window;
	shared_ptr<AudioSink>			m_audiosink;
};