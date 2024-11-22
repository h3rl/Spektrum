#pragma once
#include "stdafx.h"

#define AudioWindowFunctionCombo "None\0Blackman\0Hamming\0Hann\0Rect\0Triangle\0Welch\0FlatTop\0BlackmanHarris\0\0"
enum AudioWindowFunction : int
{
	None,
	Blackman,
	Hamming,
	Hann,
	Rect,
	Triangle,
	Welch,
	FlatTop,
	BlackmanHarris
};

void apply_windowing(const AudioWindowFunction wf, float *data, size_t size);