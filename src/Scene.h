#pragma once
#include "stdafx.h"

#include <vector>

namespace Scene
{
void init();
void update(const sf::Time &dtTime);
void render();

void buildScene();

extern unsigned int bar_count;
extern double gradient_strength;
}