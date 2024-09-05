#include "color.h"

sf::Color interpolateColor(sf::Color color1, sf::Color color2, double factor)
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