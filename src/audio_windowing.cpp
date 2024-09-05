#include "audio_windowing.h"
#include "logging.h"

void apply_windowing(const AudioWindowFunction wf, double *data, size_t size)
{
	const double N = size - 1;

	if (wf == AudioWindowFunction::None || wf == AudioWindowFunction::Rect)
	{
		return;
	}
	else if (wf == AudioWindowFunction::Blackman)
	{
		static const double a0 = 0.42f;
		static const double a1 = 0.5f;
		static const double a2 = 0.08f;
		for (int i = 0; i < size; i++)
		{
			data[i] *= a0 - a1 * cos(2.f * PIf * i / N) + a2 * cos(4.f * PIf * i / N);
		}
	}
	else if (wf == AudioWindowFunction::Hamming)
	{
		for (int i = 0; i < size; i++)
		{
			data[i] *= 0.54f - 0.46f * cosf(2 * PIf * i / N);
		}
	}
	else if (wf == AudioWindowFunction::Hann)
	{
		for (int i = 0; i < size; i++)
		{
			data[i] *= 0.5f - 0.5f * cosf(2 * PIf * i / N);
		}
	}
	else if (wf == AudioWindowFunction::Triangle)
	{
		for (int i = 0; i < size; i++)
		{
			data[i] *= 1.f - fabsf(2.f * (i - 0.5f * N) / N);
		}
	}
	else if (wf == AudioWindowFunction::Welch)
	{
		for (int i = 0; i < size; i++)
		{
			data[i] *= 1.f - powf((i - N / 2.f) / (N / 2.f), 2.f);
		}
	}
	else if (wf == AudioWindowFunction::FlatTop)
	{
		static const double a0 = 0.21557895f;
		static const double a1 = 0.41663158f;
		static const double a2 = 0.277263158f;
		static const double a3 = 0.083578947f;
		static const double a4 = 0.006947368f;

		for (int i = 0; i < size; i++)
		{
			data[i] *= a0 - a1 * cosf(2 * PIf * i / N) + a2 * cosf(4 * PIf * i / N) - a3 * cosf(6 * PIf * i / N) +
			           a4 * cosf(8 * PIf * i / N);
		}
	}
	else if (wf == AudioWindowFunction::BlackmanHarris)
	{
		static const double a0 = 0.35875f;
		static const double a1 = 0.48829f;
		static const double a2 = 0.14128f;
		static const double a3 = 0.01168f;
		for (int i = 0; i < size; i++)
		{
			data[i] *= a0 - a1 * cosf(2 * PIf * i / N) + a2 * cosf(4 * PIf * i / N) - a3 * cosf(6 * PIf * i / N);
		}
	}
	else
	{
		log_warn("Unknown window function");
	}
}
