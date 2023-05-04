#pragma once
#include "stdafx.h"

#include <vector>

#include "Config.h"
#include "AudioSink.h"

class Scene : private sf::NonCopyable
{
public:
							Scene();
	void					init(sf::RenderWindow& window, AudioSink& soundhandler);
	void					init(sf::RenderWindow& window, AudioSink& soundhandler, sf::RenderStates& state);
	void					update(sf::Time dtTime);
	void					render(sf::RenderStates rs);

private:

	void					buildScene();

	std::vector<sf::RectangleShape>	rects;

private:
	Config*					config;
	sf::RenderWindow*		window;
	sf::RenderStates*		renderStates;
	sf::Transform			transform;

	AudioSink*				audiosink;
};