#include "Scene.h"
#include "AudioSink.h"

Scene::Scene() :
	m_window(nullptr),
	m_audiosink(nullptr),
	m_pData(nullptr)
{
}

void Scene::init(shared_ptr<Window> window, shared_ptr<AudioSink> audiosink)
{
	this->m_window = window;
	this->m_audiosink = audiosink;
	this->m_pData = m_audiosink->Output;

	buildScene();
}

sf::Color interpolateColor(sf::Color color1, sf::Color color2, float factor)
{
	uint8_t r1 = color1.r;
	uint8_t g1 = color1.g;
	uint8_t b1 = color1.b;

	uint8_t r2 = color2.r;
	uint8_t g2 = color2.g;
	uint8_t b2 = color2.b;

	uint8_t r = (uint8_t)((r2 - r1) * factor + r1);
	uint8_t g = (uint8_t)((g2 - g1) * factor + g1);
	uint8_t b = (uint8_t)((b2 - b1) * factor + b1);
	return sf::Color(r, g, b);
}


void Scene::buildScene()
{
	const float wheight = m_window->getSize().y / 2.f;

	// make sure its emtpy
	rects.clear();

	const int rectsCount = config::audio::bar_count;

	for (int i = 0; i < rectsCount; i++)
	{
		const float fraction = (float)i / (float)rectsCount;

		const float xpos = m_window->getSize().x * fraction;
		const float ypos = 0;// m_window->getSize().y * 1 / 8;

		const sf::Color color = interpolateColor(sf::Color::Red, sf::Color::Yellow, fraction);

		sf::RectangleShape r;
		r.setFillColor(color);
		r.setPosition(sf::Vector2f(xpos, ypos));
		rects.push_back(r);
	}
}

//TODO:finish
void Scene::update(sf::Time dtTime)
{
	if (state::window_needs_redraw)
	{
		buildScene();
		state::window_needs_redraw = false;
	}

	sf::Vector2f windowSize = (sf::Vector2f)m_window->getSize();

	const float height = windowSize.y * 7.f / 8.f;
	const float width = windowSize.x / rects.size();

	const float gain = config::audio::bar_gain;

	for (int i = 0; i < rects.size(); i++)
	{
		sf::RectangleShape& r = rects[i];

		const float barheight = m_audiosink->Output[i]*height*gain;

		r.setSize(sf::Vector2f(width, barheight));
	}
}

//TODO:finish
void Scene::render()
{
	for (const auto& r : rects)
	{
		m_window->draw(r);
	}
}