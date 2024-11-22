#pragma once
#include "stdafx.h"
#include <boost/circular_buffer.hpp>

enum class BWType : uint8_t
{
	BP_50_100,
	BP_50_80,
};

namespace Filter
{
void gaussian_filter(float *in, float *out, unsigned int size, float sigma);
void median_filter(float *in, float *out, unsigned int size, unsigned int window_size);
void mean_filter(float *in, float *out, unsigned int size, unsigned int window_size);
// void mean_filter(float *inout, unsigned int size, unsigned int window_size);

void time_smoothing(float *data, float *prev_data, unsigned int size, float factor);
void convert_db(float *data, unsigned int size, float min_db, float max_db);
void lp_100hz(float *data, unsigned int size);
float butterworth(boost::circular_buffer<float> &in, boost::circular_buffer<float> &out, BWType type);
} // namespace Filter