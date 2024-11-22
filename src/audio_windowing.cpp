#include "audio_windowing.h"
#include "logging.h"

// TODO: make windowing class, swapping window function should precalulate window coefficients

void apply_windowing(const AudioWindowFunction wf, float *data, size_t size)
{
	const float N = size - 1;

	if (wf == AudioWindowFunction::None || wf == AudioWindowFunction::Rect)
	{
		return;
	}
	else if (wf == AudioWindowFunction::Blackman)
	{
		static const float a0 = 0.42;
		static const float a1 = 0.5;
		static const float a2 = 0.08;
		for (int i = 0; i < size; i++)
		{
			data[i] *= a0 - a1 * cos(2.0 * PI * i / N) + a2 * cos(4.0 * PI * i / N);
		}
	}
	else if (wf == AudioWindowFunction::Hamming)
	{
		const float a0 = 25.0 / 46.0;
		const float a1 = 1.0 - a0;
		for (int i = 0; i < size; i++)
		{
			data[i] *= a0 - a1 * cos(2 * PI * i / N);
		}
	}
	else if (wf == AudioWindowFunction::Hann)
	{
		for (int i = 0; i < size; i++)
		{
			//data[i] *= 0.5 - 0.5 * cos(2 * PI * i / N);
			data[i] *= pow(sin(PI * i / N), 2);
		}
	}
	else if (wf == AudioWindowFunction::Triangle)
	{
		for (int i = 0; i < size; i++)
		{
			data[i] *= 1.0 - abs(2.0 * (i - 0.5 * N) / N);
		}
	}
	else if (wf == AudioWindowFunction::Welch)
	{
		for (int i = 0; i < size; i++)
		{
			data[i] *= 1.0 - pow((i - N / 2.f) / (N / 2.f), 2.f);
		}
	}
	else if (wf == AudioWindowFunction::FlatTop)
	{
		static const float a0 = 0.21557895;
		static const float a1 = 0.41663158;
		static const float a2 = 0.277263158;
		static const float a3 = 0.083578947;
		static const float a4 = 0.006947368;

		for (int i = 0; i < size; i++)
		{
			data[i] *= a0 - a1 * cos(2 * PI * i / N) + a2 * cos(4 * PI * i / N) - a3 * cos(6 * PI * i / N) +
			           a4 * cos(8 * PI * i / N);
		}
	}
	else if (wf == AudioWindowFunction::BlackmanHarris)
	{
		static const float a0 = 0.35875;
		static const float a1 = 0.48829;
		static const float a2 = 0.14128;
		static const float a3 = 0.01168;
		for (int i = 0; i < size; i++)
		{
			data[i] *= a0 - a1 * cos(2 * PI * i / N) + a2 * cos(4 * PI * i / N) - a3 * cos(6 * PI * i / N);
		}
	}
	else
	{
		log_warn("Unknown window function");
	}
}
