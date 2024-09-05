#include "scene.h"
#include "window.h"

// #include <numeric> // std::accumulate

namespace Scene
{
unsigned int m_bar_count;
std::vector<sf::RectangleShape> rects;
sf::RectangleShape radial_gradient_rect;
sf::Shader radial_gradient_shader;
double gradient_strength = 1;

double *m_pData;

void init()
{
	buildScene();
}

void buildScene()
{
	const double wheight = Window::height / 2.f;
	const double wwidth = Window::width / 2.f;

	// make sure its emtpy
	rects.clear();

	// const int rectsCount = config.audio.bar_count;

	// for (int i = 0; i < rectsCount; i++)
	// {
	// 	const double fraction = (double)i / (double)rectsCount;

	// 	const double xpos = g_window.getSize().x * fraction;
	// 	const double ypos = 0; // g_window.getSize().y * 1 / 8;

	// 	const sf::Color color = interpolateColor(sf::Color::Red, sf::Color::Yellow, fraction);

	// 	sf::RectangleShape r;
	// 	r.setFillColor(color);
	// 	r.setPosition(sf::Vector2f(xpos, ypos));
	// 	rects.push_back(r);
	// }

	// const double r = 200.f;

	radial_gradient_rect.setPosition(0, 0);
	radial_gradient_rect.setSize(Window::getSizef());

	radial_gradient_shader.loadFromFile("assets/radial_gradient.frag", sf::Shader::Fragment);
	radial_gradient_shader.setUniform("windowsize", Window::getSizef());
	radial_gradient_shader.setUniform("center_color", sf::Vector3f(0, 0, 0));
	radial_gradient_shader.setUniform("outer_color", sf::Vector3f(1, 0, 0));
}

// //TODO:finish
// static double bass_amount_last = 1.f;
// static boost::circular_buffer<double> signalenergies(32); // store energy for 64 samples

// const bool drawcircular = true;

void update(const sf::Time &dtTime)
{
	if (Window::needs_redraw)
	{
		buildScene();
		Window::needs_redraw = false;
	}

	sf::Vector2f window_size = Window::getSizef();

	const int rectsCount = rects.size();

	// // Gradient settings
	// const int bass_start = (int)(config.audio.min_freq / g_audiosink.getFreqPerSample()); // this is clamped to [0,inf>
	// const int bass_end = std::min((int)(config.audio.max_freq / g_audiosink.getFreqPerSample()),
	//                               rectsCount); // this is clamped to <bass_start, inf> so clamp to rectcount aswell.
	// const int bass_range = bass_end - bass_start;

	// if (drawcircular)
	// {
	// 	for (int i = 0; i < rectsCount; i++)
	// 	{
	// 		const double &freq_gain = g_audiosink.Output[i];
	// 		sf::RectangleShape &r = rects[i];

	// 		const double &bar_gain = config.audio.bar_gain;
	// 		const double barheight = freq_gain * 255.f;
	// 		const double delta_deg = 360.f / rectsCount;
	// 		const double degs = delta_deg * i;
	// 		const double rads = PIf / 180.f * degs;

	// 		const double radius = 75.f;
	// 		const double x = window_size.x / 2.f + radius * cos(rads);
	// 		const double y = window_size.y / 2.f + radius * sin(rads);

	// 		const double circumference = 2.f * PIf * radius;
	// 		const double barwidth = 2.f * circumference / (double)rectsCount;

	// 		r.setSize(sf::Vector2f(barwidth, barheight));
	// 		r.setPosition(x, y);
	// 		r.setRotation(degs - 90.f);

	// 		const double fraction = (double)i / (double)rectsCount;
	// 		const sf::Color color = interpolateColor(sf::Color::Red, sf::Color::Yellow, fraction);
	// 		r.setFillColor(color);

	// 		//if (bass_start <= i && i <= bass_end)
	// 		//{
	// 		//	r.setFillColor(sf::Color::Blue);
	// 		//}
	// 		//else
	// 		//{
	// 		//	const sf::Color color = interpolateColor(sf::Color::Red, sf::Color::Yellow, fraction);
	// 		//	r.setFillColor(color);
	// 		//}
	// 	}
	// }
	// else
	// {
	// 	const double bar_maxheight = window_size.y * 7.f / 8.f;
	// 	const double bar_maxwidth = window_size.x / rects.size();

	// 	for (int i = 0; i < rectsCount; i++)
	// 	{
	// 		const double &freq_gain = g_audiosink.Output[i];
	// 		sf::RectangleShape &r = rects[i];

	// 		const double &bar_gain = config.audio.bar_gain;
	// 		const double barheight = freq_gain * bar_maxheight * bar_gain;
	// 		r.setSize(sf::Vector2f(bar_maxwidth, barheight));

	// 		const double fraction = (double)i / (double)rectsCount;

	// 		if (bass_start <= i && i <= bass_end)
	// 		{
	// 			r.setFillColor(sf::Color::Blue);
	// 		}
	// 		else
	// 		{
	// 			const sf::Color color = interpolateColor(sf::Color::Red, sf::Color::Yellow, fraction);
	// 			r.setFillColor(color);
	// 		}
	// 	}
	// }
	// // calculate energy of signal
	// double energy = 0;
	// for (int i = bass_start; i < bass_end; i++)
	// {
	// 	const double &freq_output = g_audiosink.Output[i];
	// 	energy += freq_output;
	// }
	// energy /= bass_range;
	// signalenergies.push_back(energy);

	// // calculate average
	// double average = std::accumulate(signalenergies.begin(), signalenergies.end(), 0.0f) / signalenergies.size();

	// // calculate variance
	// double variance = 0;
	// for (int i = 0; i < signalenergies.size(); i++)
	// {
	// 	const double &energy = signalenergies[i];
	// 	variance += std::powf((energy - average), 2);
	// }
	// variance /= signalenergies.size();

	// double &coeff_a = config.audio.bass_threshold_a;
	// double &coeff_b = config.audio.bass_threshold_b;

	// double threshold = coeff_a * variance + coeff_b;
	// double energythreshold = threshold * average;

	// if (energy > energythreshold)
	// {
	// 	// its a kick
	// 	gradient_strength = 1.f;
	// }
	// else
	// {
	// 	gradient_strength *= 0.997f;
	// }

	// state.debug_textvec.push_back(std::format("average energy: {:4f}", average));
	// state.debug_textvec.push_back(std::format("variance energy: {:4f}", variance));
	// state.debug_textvec.push_back(std::format("current energy: {:4f}", energy));
	// state.debug_textvec.push_back(std::format("ethreshold = a*var + b: {:4f}", energythreshold));
	// state.debug_textvec.push_back(std::format("energy-ethreshold: {:4f}", (double)(energy - energythreshold)));
	// state.debug_textvec.push_back(std::format("a*var: {:4f}", (double)(coeff_a * variance)));

	radial_gradient_shader.setUniform("strength", 0.94f * 0.00075f);

	radial_gradient_rect.setPosition(0, 0);
	radial_gradient_rect.setSize(Window::getSizef());
}

//TODO:finish
void render()
{
	// Draw the gradient quad
	p_window->draw(radial_gradient_rect, &radial_gradient_shader);

	// for (const auto &r : rects)
	// {
	// 	p_window->draw(r);
	// }
}

}; // namespace Scene