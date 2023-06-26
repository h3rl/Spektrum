#pragma once
#include "stdafx.h"

#include <vector>

#include "AudioSink.h"

class Scene : private sf::NonCopyable
{
public:
							Scene();
	bool					init();
	void					update(const sf::Time& dtTime);
	void					render();

	void					buildScene();

private:
	std::vector<sf::RectangleShape>	rects;
	sf::RectangleShape				radial_gradient_rect;
	sf::Shader						radial_gradient_shader;
	float							gradient_strength = 1.f;

	float*							m_pData;
};

extern Scene g_scene;