#pragma once
#include "stdafx.h"

namespace SceneHistogram
{
extern unsigned int dft_size;
extern unsigned int bar_count;

void build();
void update();
void render();
}