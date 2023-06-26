#include "Scene.h"
#include "AudioSink.h"

#include <numeric> // std::accumulate

Scene::Scene() :
	m_pData(nullptr)
{
}

bool Scene::init()
{
	this->m_pData = g_audiosink.Output;

	if (!this->m_pData)
	{
		return false;
	}

	buildScene();

	return true;
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
	const float wheight = g_window.getSize().y / 2.f;
	const float wwidth = g_window.getSize().x / 2.f;

	// make sure its emtpy
	rects.clear();

	const int rectsCount = config.audio.bar_count;

	for (int i = 0; i < rectsCount; i++)
	{
		const float fraction = (float)i / (float)rectsCount;

		const float xpos = g_window.getSize().x * fraction;
		const float ypos = 0;// g_window.getSize().y * 1 / 8;

		const sf::Color color = interpolateColor(sf::Color::Red, sf::Color::Yellow, fraction);

		sf::RectangleShape r;
		r.setFillColor(color);
		r.setPosition(sf::Vector2f(xpos, ypos));
		rects.push_back(r);
	}

	const float r = 200.f;

	radial_gradient_rect.setPosition(0, 0);
	radial_gradient_rect.setSize(g_window.getSizef());

	radial_gradient_shader.loadFromFile("Assets/radial_gradient.frag", sf::Shader::Fragment);
	radial_gradient_shader.setUniform("windowsize", g_window.getSizef());
	radial_gradient_shader.setUniform("center_color", sf::Vector3f(0, 0, 0));
	radial_gradient_shader.setUniform("outer_color", sf::Vector3f(1, 0, 0));

}

//TODO:finish
static float bass_amount_last = 1.f;
static boost::circular_buffer<float> signalenergies(32); // store energy for 64 samples

const bool drawcircular = true;

void Scene::update(const sf::Time& dtTime)
{
	if (state.window_needs_redraw)
	{
		buildScene();
		state.window_needs_redraw = false;
	}

	sf::Vector2f window_size = g_window.getSizef();

	const int rectsCount = rects.size();

	// Gradient settings
	const int bass_start = (int)(config.audio.min_freq / g_audiosink.getFreqPerSample()); // this is clamped to [0,inf>
	const int bass_end = std::min((int)(config.audio.max_freq / g_audiosink.getFreqPerSample()), rectsCount); // this is clamped to <bass_start, inf> so clamp to rectcount aswell.
	const int bass_range = bass_end - bass_start;

	if (drawcircular)
	{
		for (int i = 0; i < rectsCount; i++)
		{
			const float& freq_gain = g_audiosink.Output[i];
			sf::RectangleShape& r = rects[i];

			const float& bar_gain = config.audio.bar_gain;
			const float barheight = freq_gain * 255.f;
			const float delta_deg = 360.f / rectsCount;
			const float degs = delta_deg * i;
			const float rads = PIf / 180.f * degs;

			const float radius = 75.f;
			const float x = window_size.x / 2.f + radius * cos(rads);
			const float y = window_size.y / 2.f + radius * sin(rads);

			const float circumference = 2.f * PIf * radius;
			const float barwidth = 2.f* circumference/ (float)rectsCount;

			r.setSize(sf::Vector2f(barwidth, barheight));
			r.setPosition(x, y);
			r.setRotation(degs-90.f);

			const float fraction = (float)i / (float)rectsCount;
			const sf::Color color = interpolateColor(sf::Color::Red, sf::Color::Yellow, fraction);
			r.setFillColor(color);

			//if (bass_start <= i && i <= bass_end)
			//{
			//	r.setFillColor(sf::Color::Blue);
			//}
			//else
			//{
			//	const sf::Color color = interpolateColor(sf::Color::Red, sf::Color::Yellow, fraction);
			//	r.setFillColor(color);
			//}
		}
	}
	else
	{
		const float bar_maxheight = window_size.y * 7.f / 8.f;
		const float bar_maxwidth = window_size.x / rects.size();

		for (int i = 0; i < rectsCount; i++)
		{
			const float& freq_gain = g_audiosink.Output[i];
			sf::RectangleShape& r = rects[i];

			const float& bar_gain = config.audio.bar_gain;
			const float barheight = freq_gain * bar_maxheight * bar_gain;
			r.setSize(sf::Vector2f(bar_maxwidth, barheight));

			const float fraction = (float)i / (float)rectsCount;

			if (bass_start <= i && i <= bass_end)
			{
				r.setFillColor(sf::Color::Blue);
			}
			else
			{
				const sf::Color color = interpolateColor(sf::Color::Red, sf::Color::Yellow, fraction);
				r.setFillColor(color);
			}
		}
	}
	// calculate energy of signal
	float energy = 0;
	for (int i = bass_start; i < bass_end; i++)
	{
		const float& freq_output = g_audiosink.Output[i];
		energy += freq_output;
	}
	energy /= bass_range;
	signalenergies.push_back(energy);

	// calculate average
	float average = std::accumulate(signalenergies.begin(), signalenergies.end(), 0.0f) / signalenergies.size();

	// calculate variance
	float variance = 0;
	for (int i = 0; i < signalenergies.size(); i++)
	{
		const float& energy = signalenergies[i];
		variance += std::powf((energy - average),2);
	}
	variance /= signalenergies.size();

	float& coeff_a = config.audio.bass_threshold_a;
	float& coeff_b = config.audio.bass_threshold_b;

	float threshold = coeff_a * variance + coeff_b;
	float energythreshold = threshold * average;


	if (energy > energythreshold)
	{
		// its a kick
		gradient_strength = 1.f;
	}
	else {
		gradient_strength *= 0.997f;
	}


	state.debug_textvec.push_back(std::format("average energy: {:4f}", average));
	state.debug_textvec.push_back(std::format("variance energy: {:4f}", variance));
	state.debug_textvec.push_back(std::format("current energy: {:4f}", energy));
	state.debug_textvec.push_back(std::format("ethreshold = a*var + b: {:4f}", energythreshold));
	state.debug_textvec.push_back(std::format("energy-ethreshold: {:4f}", (float)(energy - energythreshold)));
	state.debug_textvec.push_back(std::format("a*var: {:4f}", (float)(coeff_a * variance)));


	radial_gradient_shader.setUniform("strength", gradient_strength * 0.00075f);

	radial_gradient_rect.setPosition(0, 0);
	radial_gradient_rect.setSize(g_window.getSizef());
}

//TODO:finish
void Scene::render()
{
	// Draw the gradient quad
	g_window.draw(radial_gradient_rect, &radial_gradient_shader);

	for (const auto& r : rects)
	{
		g_window.draw(r);
	}
}

Scene g_scene;