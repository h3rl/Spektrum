#include "filter.h"

namespace Filter
{
void gaussian_filter(float *in, float *out, unsigned int size, float sigma)
{
	// Compute the gaussian kernel size.
	unsigned int kernel_size = 1 + 2 * (int)(2.0f * sigma);

	// Allocate the kernel array.
	float *kernel = new float[kernel_size];

	// Compute the kernel values.
	float sum = 0.0f;
	for (int i = 0; i < kernel_size; i++)
	{
		int x = i - kernel_size / 2;
		kernel[i] = exp(-x * x / (2.0f * sigma * sigma));
		sum += kernel[i];
	}

	// Normalize the kernel.
	for (int i = 0; i < kernel_size; i++)
	{
		kernel[i] /= sum;
	}

	// Apply the filter.
	for (int i = 0; i < size; i++)
	{
		float sum = 0.0f;
		for (int j = 0; j < kernel_size; j++)
		{
			int idx = i + j - kernel_size / 2;
			if (idx < 0 || idx >= (int)size)
			{
				continue;
			}
			sum += in[idx] * kernel[j];
		}
		out[i] = sum;
	}

	// Free the kernel array.
	delete[] kernel;
}
void median_filter(float *in, float *out, unsigned int size, unsigned int window_size)
{
}

void mean_filter(float *in, float *out, unsigned int size, unsigned int window_size)
{
	// Iterate over each element in the data array.
	for (int i = 0; i < size; i++)
	{
		float sum = 0.0f;
		unsigned int count = 0;

		// Compute the mean within the window, handling edge cases.
		for (int j = -(int)window_size / 2; j <= (int)window_size / 2; j++)
		{
			int idx = i + j;

			// Handle edge cases by clamping the index to valid bounds (0 to size-1).
			if (idx < 0 && idx >= (int)size)
			{
				continue;
			}
			sum += in[idx];
			++count;
		}

		// Store the averaged value in the original array.
		out[i] = sum / count;
	}
}

void time_smoothing(float *data, float *prev_data, unsigned int size, float factor)
{
	for (unsigned int i = 0; i < size; i++)
	{
		data[i] = factor * prev_data[i] + (1.0f - factor) * data[i];
	}
}

void convert_db(float *data, unsigned int size, float min_db, float max_db)
{
	for (unsigned int i = 0; i < size; i++)
	{
		float db_out = 20.f * log10(data[i]);
		float floatval = 1.f / (max_db - min_db) * (db_out - min_db);
		data[i] = clamp(floatval, 0.f, 1.f);
	}
}

float A_50_100[] = { 1.000000, -2.973820, 2.947982, -0.974160 };
float B_50_100[] = { 0.000000, 0.000001, 0.000001, 0.000000 };
int N_50_100 = 3;
float A_50_80[] = { 1.000000, -5.991941, 14.959941, -19.920355, 14.920827, -5.960649, 0.992177 };
float B_50_80[] = { 0.000000, 0.000000, -0.000000, 0.000000, 0.000000, 0.000000, -0.000000 };
int N_50_80 = 3;

float butterworth(boost::circular_buffer<float> &in, boost::circular_buffer<float> &out, BWType type)
{
	// A, B, order
	float *A, *B, N;
	if (type == BWType::BP_50_100)
	{
		A = A_50_100;
		B = B_50_100;
		N = N_50_100;
	}
	else if (type == BWType::BP_50_80)
	{
		A = A_50_80;
		B = B_50_80;
		N = N_50_80;
	}
	else
	{
		return in[0];
	}

	float filtered = B[0] * in[0];
	for (unsigned int i = 1; i < N + 1; i++)
	{
		filtered += B[i] * in[i];
		filtered -= A[i] * out[i - 1];
	}
	filtered /= A[0];
	return filtered;
}

void lp_100hz(float *data, float *filtered, unsigned int size)
{
	// sos contains [b0, b1, b2, 1, a1, a2]
	// G contians []
	const float SOS[] = {
		1, 2, 1, 1, -1.98148850914457352878628171310992911458, 0.981658282617134170244810320582473650575
	};

	for (unsigned int i = size - 1; i > 0; i--)
	{
		filtered[i + 2] = SOS[0] * data[i + 2] + SOS[1] * data[i + 1] + SOS[2] * data[i] - SOS[4] * filtered[i + 1] -
		                  SOS[5] * filtered[i];
	}
}
} // namespace Filter