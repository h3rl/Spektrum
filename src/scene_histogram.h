#pragma once
#include "stdafx.h"

namespace SceneHistogram
{
extern unsigned int dft_size;
extern unsigned int bar_count;

void init();
void rebuild();
void update(const sf::Time &dtTime);
void render();
}