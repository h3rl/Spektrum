#include "scene_histogram.h"
#include "window.h"
#include "color.h"
#include "audio_sink.h"
#include "dft.h"
#include "logging.h"

namespace SceneHistogram
{
unsigned int dft_size = 1 << 12;
unsigned int bar_count = 1 << 11;

DFT dft;
std::vector<sf::RectangleShape> bars;

void init()
{
	dft.create(dft_size, AudioSink::buffer);
	rebuild();
}

void rebuild()
{
	bars.clear();
	bars.reserve(bar_count);

	sf::Color start_color = sf::Color::Red;
	sf::Color end_color = sf::Color::Yellow;

	const double bar_width = (double)Window::width / (double)bar_count;

	for (unsigned int i = 0; i < bar_count; i++)
	{
		double posx = bar_width * i;

		double factor = (double)i / (double)bar_count;

		sf::RectangleShape bar;
		bar.setSize(sf::Vector2f(bar_width, -7.5 * (double)(i + 6)));
		bar.setPosition(sf::Vector2f(Window::width * factor, Window::height));
		bar.setFillColor(interpolateColor(start_color, end_color, factor));
		bars.push_back(bar);
	}
}

void update(const sf::Time &dtTime)
{
	dft.execute();
	const double *output = dft.getOutput();

	for (unsigned int i = 0; i < bar_count; i++)
	{
		auto &bar = bars[i];
		const double &freq_gain = output[i];
		const double bar_height = freq_gain * Window::height;
		bar.setSize(sf::Vector2f(bar.getSize().x, -bar_height));
	}
}

void render()
{
	for (const auto &bar : bars)
	{
		p_window->draw(bar);
	}
}
}; // namespace SceneHistogram