#include "beat_detector.h"
#include "logging.h"
#include "dft.h"
#include "audio_sink.h"
#include "audio_windowing.h"
#include "filter.h"

#define _logn(x, n) (log(x) / log(n))

float *zeroalloc(size_t size)
{
	float *buffer = new float[size];
	memset(buffer, 0, sizeof(float) * size);
	return buffer;
}

namespace BeatDetector
{
// Globals
unsigned int dft_size = 4096;
float dft_zeropad_factor = 0.8;
float filter_factor_sigma = 10.0;
float time_smoothing = 0.1;

float offset = 0.0f;
float gain = 1.0f;
float logn = 16.0f;
float db_min = -100.0f;
float db_max = -30.0f;

float f_min = 0.0f;
float f_max = 20000.0f;

float loudness = 0.0f;
float loudness_alpha = 0.9f;
float loudness_f_min = 50;
float loudness_f_max = 150;

// Locals
DFT dft;
//DFT dft_kick;
float *audio_data;
float *dft_data;
float *dft_tmp;
float *dft_ts_prev;

bool buffers_allocated = false;

void update()
{
	unsigned int max_size = AudioSink::get_buffer_size();

	// Limit dft size
	if (dft_size > max_size)
	{
		dft_size = max_size;
	}

	assert_msg(dft_size > 0, "BeatDetector:dft_size is 0!");

	// Check that allocation is ok
	if (!buffers_allocated)
	{
		audio_data = zeroalloc(max_size);
		dft_data = zeroalloc(max_size / 2 + 1);
		dft_tmp = zeroalloc(max_size / 2 + 1);
		dft_ts_prev =zeroalloc(max_size / 2 + 1);
		buffers_allocated = true;
	}

	//AudioSink::get_buffer(audio_data, dft_size, AudioType::Kick);
	// gives us 1 bins to look at for beat detection freq = 47-94, idx 1
	//dft_kick.execute(1024, audio_data, dft_data);

	// Original data, for visuals
	AudioSink::get_buffer(audio_data, dft_size);
	//dft_kick.execute(dft_size, audio_data, dft_data);

	// Zero padding
	if (dft_zeropad_factor < 0.9999)
	{
		float keep_factor = 1.0 - dft_zeropad_factor;
		unsigned int start_idx = dft_size * keep_factor;
		std::fill(audio_data + start_idx, audio_data + dft_size, 0.0f);
	}

	apply_windowing(AudioWindowFunction::Hann, audio_data, dft_size); // * keep_factor);

	dft.execute(dft_size, audio_data, dft_data);

	// Calculate loudness over a frequency range
	loudness_f_min = clamp(loudness_f_min, 0.0f, AudioSink::get_samplerate() / 2);
	loudness_f_max = clamp(loudness_f_max, 0.0f, AudioSink::get_samplerate() / 2);
	if (loudness_f_min > loudness_f_max)
	{
		loudness_f_min = loudness_f_max;
	}
	float new_loudness = 0;
	size_t start = get_freq_idx(loudness_f_min);
	size_t end = get_freq_idx(loudness_f_max);
	for (size_t i = start; i < end; i++)
	{
		new_loudness += dft_data[i];
	}
	new_loudness /= end - start;

	const float &a = loudness_alpha;
	loudness = a * loudness + (1 - a) * new_loudness;

	const unsigned int sz = dft_size / 2 + 1;
	// Filtering
	if (filter_factor_sigma > 0)
	{
		Filter::gaussian_filter(dft_data, dft_tmp, sz, filter_factor_sigma);
		std::copy(dft_tmp, dft_tmp + sz, dft_data);
	}

	// Time smoothing
	time_smoothing = clamp(time_smoothing, 0.f, 1.0f);
	Filter::time_smoothing(dft_data, dft_ts_prev, sz, time_smoothing);
	std::copy(dft_data, dft_data + sz, dft_ts_prev);

	// Db conversion
	if (db_min > db_max)
	{
		db_min = db_max;
	}
	Filter::convert_db(dft_data, sz, db_min, db_max);
}
void release()
{
	assert_msg(buffers_allocated, "BeatDetector::release was called, but it is not initialized!");
	delete[] audio_data;
	delete[] dft_data;
	delete[] dft_tmp;
	delete[] dft_ts_prev;
	buffers_allocated = false;
}

unsigned int get_freq_idx(float freq)
{
	return (unsigned int)(freq / AudioSink::get_samplerate() * dft_size);
}

void get_dft_data(float **data, unsigned int *size)
{
	// *data = dft_data;
	// *size = dft_size / 2 + 1;

	// Adjust to min/max freq
	f_min = clamp(f_min, 0.0f, AudioSink::get_samplerate() / 2);
	f_max = clamp(f_max, 0.0f, AudioSink::get_samplerate() / 2);

	unsigned int min_idx = get_freq_idx(f_min);
	unsigned int max_idx = get_freq_idx(f_max);
	if (max_idx < min_idx)
	{
		max_idx = min_idx;
	}

	*data = dft_data + min_idx;
	*size = max_idx - min_idx;
}
}; // namespace BeatDetector