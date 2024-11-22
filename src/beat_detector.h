#pragma once
#include "stdafx.h"

namespace BeatDetector
{
extern unsigned int dft_size;
extern float dft_zeropad_factor;
extern float filter_factor_sigma;
extern float time_smoothing;

// Scaling/positioning
extern float offset;
extern float gain;
extern float logn;
extern float db_min;
extern float db_max;

extern float f_min;
extern float f_max;

extern float loudness;
extern float loudness_alpha;
extern float loudness_f_min;
extern float loudness_f_max;

void update();
void release();

unsigned int get_freq_idx(float freq);
void get_dft_data(float **data, unsigned int *size);

}; // namespace BeatDetector