#include "scene_histogram.h"
#include "window.h"
#include "color.h"
#include "beat_detector.h"
#include "dft.h"
#include "logging.h"

namespace SceneHistogram
{
unsigned int dft_size = 1 << 12;
unsigned int bar_count = 1 << 11;

std::vector<sf::RectangleShape> bars;

sf::RectangleShape radial_gradient_rect;
sf::Shader radial_gradient_shader;
float gradient_strength = 1;

void build()
{
	bars.clear();
	bars.reserve(bar_count);

	sf::Color start_color = sf::Color::Red;
	sf::Color end_color = sf::Color::Yellow;

	const float bar_width = (float)Window::width / (float)bar_count;

	for (unsigned int i = 0; i < bar_count; i++)
	{
		float posx = bar_width * i;

		float factor = (float)i / (float)bar_count;

		sf::RectangleShape bar;
		bar.setSize(sf::Vector2f(bar_width, -7.5 * (float)(i + 6)));
		bar.setPosition(sf::Vector2f(Window::width * factor, Window::height));
		bar.setFillColor(interpolateColor(start_color, end_color, factor));
		bars.push_back(bar);
	}

	radial_gradient_rect.setPosition(0, 0);
	radial_gradient_rect.setSize((sf::Vector2f)p_window->getSize());

	radial_gradient_shader.loadFromFile("assets/radial_gradient.frag", sf::Shader::Fragment);
	radial_gradient_shader.setUniform("windowsize", (sf::Vector2f)p_window->getSize());
	radial_gradient_shader.setUniform("center_color", sf::Vector3f(0, 0, 0));
	radial_gradient_shader.setUniform("outer_color", sf::Vector3f(1, 0, 0));
}

void update()
{
	float *output = nullptr;
	unsigned int output_size = 0;
	BeatDetector::get_dft_data(&output, &output_size);

	if (output_size != bar_count)
	{
		bar_count = output_size;
		build();
	}

	for (unsigned int i = 0; i < bar_count; i++)
	{
		auto &bar = bars[i];
		const float &freq_gain = output[i];
		const float bar_height = freq_gain * Window::height;
		bar.setSize(sf::Vector2f(bar.getSize().x, -bar_height));
	}
	gradient_strength = BeatDetector::loudness * 500;
	std::stringstream ss;
	ss << "loudness " << std::fixed << std::setprecision(10) << gradient_strength;
	debug_vec.push_back(ss.str());
	radial_gradient_shader.setUniform("strength", gradient_strength * 0.00075f);
}

void render()
{
	p_window->draw(radial_gradient_rect, &radial_gradient_shader);

	for (const auto &bar : bars)
	{
		p_window->draw(bar);
	}
}
}; // namespace SceneHistogram