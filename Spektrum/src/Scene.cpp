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
	const float wwidth = m_window->getSize().x / 2.f;

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

	const float r = 200.f;

	radial_gradient_rect.setPosition(0, 0);
	radial_gradient_rect.setSize(m_window->getSizef());

	radial_gradient_shader.loadFromFile("Assets/radial_gradient.frag", sf::Shader::Fragment);
	radial_gradient_shader.setUniform("windowsize", m_window->getSizef());
	radial_gradient_shader.setUniform("center_color", sf::Vector3f(0, 0, 0));
	radial_gradient_shader.setUniform("outer_color", sf::Vector3f(1, 0, 0));

}

#include <numeric>

//TODO:finish
void Scene::update(sf::Time dtTime)
{
	if (state::window_needs_redraw)
	{
		buildScene();
		state::window_needs_redraw = false;
	}

	sf::Vector2f window_size = m_window->getSizef();

	const float bar_maxheight = window_size.y * 7.f / 8.f;
	const float bar_maxwidth = window_size.x / rects.size();


	// Gradient settings
	const float bass_start = 5;
	const float bass_range = 2;

	for (int i = 0; i < rects.size(); i++)
	{
		const float& freq_gain = m_audiosink->Output[i];
		sf::RectangleShape& r = rects[i];

		const float& bar_gain = config::audio::bar_gain;
		const float barheight = freq_gain * bar_maxheight * bar_gain;
		r.setSize(sf::Vector2f(bar_maxwidth, barheight));

		if (i >= bass_start && i <= bass_start + bass_range)
		{
			r.setFillColor(sf::Color::Blue);
		}
	}


	float newgradient = 0;
	for (int i = bass_start; i < bass_start+bass_range; i++)
	{
		const float strength = m_audiosink->Output[i];
		newgradient += strength;
	}
	newgradient /= bass_range;
	gradient_strength = newgradient;

	radial_gradient_shader.setUniform("strength", gradient_strength * 0.00075f);

	radial_gradient_rect.setPosition(0, 0);
	radial_gradient_rect.setSize(m_window->getSizef());
}

//TODO:finish
void Scene::render()
{
	// Draw the gradient quad
	m_window->draw(radial_gradient_rect, &radial_gradient_shader);

	for (const auto& r : rects)
	{
		m_window->draw(r);
	}
}