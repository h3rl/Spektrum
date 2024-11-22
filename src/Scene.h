#pragma once
#include "stdafx.h"

#include <vector>

enum class Scenes
{
	Histogram,
	Circle,
};

namespace Scene
{
extern Scenes scene;

void build();
void update();
void render();
}; // namespace Scene